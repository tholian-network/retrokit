/*
 * Copyright (C) 2019-2020 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "RemoteMediaPlayerProxy.h"

#if ENABLE(GPU_PROCESS)

#include "DataReference.h"
#include "GPUConnectionToWebProcess.h"
#include "LayerHostingContext.h"
#include "Logging.h"
#include "MediaPlayerPrivateRemoteMessages.h"
#include "RemoteAudioSourceProviderProxy.h"
#include "RemoteAudioTrackProxy.h"
#include "RemoteMediaPlayerManagerProxy.h"
#include "RemoteMediaPlayerProxyConfiguration.h"
#include "RemoteMediaResource.h"
#include "RemoteMediaResourceIdentifier.h"
#include "RemoteMediaResourceLoader.h"
#include "RemoteMediaResourceManager.h"
#include "RemoteTextTrackProxy.h"
#include "RemoteVideoTrackProxy.h"
#include "TextTrackPrivateRemoteConfiguration.h"
#include "TrackPrivateRemoteConfiguration.h"
#include "WebCoreArgumentCoders.h"
#include <WebCore/LayoutRect.h>
#include <WebCore/MediaPlayer.h>
#include <WebCore/MediaPlayerPrivate.h>
#include <WebCore/NotImplemented.h>

#if PLATFORM(COCOA)
#include <WebCore/AudioSourceProviderAVFObjC.h>
#endif

namespace WebKit {

using namespace WebCore;

RemoteMediaPlayerProxy::RemoteMediaPlayerProxy(RemoteMediaPlayerManagerProxy& manager, MediaPlayerIdentifier identifier, Ref<IPC::Connection>&& connection, MediaPlayerEnums::MediaEngineIdentifier engineIdentifier, RemoteMediaPlayerProxyConfiguration&& configuration)
    : m_id(identifier)
    , m_webProcessConnection(WTFMove(connection))
    , m_manager(makeWeakPtr(manager))
    , m_engineIdentifier(engineIdentifier)
    , m_updateCachedStateMessageTimer(RunLoop::main(), this, &RemoteMediaPlayerProxy::timerFired)
    , m_configuration(configuration)
    , m_renderingResourcesRequest(ScopedRenderingResourcesRequest::acquire())
#if !RELEASE_LOG_DISABLED
    , m_logger(manager.logger())
#endif
{
    m_typesRequiringHardwareSupport = m_configuration.mediaContentTypesRequiringHardwareSupport;
    m_player = MediaPlayer::create(*this, m_engineIdentifier);
}

RemoteMediaPlayerProxy::~RemoteMediaPlayerProxy()
{
    if (m_performTaskAtMediaTimeCompletionHandler)
        m_performTaskAtMediaTimeCompletionHandler(std::nullopt, std::nullopt);
    setShouldEnableAudioSourceProvider(false);
}

void RemoteMediaPlayerProxy::invalidate()
{
    m_updateCachedStateMessageTimer.stop();
    m_player->invalidate();
    if (m_sandboxExtension) {
        m_sandboxExtension->revoke();
        m_sandboxExtension = nullptr;
    }
    m_renderingResourcesRequest = { };
}

void RemoteMediaPlayerProxy::getConfiguration(RemoteMediaPlayerConfiguration& configuration)
{
    configuration.engineDescription = m_player->engineDescription();
    auto maxDuration = m_player->maximumDurationToCacheMediaTime();
    configuration.maximumDurationToCacheMediaTime = maxDuration ? maxDuration : 0.2;
    configuration.supportsScanning = m_player->supportsScanning();
    configuration.supportsAcceleratedRendering = m_player->supportsAcceleratedRendering();
    configuration.supportsPlayAtHostTime = m_player->supportsPlayAtHostTime();
    configuration.supportsPauseAtHostTime = m_player->supportsPauseAtHostTime();
    configuration.shouldIgnoreIntrinsicSize = m_player->shouldIgnoreIntrinsicSize();

    m_observingTimeChanges = m_player->setCurrentTimeDidChangeCallback([this, weakThis = makeWeakPtr(this)] (auto currentTime) mutable {
        if (!weakThis)
            return;

        currentTimeChanged(currentTime);
    });
}

void RemoteMediaPlayerProxy::load(URL&& url, std::optional<SandboxExtension::Handle>&& sandboxExtensionHandle, const ContentType& contentType, CompletionHandler<void(RemoteMediaPlayerConfiguration&&)>&& completionHandler)
{
    RemoteMediaPlayerConfiguration configuration;
    if (sandboxExtensionHandle) {
        m_sandboxExtension = SandboxExtension::create(WTFMove(sandboxExtensionHandle.value()));
        if (m_sandboxExtension)
            m_sandboxExtension->consume();
        else
            WTFLogAlways("Unable to create sandbox extension for media url.\n");
    }

    m_player->load(url, contentType);
    getConfiguration(configuration);
    completionHandler(WTFMove(configuration));
}

#if ENABLE(MEDIA_SOURCE)
void RemoteMediaPlayerProxy::loadMediaSource(URL&& url, const WebCore::ContentType& contentType, bool webMParserEnabled, RemoteMediaSourceIdentifier mediaSourceIdentifier, CompletionHandler<void(RemoteMediaPlayerConfiguration&&)>&& completionHandler)
{
    ASSERT(m_manager && m_manager->gpuConnectionToWebProcess());

    RemoteMediaPlayerConfiguration configuration;
    if (!m_manager || !m_manager->gpuConnectionToWebProcess()) {
        completionHandler(WTFMove(configuration));
        return;
    }

    m_mediaSourceProxy = adoptRef(*new RemoteMediaSourceProxy(*m_manager->gpuConnectionToWebProcess(), mediaSourceIdentifier, webMParserEnabled, *this));
    m_player->load(url, contentType, m_mediaSourceProxy.get());
    getConfiguration(configuration);
    completionHandler(WTFMove(configuration));
}
#endif

void RemoteMediaPlayerProxy::cancelLoad()
{
    m_updateCachedStateMessageTimer.stop();
    m_player->cancelLoad();
}

void RemoteMediaPlayerProxy::prepareToPlay()
{
    m_player->prepareToPlay();
}

void RemoteMediaPlayerProxy::play()
{
    if (m_player->movieLoadType() != WebCore::MediaPlayerEnums::MovieLoadType::LiveStream)
        startUpdateCachedStateMessageTimer();
    m_player->play();
    sendCachedState();
}

void RemoteMediaPlayerProxy::pause()
{
    m_updateCachedStateMessageTimer.stop();
    updateCachedVideoMetrics();
    m_player->pause();
    sendCachedState();
}

void RemoteMediaPlayerProxy::seek(const MediaTime& time)
{
    m_player->seek(time);
}

void RemoteMediaPlayerProxy::seekWithTolerance(const MediaTime& time, const MediaTime& negativeTolerance, const MediaTime& positiveTolerance)
{
    m_player->seekWithTolerance(time, negativeTolerance, positiveTolerance);
}

void RemoteMediaPlayerProxy::setVolume(double volume)
{
    m_player->setVolume(volume);
}

void RemoteMediaPlayerProxy::setMuted(bool muted)
{
    m_player->setMuted(muted);
}

void RemoteMediaPlayerProxy::setPreload(WebCore::MediaPlayerEnums::Preload preload)
{
    m_player->setPreload(preload);
}

void RemoteMediaPlayerProxy::setPrivateBrowsingMode(bool privateMode)
{
    m_player->setPrivateBrowsingMode(privateMode);
}

void RemoteMediaPlayerProxy::setPreservesPitch(bool preservesPitch)
{
    m_player->setPreservesPitch(preservesPitch);
}

void RemoteMediaPlayerProxy::setPitchCorrectionAlgorithm(WebCore::MediaPlayer::PitchCorrectionAlgorithm algorithm)
{
    m_player->setPitchCorrectionAlgorithm(algorithm);
}

void RemoteMediaPlayerProxy::prepareForRendering()
{
    m_player->prepareForRendering();
}

void RemoteMediaPlayerProxy::setPageIsVisible(bool visible)
{
    m_player->setPageIsVisible(visible);
}

void RemoteMediaPlayerProxy::setShouldMaintainAspectRatio(bool maintainRatio)
{
    m_player->setShouldMaintainAspectRatio(maintainRatio);
}

void RemoteMediaPlayerProxy::acceleratedRenderingStateChanged(bool renderingCanBeAccelerated)
{
    m_renderingCanBeAccelerated = renderingCanBeAccelerated;
    m_player->acceleratedRenderingStateChanged();
}

void RemoteMediaPlayerProxy::setShouldDisableSleep(bool disable)
{
    m_player->setShouldDisableSleep(disable);
}

void RemoteMediaPlayerProxy::setRate(double rate)
{
    m_player->setRate(rate);
}

void RemoteMediaPlayerProxy::didLoadingProgress(CompletionHandler<void(bool)>&& completionHandler)
{
    m_player->didLoadingProgress(WTFMove(completionHandler));
}

RefPtr<PlatformMediaResource> RemoteMediaPlayerProxy::requestResource(ResourceRequest&& request, PlatformMediaResourceLoader::LoadOptions options)
{
    ASSERT(m_manager && m_manager->gpuConnectionToWebProcess());
    if (!m_manager || !m_manager->gpuConnectionToWebProcess())
        return nullptr;

    auto& remoteMediaResourceManager = m_manager->gpuConnectionToWebProcess()->remoteMediaResourceManager();
    auto remoteMediaResourceIdentifier = RemoteMediaResourceIdentifier::generate();
    auto remoteMediaResource = RemoteMediaResource::create(remoteMediaResourceManager, *this, remoteMediaResourceIdentifier);
    remoteMediaResourceManager.addMediaResource(remoteMediaResourceIdentifier, remoteMediaResource);

    m_webProcessConnection->sendWithAsyncReply(Messages::MediaPlayerPrivateRemote::RequestResource(remoteMediaResourceIdentifier, request, options), [remoteMediaResource]() {
        remoteMediaResource->setReady(true);
    }, m_id);

    return remoteMediaResource;
}

void RemoteMediaPlayerProxy::sendH2Ping(const URL& url, CompletionHandler<void(Expected<WTF::Seconds, WebCore::ResourceError>&&)>&& completionHandler)
{
    m_webProcessConnection->sendWithAsyncReply(Messages::MediaPlayerPrivateRemote::SendH2Ping(url), WTFMove(completionHandler), m_id);
}

void RemoteMediaPlayerProxy::removeResource(RemoteMediaResourceIdentifier remoteMediaResourceIdentifier)
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::RemoveResource(remoteMediaResourceIdentifier), m_id);
}

void RemoteMediaPlayerProxy::setBufferingPolicy(MediaPlayer::BufferingPolicy policy)
{
    m_player->setBufferingPolicy(policy);
}

#if PLATFORM(IOS_FAMILY)
void RemoteMediaPlayerProxy::accessLog(CompletionHandler<void(String)>&& completionHandler)
{
    completionHandler(m_player->accessLog());
}

void RemoteMediaPlayerProxy::errorLog(CompletionHandler<void(String)>&& completionHandler)
{
    completionHandler(m_player->errorLog());
}
#endif

void RemoteMediaPlayerProxy::mediaPlayerNetworkStateChanged()
{
    updateCachedState(true);
    m_cachedState.readyState = m_player->readyState();
    m_cachedState.networkState = m_player->networkState();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::NetworkStateChanged(m_cachedState), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerReadyStateChanged()
{
    updateCachedVideoMetrics();
    updateCachedState(true);
    m_cachedState.readyState = m_player->readyState();
    m_cachedState.networkState = m_player->networkState();
    m_cachedState.duration = m_player->duration();

    m_cachedState.movieLoadType = m_player->movieLoadType();
    m_cachedState.minTimeSeekable = m_player->minTimeSeekable();
    m_cachedState.maxTimeSeekable = m_player->maxTimeSeekable();
    m_cachedState.startDate = m_player->getStartDate();
    m_cachedState.startTime = m_player->startTime();
    m_cachedState.naturalSize = m_player->naturalSize();
    m_cachedState.maxFastForwardRate = m_player->maxFastForwardRate();
    m_cachedState.minFastReverseRate = m_player->minFastReverseRate();
    m_cachedState.seekableTimeRangesLastModifiedTime = m_player->seekableTimeRangesLastModifiedTime();
    m_cachedState.liveUpdateInterval = m_player->liveUpdateInterval();
    m_cachedState.hasAvailableVideoFrame = m_player->hasAvailableVideoFrame();
    m_cachedState.canSaveMediaData = m_player->canSaveMediaData();
    m_cachedState.hasSingleSecurityOrigin = m_player->hasSingleSecurityOrigin();
    m_cachedState.didPassCORSAccessCheck = m_player->didPassCORSAccessCheck();
    m_cachedState.wouldTaintDocumentSecurityOrigin = m_player->wouldTaintOrigin(m_configuration.documentSecurityOrigin.securityOrigin());

    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::ReadyStateChanged(m_cachedState), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerVolumeChanged()
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::VolumeChanged(m_player->volume()), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerMuteChanged()
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::MuteChanged(m_player->muted()), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerTimeChanged()
{
    updateCachedState(true);
    m_cachedState.duration = m_player->duration();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::TimeChanged(m_cachedState), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerDurationChanged()
{
    updateCachedState(true);
    m_cachedState.duration = m_player->duration();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::DurationChanged(m_cachedState), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerRateChanged()
{
    updateCachedVideoMetrics();
    sendCachedState();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::RateChanged(m_player->effectiveRate()), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerEngineFailedToLoad() const
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::EngineFailedToLoad(m_player->platformErrorCode()), m_id);
}

String RemoteMediaPlayerProxy::mediaPlayerReferrer() const
{
    return m_configuration.referrer;
}

String RemoteMediaPlayerProxy::mediaPlayerUserAgent() const
{
    return m_configuration.userAgent;
}

String RemoteMediaPlayerProxy::mediaPlayerSourceApplicationIdentifier() const
{
    return m_configuration.sourceApplicationIdentifier;
}

#if PLATFORM(IOS_FAMILY)
String RemoteMediaPlayerProxy::mediaPlayerNetworkInterfaceName() const
{
    return m_configuration.networkInterfaceName;
}

void RemoteMediaPlayerProxy::mediaPlayerGetRawCookies(const URL& url, WebCore::MediaPlayerClient::GetRawCookiesCallback&& completionHandler) const
{
    m_webProcessConnection->sendWithAsyncReply(Messages::MediaPlayerPrivateRemote::GetRawCookies(url), WTFMove(completionHandler), m_id);
}
#endif

const String& RemoteMediaPlayerProxy::mediaPlayerMediaCacheDirectory() const
{
    ASSERT(m_manager && m_manager->gpuConnectionToWebProcess());
    if (!m_manager || !m_manager->gpuConnectionToWebProcess())
        return emptyString();

    return m_manager->gpuConnectionToWebProcess()->mediaCacheDirectory();
}

const Vector<WebCore::ContentType>& RemoteMediaPlayerProxy::mediaContentTypesRequiringHardwareSupport() const
{
    return m_typesRequiringHardwareSupport;
}

Vector<String> RemoteMediaPlayerProxy::mediaPlayerPreferredAudioCharacteristics() const
{
    return m_configuration.preferredAudioCharacteristics;
}

bool RemoteMediaPlayerProxy::mediaPlayerShouldUsePersistentCache() const
{
    return m_configuration.shouldUsePersistentCache;
}

bool RemoteMediaPlayerProxy::mediaPlayerIsVideo() const
{
    return m_configuration.isVideo;
}

void RemoteMediaPlayerProxy::mediaPlayerPlaybackStateChanged()
{
    m_cachedState.paused = m_player->paused();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::PlaybackStateChanged(m_cachedState.paused, m_player->currentTime(), MonotonicTime::now()), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerBufferedTimeRangesChanged()
{
    m_bufferedChanged = true;
}

void RemoteMediaPlayerProxy::mediaPlayerSeekableTimeRangesChanged()
{
    m_cachedState.minTimeSeekable = m_player->minTimeSeekable();
    m_cachedState.maxTimeSeekable = m_player->maxTimeSeekable();
    m_cachedState.seekableTimeRangesLastModifiedTime = m_player->seekableTimeRangesLastModifiedTime();
    m_cachedState.liveUpdateInterval = m_player->liveUpdateInterval();

    if (!m_updateCachedStateMessageTimer.isActive())
        sendCachedState();
}

void RemoteMediaPlayerProxy::mediaPlayerCharacteristicChanged()
{
    updateCachedVideoMetrics();
    updateCachedState();
    m_cachedState.hasAudio = m_player->hasAudio();
    m_cachedState.hasVideo = m_player->hasVideo();
    m_cachedState.hasClosedCaptions = m_player->hasClosedCaptions();
    m_cachedState.languageOfPrimaryAudioTrack = m_player->languageOfPrimaryAudioTrack();

    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::CharacteristicChanged(m_cachedState), m_id);
}

bool RemoteMediaPlayerProxy::mediaPlayerRenderingCanBeAccelerated()
{
    return m_renderingCanBeAccelerated;
}

#if !PLATFORM(COCOA)
void RemoteMediaPlayerProxy::mediaPlayerRenderingModeChanged()
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::RenderingModeChanged(), m_id);
}
#endif

TrackPrivateRemoteIdentifier RemoteMediaPlayerProxy::addRemoteAudioTrackProxy(WebCore::AudioTrackPrivate& track)
{
#if !RELEASE_LOG_DISABLED
    track.setLogger(mediaPlayerLogger(), mediaPlayerLogIdentifier());
#endif

    ASSERT(m_manager && m_manager->gpuConnectionToWebProcess());
    if (!m_manager || !m_manager->gpuConnectionToWebProcess())
        return { };

    auto identifier = TrackPrivateRemoteIdentifier::generate();
    m_audioTracks.set(&track, RemoteAudioTrackProxy::create(*m_manager->gpuConnectionToWebProcess(), identifier, track, m_id));
    return identifier;
}

void RemoteMediaPlayerProxy::audioTrackSetEnabled(const TrackPrivateRemoteIdentifier& identifier, bool enabled)
{
    for (auto& track : m_audioTracks.values()) {
        if (track->identifier() == identifier) {
            track->setEnabled(enabled);
            return;
        }
    }
}

TrackPrivateRemoteIdentifier RemoteMediaPlayerProxy::addRemoteVideoTrackProxy(WebCore::VideoTrackPrivate& track)
{
#if !RELEASE_LOG_DISABLED
    track.setLogger(mediaPlayerLogger(), mediaPlayerLogIdentifier());
#endif

    ASSERT(m_manager);
    ASSERT(m_manager->gpuConnectionToWebProcess());
    if (!m_manager || !m_manager->gpuConnectionToWebProcess())
        return { };

    auto identifier = TrackPrivateRemoteIdentifier::generate();
    m_videoTracks.set(&track, RemoteVideoTrackProxy::create(*m_manager->gpuConnectionToWebProcess(), identifier, track, m_id));
    return identifier;
}

void RemoteMediaPlayerProxy::videoTrackSetSelected(const TrackPrivateRemoteIdentifier& identifier, bool selected)
{
    for (auto& track : m_videoTracks.values()) {
        if (track->identifier() == identifier) {
            track->setSelected(selected);
            return;
        }
    }
}

TrackPrivateRemoteIdentifier RemoteMediaPlayerProxy::addRemoteTextTrackProxy(WebCore::InbandTextTrackPrivate& track)
{
#if !RELEASE_LOG_DISABLED
    track.setLogger(mediaPlayerLogger(), mediaPlayerLogIdentifier());
#endif

    ASSERT(m_manager && m_manager->gpuConnectionToWebProcess());
    if (!m_manager || !m_manager->gpuConnectionToWebProcess())
        return { };

    auto identifier = TrackPrivateRemoteIdentifier::generate();
    m_textTracks.set(&track, RemoteTextTrackProxy::create(*m_manager->gpuConnectionToWebProcess(), identifier, track, m_id));
    return identifier;
}

void RemoteMediaPlayerProxy::textTrackSetMode(const TrackPrivateRemoteIdentifier& identifier, WebCore::InbandTextTrackPrivate::Mode mode)
{
    for (auto& track : m_textTracks.values()) {
        if (track->identifier() == identifier) {
            track->setMode(mode);
            return;
        }
    }
}

void RemoteMediaPlayerProxy::mediaPlayerDidAddAudioTrack(WebCore::AudioTrackPrivate& track)
{
    addRemoteAudioTrackProxy(track);
}

void RemoteMediaPlayerProxy::mediaPlayerDidRemoveAudioTrack(WebCore::AudioTrackPrivate& track)
{
    ASSERT(m_audioTracks.contains(&track));
    if (!m_audioTracks.contains(&track))
        return;

    auto audioTrack = m_audioTracks.get(&track);
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::RemoveRemoteAudioTrack(audioTrack->identifier()), m_id);
    m_audioTracks.remove(&track);
}

void RemoteMediaPlayerProxy::mediaPlayerDidAddVideoTrack(WebCore::VideoTrackPrivate& track)
{
    addRemoteVideoTrackProxy(track);
}

void RemoteMediaPlayerProxy::mediaPlayerDidRemoveVideoTrack(WebCore::VideoTrackPrivate& track)
{
    ASSERT(m_videoTracks.contains(&track));
    if (!m_videoTracks.contains(&track))
        return;

    auto videoTrack = m_videoTracks.get(&track);
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::RemoveRemoteVideoTrack(videoTrack->identifier()), m_id);
    m_videoTracks.remove(&track);
}

void RemoteMediaPlayerProxy::mediaPlayerDidAddTextTrack(WebCore::InbandTextTrackPrivate& track)
{
    addRemoteTextTrackProxy(track);
}

void RemoteMediaPlayerProxy::mediaPlayerDidRemoveTextTrack(WebCore::InbandTextTrackPrivate& track)
{
    ASSERT(m_textTracks.contains(&track));
    if (!m_textTracks.contains(&track))
        return;

    auto textTrack = m_textTracks.get(&track);
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::RemoveRemoteTextTrack(textTrack->identifier()), m_id);
    m_textTracks.remove(&track);
}

void RemoteMediaPlayerProxy::textTrackRepresentationBoundsChanged(const IntRect&)
{
    notImplemented();
}

void RemoteMediaPlayerProxy::mediaPlayerResourceNotSupported()
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::ResourceNotSupported(), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerSizeChanged()
{
    m_cachedState.naturalSize = m_player->naturalSize();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::SizeChanged(m_cachedState.naturalSize), m_id);
}

void RemoteMediaPlayerProxy::mediaPlayerActiveSourceBuffersChanged()
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::ActiveSourceBuffersChanged(), m_id);
}

float RemoteMediaPlayerProxy::mediaPlayerContentsScale() const
{
    return m_videoContentScale;
}

bool RemoteMediaPlayerProxy::mediaPlayerPlatformVolumeConfigurationRequired() const
{
    notImplemented();
    return false;
}

CachedResourceLoader* RemoteMediaPlayerProxy::mediaPlayerCachedResourceLoader()
{
    notImplemented();
    return nullptr;
}

RefPtr<PlatformMediaResourceLoader> RemoteMediaPlayerProxy::mediaPlayerCreateResourceLoader()
{
    return adoptRef(*new RemoteMediaResourceLoader(*this));
}

bool RemoteMediaPlayerProxy::doesHaveAttribute(const AtomString&, AtomString*) const
{
    notImplemented();
    return false;
}

#if ENABLE(AVF_CAPTIONS)
Vector<RefPtr<PlatformTextTrack>> RemoteMediaPlayerProxy::outOfBandTrackSources()
{
    Vector<RefPtr<PlatformTextTrack>> sources;
    for (auto& data : m_configuration.outOfBandTrackData)
        sources.append(PlatformTextTrack::create(WTFMove(data)));
    
    return sources;
}

#endif

double RemoteMediaPlayerProxy::mediaPlayerRequestedPlaybackRate() const
{
    notImplemented();
    return 0;
}

bool RemoteMediaPlayerProxy::mediaPlayerShouldDisableSleep() const
{
    notImplemented();
    return false;
}

bool RemoteMediaPlayerProxy::mediaPlayerShouldCheckHardwareSupport() const
{
    notImplemented();
    return false;
}

void RemoteMediaPlayerProxy::startUpdateCachedStateMessageTimer()
{
    static const Seconds lessFrequentTimeupdateEventFrequency { 2000_ms };
    static const Seconds moreFrequentTimeupdateEventFrequency { 250_ms };

    if (m_updateCachedStateMessageTimer.isActive())
        return;

    auto frequency = m_observingTimeChanges ? lessFrequentTimeupdateEventFrequency : moreFrequentTimeupdateEventFrequency;
    m_updateCachedStateMessageTimer.startRepeating(frequency);
}

void RemoteMediaPlayerProxy::timerFired()
{
    sendCachedState();
}

bool RemoteMediaPlayerProxy::mediaPlayerPausedOrStalled() const
{
    return m_player->paused() || m_player->readyState() < MediaPlayer::ReadyState::HaveFutureData;
}

void RemoteMediaPlayerProxy::currentTimeChanged(const MediaTime& mediaTime)
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::CurrentTimeChanged(mediaTime, MonotonicTime::now(), !mediaPlayerPausedOrStalled()), m_id);
}

void RemoteMediaPlayerProxy::updateCachedState(bool forceCurrentTimeUpdate)
{
    if (!m_observingTimeChanges || forceCurrentTimeUpdate)
        currentTimeChanged(m_player->currentTime());

    m_cachedState.paused = m_player->paused();
    maybeUpdateCachedVideoMetrics();
    if (m_bufferedChanged) {
        m_bufferedChanged = false;
        if (m_engineIdentifier != MediaPlayerEnums::MediaEngineIdentifier::AVFoundationMSE)
            m_cachedState.bufferedRanges = *m_player->buffered();
    }
}

void RemoteMediaPlayerProxy::sendCachedState()
{
    updateCachedState();
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::UpdateCachedState(m_cachedState), m_id);
    m_cachedState.bufferedRanges.clear();
}

void RemoteMediaPlayerProxy::beginSimulatedHDCPError()
{
    m_player->beginSimulatedHDCPError();
}

void RemoteMediaPlayerProxy::endSimulatedHDCPError()
{
    m_player->endSimulatedHDCPError();
}

void RemoteMediaPlayerProxy::notifyActiveSourceBuffersChanged()
{
    m_webProcessConnection->send(Messages::MediaPlayerPrivateRemote::ActiveSourceBuffersChanged(), m_id);
}

void RemoteMediaPlayerProxy::applicationWillResignActive()
{
    m_player->applicationWillResignActive();
}

void RemoteMediaPlayerProxy::applicationDidBecomeActive()
{
    m_player->applicationDidBecomeActive();
}

void RemoteMediaPlayerProxy::notifyTrackModeChanged()
{
#if ENABLE(AVF_CAPTIONS)
    m_player->notifyTrackModeChanged();
#endif
}

void RemoteMediaPlayerProxy::tracksChanged()
{
    m_player->tracksChanged();
}

void RemoteMediaPlayerProxy::performTaskAtMediaTime(const MediaTime& taskTime, MonotonicTime messageTime, PerformTaskAtMediaTimeCompletionHandler&& completionHandler)
{
    if (m_performTaskAtMediaTimeCompletionHandler) {
        // A media player is only expected to track one pending task-at-time at once (e.g. see
        // MediaPlayerPrivateAVFoundationObjC::performTaskAtMediaTime), so cancel the existing
        // CompletionHandler.
        auto handler = WTFMove(m_performTaskAtMediaTimeCompletionHandler);
        handler(std::nullopt, std::nullopt);
    }

    auto now = MonotonicTime::now();
    auto transmissionTime = MediaTime::createWithDouble((now - messageTime).value(), 1);
    auto adjustedTaskTime = taskTime - transmissionTime;
    auto currentTime = m_player->currentTime();
    if (adjustedTaskTime <= currentTime) {
        completionHandler(currentTime, now);
        return;
    }

    m_performTaskAtMediaTimeCompletionHandler = WTFMove(completionHandler);
    m_player->performTaskAtMediaTime([this, weakThis = makeWeakPtr(this)]() mutable {
        if (!weakThis || !m_performTaskAtMediaTimeCompletionHandler)
            return;

        auto completionHandler = WTFMove(m_performTaskAtMediaTimeCompletionHandler);
        completionHandler(m_player->currentTime(), MonotonicTime::now());
    }, adjustedTaskTime);
}

void RemoteMediaPlayerProxy::wouldTaintOrigin(struct WebCore::SecurityOriginData originData, CompletionHandler<void(std::optional<bool>)>&& completionHandler)
{
    completionHandler(m_player->wouldTaintOrigin(originData.securityOrigin()));
}

void RemoteMediaPlayerProxy::setVideoPlaybackMetricsUpdateInterval(double interval)
{
    static const Seconds metricsAdvanceUpdate = 0.25_s;
    ALWAYS_LOG(LOGIDENTIFIER, interval);

    updateCachedVideoMetrics();
    m_videoPlaybackMetricsUpdateInterval = Seconds(interval);
    m_nextPlaybackQualityMetricsUpdateTime = MonotonicTime::now() + Seconds(interval) - metricsAdvanceUpdate;
}

void RemoteMediaPlayerProxy::maybeUpdateCachedVideoMetrics()
{
    if (m_cachedState.paused || !m_videoPlaybackMetricsUpdateInterval || MonotonicTime::now() < m_nextPlaybackQualityMetricsUpdateTime)
        return;

    updateCachedVideoMetrics();
}

void RemoteMediaPlayerProxy::updateCachedVideoMetrics()
{
    ALWAYS_LOG(LOGIDENTIFIER);
    m_nextPlaybackQualityMetricsUpdateTime = MonotonicTime::now() + m_videoPlaybackMetricsUpdateInterval;
    m_cachedState.videoMetrics = m_player->videoPlaybackQualityMetrics();
}

void RemoteMediaPlayerProxy::setPreferredDynamicRangeMode(DynamicRangeMode mode)
{
    if (m_player)
        m_player->setPreferredDynamicRangeMode(mode);
}

void RemoteMediaPlayerProxy::createAudioSourceProvider()
{
#if ENABLE(WEB_AUDIO) && PLATFORM(COCOA)
    if (!m_player)
        return;

    auto* provider = m_player->audioSourceProvider();
    if (!provider || !is<AudioSourceProviderAVFObjC>(provider))
        return;

    m_remoteAudioSourceProvider = RemoteAudioSourceProviderProxy::create(m_id, m_webProcessConnection.copyRef(), downcast<AudioSourceProviderAVFObjC>(*provider));
#endif
}

void RemoteMediaPlayerProxy::setShouldEnableAudioSourceProvider(bool shouldEnable)
{
#if ENABLE(WEB_AUDIO) && PLATFORM(COCOA)
    if (auto* provider = m_player->audioSourceProvider())
        provider->setClient(shouldEnable ? m_remoteAudioSourceProvider.get() : nullptr);
#endif
}

void RemoteMediaPlayerProxy::playAtHostTime(MonotonicTime time)
{
    if (m_player)
        m_player->playAtHostTime(time);
}

void RemoteMediaPlayerProxy::pauseAtHostTime(MonotonicTime time)
{
    if (m_player)
        m_player->pauseAtHostTime(time);
}

#if !RELEASE_LOG_DISABLED
WTFLogChannel& RemoteMediaPlayerProxy::logChannel() const
{
    return JOIN_LOG_CHANNEL_WITH_PREFIX(LOG_CHANNEL_PREFIX, Media);
}
#endif

} // namespace WebKit

#endif // ENABLE(GPU_PROCESS)
