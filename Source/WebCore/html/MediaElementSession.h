/*
 * Copyright (C) 2014-2021 Apple Inc. All rights reserved.
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

#pragma once

#if ENABLE(VIDEO)

#include "MediaPlayer.h"
#include "MediaProducer.h"
#include "MediaUsageInfo.h"
#include "PlatformMediaSession.h"
#include "Timer.h"
#include <memory>
#include <wtf/TypeCasts.h>

namespace WebCore {

enum class MediaSessionMainContentPurpose { MediaControls, Autoplay };
enum class MediaPlaybackState { Playing, Paused };

enum class MediaPlaybackDenialReason {
    UserGestureRequired,
    PageConsentRequired,
    InvalidState,
};

class Document;
class HTMLMediaElement;
class MediaMetadata;
class MediaSession;
class MediaSessionObserver;
class SourceBuffer;

struct MediaPositionState;

enum class MediaSessionPlaybackState : uint8_t;

class MediaElementSession final : public PlatformMediaSession {
    WTF_MAKE_FAST_ALLOCATED;
public:
    explicit MediaElementSession(HTMLMediaElement&);
    virtual ~MediaElementSession();

    void registerWithDocument(Document&);
    void unregisterWithDocument(Document&);

    void clientWillBeginAutoplaying() final;
    bool clientWillBeginPlayback() final;
    bool clientWillPausePlayback() final;

    void visibilityChanged();
    void isVisibleInViewportChanged();
    void inActiveDocumentChanged();

    Expected<void, MediaPlaybackDenialReason> playbackStateChangePermitted(MediaPlaybackState) const;
    bool autoplayPermitted() const;
    bool dataLoadingPermitted() const;
    MediaPlayer::BufferingPolicy preferredBufferingPolicy() const;
    bool fullscreenPermitted() const;
    bool pageAllowsDataLoading() const;
    bool pageAllowsPlaybackAfterResuming() const;

    MediaPlayer::Preload effectivePreloadForElement() const;
    bool allowsAutomaticMediaDataLoading() const;

    void mediaEngineUpdated();

    void resetPlaybackSessionState() override;

    void suspendBuffering() override;
    void resumeBuffering() override;
    bool bufferingSuspended() const;
    void updateBufferingPolicy() { scheduleClientDataBufferingCheck(); }

    // Restrictions to modify default behaviors.
    enum BehaviorRestrictionFlags : unsigned {
        NoRestrictions = 0,
        RequireUserGestureForLoad = 1 << 0,
        RequireUserGestureForVideoRateChange = 1 << 1,
        RequirePageConsentToLoadMedia = 1 << 3,
        RequirePageConsentToResumeMedia = 1 << 4,
        RequireUserGestureForAudioRateChange = 1 << 5,
        AutoPreloadingNotPermitted = 1 << 10,
        InvisibleAutoplayNotPermitted = 1 << 11,
        OverrideUserGestureRequirementForMainContent = 1 << 12,
        RequireUserGestureToControlControlsManager = 1 << 13,
        RequirePlaybackToControlControlsManager = 1 << 14,
        RequireUserGestureForVideoDueToLowPowerMode = 1 << 15,
        RequirePageVisibilityToPlayAudio = 1 << 16,
        AllRestrictions = ~NoRestrictions,
    };
    typedef unsigned BehaviorRestrictions;

    WEBCORE_EXPORT BehaviorRestrictions behaviorRestrictions() const { return m_restrictions; }
    WEBCORE_EXPORT void addBehaviorRestriction(BehaviorRestrictions);
    WEBCORE_EXPORT void removeBehaviorRestriction(BehaviorRestrictions);
    bool hasBehaviorRestriction(BehaviorRestrictions restriction) const { return restriction & m_restrictions; }

#if ENABLE(MEDIA_SOURCE)
    size_t maximumMediaSourceBufferSize(const SourceBuffer&) const;
#endif

    HTMLMediaElement& element() const { return m_element; }

    bool wantsToObserveViewportVisibilityForMediaControls() const;
    bool wantsToObserveViewportVisibilityForAutoplay() const;

    enum class PlaybackControlsPurpose { ControlsManager, NowPlaying, MediaSession };
    bool canShowControlsManager(PlaybackControlsPurpose) const;
    bool isLargeEnoughForMainContent(MediaSessionMainContentPurpose) const;
    bool isMainContentForPurposesOfAutoplayEvents() const;
    MonotonicTime mostRecentUserInteractionTime() const;

    bool allowsPlaybackControlsForAutoplayingAudio() const;

    static bool isMediaElementSessionMediaType(MediaType type)
    {
        return type == MediaType::Video
            || type == MediaType::Audio
            || type == MediaType::VideoAudio;
    }

    std::optional<NowPlayingInfo> nowPlayingInfo() const final;

    WEBCORE_EXPORT void updateMediaUsageIfChanged() final;
    std::optional<MediaUsageInfo> mediaUsageInfo() const { return m_mediaUsageInfo; }

#if !RELEASE_LOG_DISABLED
    const void* logIdentifier() const final { return m_logIdentifier; }
    const char* logClassName() const final { return "MediaElementSession"; }
#endif

#if ENABLE(MEDIA_SESSION)
    void didReceiveRemoteControlCommand(RemoteControlCommandType, const RemoteCommandArgument&) final;
#endif
    void metadataChanged(const RefPtr<MediaMetadata>&);
    void positionStateChanged(const std::optional<MediaPositionState>&);
    void playbackStateChanged(MediaSessionPlaybackState);
    void actionHandlersChanged();

    MediaSession* mediaSession() const;

private:

    void ensureIsObservingMediaSession();

    bool updateIsMainContent() const;
    void mainContentCheckTimerFired();

    void scheduleClientDataBufferingCheck();
    void clientDataBufferingTimerFired();
    void updateClientDataBuffering();

    void addMediaUsageManagerSessionIfNecessary();

    HTMLMediaElement& m_element;
    BehaviorRestrictions m_restrictions;

    std::optional<MediaUsageInfo> m_mediaUsageInfo;

    bool m_elementIsHiddenUntilVisibleInViewport { false };
    bool m_elementIsHiddenBecauseItWasRemovedFromDOM { false };

    MonotonicTime m_mostRecentUserInteractionTime;

    mutable bool m_isMainContent { false };
    Timer m_mainContentCheckTimer;
    Timer m_clientDataBufferingTimer;

#if !RELEASE_LOG_DISABLED
    const void* m_logIdentifier;
#endif

#if ENABLE(MEDIA_USAGE)
    bool m_haveAddedMediaUsageManagerSession { false };
#endif
    
#if ENABLE(MEDIA_SESSION)
    bool m_isScrubbing { false };
    std::unique_ptr<MediaSessionObserver> m_observer;
#endif
};

String convertEnumerationToString(const MediaPlaybackDenialReason);

} // namespace WebCore

namespace WTF {
    
template<typename Type>
struct LogArgument;

template <>
struct LogArgument<WebCore::MediaPlaybackDenialReason> {
    static String toString(const WebCore::MediaPlaybackDenialReason reason)
    {
        return convertEnumerationToString(reason);
    }
};
    
}; // namespace WTF


SPECIALIZE_TYPE_TRAITS_BEGIN(WebCore::MediaElementSession)
static bool isType(const WebCore::PlatformMediaSession& session) { return WebCore::MediaElementSession::isMediaElementSessionMediaType(session.mediaType()); }
SPECIALIZE_TYPE_TRAITS_END()

#endif // ENABLE(VIDEO)
