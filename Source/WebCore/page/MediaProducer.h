/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
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

#pragma once

#include <wtf/EnumTraits.h>
#include <wtf/OptionSet.h>
#include <wtf/WeakPtr.h>

namespace WebCore {

class MediaProducer : public CanMakeWeakPtr<MediaProducer> {
public:
    enum class MediaState {
        IsPlayingAudio = 1 << 0,
        IsPlayingVideo = 1 << 1,
        DidPlayToEnd = 1 << 5,
        IsSourceElementPlaying = 1 << 6,
        IsNextTrackControlEnabled = 1 << 7,
        IsPreviousTrackControlEnabled = 1 << 8,
        HasAudioOrVideo = 1 << 10,
        HasUserInteractedWithMediaElement = 1 << 17
    };
    using MediaStateFlags = OptionSet<MediaState>;
    static constexpr MediaStateFlags IsNotPlaying = { };

    static bool isCapturing(MediaStateFlags state) { return false; }

    virtual MediaStateFlags mediaState() const = 0;

    enum class MutedState {
        AudioIsMuted = 1 << 0
    };

    virtual void pageMutedStateDidChange() = 0;

protected:
    virtual ~MediaProducer() = default;
};

} // namespace WebCore

namespace WTF {

template<> struct EnumTraits<WebCore::MediaProducer::MediaState> {
    using values = EnumValues<
        WebCore::MediaProducer::MediaState,
        WebCore::MediaProducer::MediaState::IsPlayingAudio,
        WebCore::MediaProducer::MediaState::IsPlayingVideo,
        WebCore::MediaProducer::MediaState::DidPlayToEnd,
        WebCore::MediaProducer::MediaState::IsSourceElementPlaying,
        WebCore::MediaProducer::MediaState::IsNextTrackControlEnabled,
        WebCore::MediaProducer::MediaState::IsPreviousTrackControlEnabled,
        WebCore::MediaProducer::MediaState::HasAudioOrVideo,
        WebCore::MediaProducer::MediaState::HasUserInteractedWithMediaElement
    >;
};

template<> struct EnumTraits<WebCore::MediaProducer::MutedState> {
    using values = EnumValues<
        WebCore::MediaProducer::MutedState,
        WebCore::MediaProducer::MutedState::AudioIsMuted
    >;
};

}
