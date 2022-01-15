/*
 * Copyright (C) 2013, 2014 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#if ENABLE(VIDEO)

#include "MediaControlsHost.h"

#include "AddEventListenerOptions.h"
#include "AudioTrackList.h"
#include "CaptionUserPreferences.h"
#include "Chrome.h"
#include "ChromeClient.h"
#include "ContextMenu.h"
#include "ContextMenuController.h"
#include "ContextMenuItem.h"
#include "ContextMenuProvider.h"
#include "Event.h"
#include "EventListener.h"
#include "EventNames.h"
#include "EventTarget.h"
#include "FloatRect.h"
#include "HTMLElement.h"
#include "HTMLMediaElement.h"
#include "HTMLVideoElement.h"
#include "LocalizedStrings.h"
#include "Logging.h"
#include "MediaControlTextTrackContainerElement.h"
#include "MediaControlsContextMenuItem.h"
#include "Node.h"
#include "Page.h"
#include "PageGroup.h"
#include "RenderTheme.h"
#include "TextTrack.h"
#include "TextTrackCueList.h"
#include "TextTrackList.h"
#include "UserGestureIndicator.h"
#include "VTTCue.h"
#include "VoidCallback.h"
#include <JavaScriptCore/JSCJSValueInlines.h>
#include <wtf/Function.h>
#include <wtf/JSONValues.h>
#include <wtf/Scope.h>
#include <wtf/UUID.h>
#include <wtf/Variant.h>

#if USE(APPLE_INTERNAL_SDK)
#include <WebKitAdditions/MediaControlsHostAdditions.h>
#endif

namespace WebCore {

const AtomString& MediaControlsHost::automaticKeyword()
{
    static MainThreadNeverDestroyed<const AtomString> automatic("automatic", AtomString::ConstructFromLiteral);
    return automatic;
}

const AtomString& MediaControlsHost::forcedOnlyKeyword()
{
    static MainThreadNeverDestroyed<const AtomString> forcedOnly("forced-only", AtomString::ConstructFromLiteral);
    return forcedOnly;
}

static const AtomString& alwaysOnKeyword()
{
    static MainThreadNeverDestroyed<const AtomString> alwaysOn("always-on", AtomString::ConstructFromLiteral);
    return alwaysOn;
}

static const AtomString& manualKeyword()
{
    static MainThreadNeverDestroyed<const AtomString> alwaysOn("manual", AtomString::ConstructFromLiteral);
    return alwaysOn;
}

Ref<MediaControlsHost> MediaControlsHost::create(HTMLMediaElement& mediaElement)
{
    return adoptRef(*new MediaControlsHost(mediaElement));
}

MediaControlsHost::MediaControlsHost(HTMLMediaElement& mediaElement)
    : m_mediaElement(makeWeakPtr(mediaElement))
{
}

MediaControlsHost::~MediaControlsHost()
{
#if ENABLE(MEDIA_CONTROLS_CONTEXT_MENUS)
    if (auto showMediaControlsContextMenuCallback = std::exchange(m_showMediaControlsContextMenuCallback, nullptr))
        showMediaControlsContextMenuCallback->handleEvent();
#endif // ENABLE(MEDIA_CONTROLS_CONTEXT_MENUS)
}

String MediaControlsHost::layoutTraitsClassName() const
{
#if defined(MEDIA_CONTROLS_HOST_LAYOUT_TRAITS_CLASS_NAME_OVERRIDE)
    return MEDIA_CONTROLS_HOST_LAYOUT_TRAITS_CLASS_NAME_OVERRIDE;
#else
#if PLATFORM(MAC) || PLATFORM(MACCATALYST)
    return "MacOSLayoutTraits";
#elif PLATFORM(IOS)
    return "IOSLayoutTraits";
#else
    ASSERT_NOT_REACHED();
    return nullString();
#endif
#endif
}

Vector<RefPtr<TextTrack>> MediaControlsHost::sortedTrackListForMenu(TextTrackList& trackList)
{
    if (!m_mediaElement)
        return { };

    Page* page = m_mediaElement->document().page();
    if (!page)
        return { };

    return page->group().ensureCaptionPreferences().sortedTrackListForMenu(&trackList, { TextTrack::Kind::Subtitles, TextTrack::Kind::Captions, TextTrack::Kind::Descriptions });
}

Vector<RefPtr<AudioTrack>> MediaControlsHost::sortedTrackListForMenu(AudioTrackList& trackList)
{
    if (!m_mediaElement)
        return { };

    Page* page = m_mediaElement->document().page();
    if (!page)
        return { };

    return page->group().ensureCaptionPreferences().sortedTrackListForMenu(&trackList);
}

String MediaControlsHost::displayNameForTrack(const std::optional<TextOrAudioTrack>& track)
{
    if (!m_mediaElement || !track)
        return emptyString();

    Page* page = m_mediaElement->document().page();
    if (!page)
        return emptyString();

    return WTF::visit([page] (auto& track) {
        return page->group().ensureCaptionPreferences().displayNameForTrack(track.get());
    }, track.value());
}

TextTrack& MediaControlsHost::captionMenuOffItem()
{
    return TextTrack::captionMenuOffItem();
}

TextTrack& MediaControlsHost::captionMenuAutomaticItem()
{
    return TextTrack::captionMenuAutomaticItem();
}

AtomString MediaControlsHost::captionDisplayMode() const
{
    if (!m_mediaElement)
        return emptyAtom();

    Page* page = m_mediaElement->document().page();
    if (!page)
        return emptyAtom();

    switch (page->group().ensureCaptionPreferences().captionDisplayMode()) {
    case CaptionUserPreferences::Automatic:
        return automaticKeyword();
    case CaptionUserPreferences::ForcedOnly:
        return forcedOnlyKeyword();
    case CaptionUserPreferences::AlwaysOn:
        return alwaysOnKeyword();
    case CaptionUserPreferences::Manual:
        return manualKeyword();
    default:
        ASSERT_NOT_REACHED();
        return emptyAtom();
    }
}

void MediaControlsHost::setSelectedTextTrack(TextTrack* track)
{
    if (m_mediaElement)
        m_mediaElement->setSelectedTextTrack(track);
}

Element* MediaControlsHost::textTrackContainer()
{
    if (!m_textTrackContainer && m_mediaElement)
        m_textTrackContainer = MediaControlTextTrackContainerElement::create(m_mediaElement->document(), *m_mediaElement);

    return m_textTrackContainer.get();
}

void MediaControlsHost::updateTextTrackContainer()
{
    if (m_textTrackContainer)
        m_textTrackContainer->updateDisplay();
}

void MediaControlsHost::updateTextTrackRepresentationImageIfNeeded()
{
    if (m_textTrackContainer)
        m_textTrackContainer->updateTextTrackRepresentationImageIfNeeded();
}

void MediaControlsHost::updateCaptionDisplaySizes(ForceUpdate force)
{
    if (m_textTrackContainer)
        m_textTrackContainer->updateSizes(force == ForceUpdate::Yes ? MediaControlTextTrackContainerElement::ForceUpdate::Yes : MediaControlTextTrackContainerElement::ForceUpdate::No);
}

bool MediaControlsHost::allowsInlineMediaPlayback() const
{
    return m_mediaElement && true;
}

bool MediaControlsHost::isVideoLayerInline() const
{
    return m_mediaElement && m_mediaElement->isVideoLayerInline();
}

bool MediaControlsHost::isInMediaDocument() const
{
    return m_mediaElement && m_mediaElement->document().isMediaDocument();
}

bool MediaControlsHost::userGestureRequired() const
{
    return m_mediaElement && !m_mediaElement->mediaSession().playbackStateChangePermitted(MediaPlaybackState::Playing);
}

bool MediaControlsHost::shouldForceControlsDisplay() const
{
    return m_mediaElement && m_mediaElement->shouldForceControlsDisplay();
}

bool MediaControlsHost::controlsDependOnPageScaleFactor() const
{
    return m_mediaElement && m_mediaElement->mediaControlsDependOnPageScaleFactor();
}

void MediaControlsHost::setControlsDependOnPageScaleFactor(bool value)
{
    if (m_mediaElement)
        m_mediaElement->setMediaControlsDependOnPageScaleFactor(value);
}

String MediaControlsHost::generateUUID()
{
    return createCanonicalUUIDString();
}

} // namespace WebCore

#endif // ENABLE(VIDEO)
