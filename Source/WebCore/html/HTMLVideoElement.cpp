/*
 * Copyright (C) 2007-2020 Apple Inc. All rights reserved.
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
#include "HTMLVideoElement.h"

#if ENABLE(VIDEO)

#include "CSSPropertyNames.h"
#include "Chrome.h"
#include "ChromeClient.h"
#include "Document.h"
#include "EventNames.h"
#include "Frame.h"
#include "HTMLImageLoader.h"
#include "HTMLNames.h"
#include "HTMLParserIdioms.h"
#include "ImageBuffer.h"
#include "Logging.h"
#include "Page.h"
#include "RenderImage.h"
#include "RenderVideo.h"
#include "ScriptController.h"
#include "Settings.h"
#include <wtf/IsoMallocInlines.h>
#include <wtf/text/TextStream.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(HTMLVideoElement);

using namespace HTMLNames;

inline HTMLVideoElement::HTMLVideoElement(const QualifiedName& tagName, Document& document, bool createdByParser)
    : HTMLMediaElement(tagName, document, createdByParser)
{
    ASSERT(hasTagName(videoTag));
    setHasCustomStyleResolveCallbacks();
    m_defaultPosterURL = document.settings().defaultVideoPosterURL();
}

Ref<HTMLVideoElement> HTMLVideoElement::create(const QualifiedName& tagName, Document& document, bool createdByParser)
{
    auto videoElement = adoptRef(*new HTMLVideoElement(tagName, document, createdByParser));

    videoElement->suspendIfNeeded();
    return videoElement;
}

Ref<HTMLVideoElement> HTMLVideoElement::create(Document& document)
{
    return create(videoTag, document, false);
}

bool HTMLVideoElement::rendererIsNeeded(const RenderStyle& style)
{
    return HTMLElement::rendererIsNeeded(style); 
}

RenderPtr<RenderElement> HTMLVideoElement::createElementRenderer(RenderStyle&& style, const RenderTreePosition&)
{
    return createRenderer<RenderVideo>(*this, WTFMove(style));
}

void HTMLVideoElement::didAttachRenderers()
{
    HTMLMediaElement::didAttachRenderers();

    if (shouldDisplayPosterImage()) {
        if (!m_imageLoader)
            m_imageLoader = makeUnique<HTMLImageLoader>(*this);
        m_imageLoader->updateFromElement();
        if (auto* renderer = this->renderer())
            renderer->imageResource().setCachedImage(m_imageLoader->image());
    }
}

void HTMLVideoElement::collectPresentationalHintsForAttribute(const QualifiedName& name, const AtomString& value, MutableStyleProperties& style)
{
    if (name == widthAttr) {
        addHTMLLengthToStyle(style, CSSPropertyWidth, value);
        applyAspectRatioFromWidthAndHeightAttributesToStyle(value, attributeWithoutSynchronization(heightAttr), style);
    } else if (name == heightAttr) {
        addHTMLLengthToStyle(style, CSSPropertyHeight, value);
        applyAspectRatioFromWidthAndHeightAttributesToStyle(attributeWithoutSynchronization(widthAttr), value, style);
    } else
        HTMLMediaElement::collectPresentationalHintsForAttribute(name, value, style);
}

bool HTMLVideoElement::hasPresentationalHintsForAttribute(const QualifiedName& name) const
{
    if (name == widthAttr || name == heightAttr)
        return true;
    return HTMLMediaElement::hasPresentationalHintsForAttribute(name);
}

void HTMLVideoElement::parseAttribute(const QualifiedName& name, const AtomString& value)
{
    if (name == posterAttr) {
        if (shouldDisplayPosterImage()) {
            if (!m_imageLoader)
                m_imageLoader = makeUnique<HTMLImageLoader>(*this);
            m_imageLoader->updateFromElementIgnoringPreviousError();
        } else {
            if (auto* renderer = this->renderer()) {
                renderer->imageResource().setCachedImage(nullptr);
                renderer->updateFromElement();
            }
        }
    } else {
        HTMLMediaElement::parseAttribute(name, value);
    }
}

unsigned HTMLVideoElement::videoWidth() const
{
    if (!player())
        return 0;
    return clampToUnsigned(player()->naturalSize().width());
}

unsigned HTMLVideoElement::videoHeight() const
{
    if (!player())
        return 0;
    return clampToUnsigned(player()->naturalSize().height());
}

void HTMLVideoElement::scheduleResizeEvent()
{
    m_lastReportedVideoWidth = videoWidth();
    m_lastReportedVideoHeight = videoHeight();
    scheduleEvent(eventNames().resizeEvent);
}

void HTMLVideoElement::scheduleResizeEventIfSizeChanged()
{
    if (m_lastReportedVideoWidth == videoWidth() && m_lastReportedVideoHeight == videoHeight())
        return;
    scheduleResizeEvent();
}

bool HTMLVideoElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == posterAttr || HTMLMediaElement::isURLAttribute(attribute);
}

const AtomString& HTMLVideoElement::imageSourceURL() const
{
    const AtomString& url = attributeWithoutSynchronization(posterAttr);
    if (!stripLeadingAndTrailingHTMLSpaces(url).isEmpty())
        return url;
    return m_defaultPosterURL;
}

bool HTMLVideoElement::shouldDisplayPosterImage() const
{
    if (!showPosterFlag())
        return false;

    if (posterImageURL().isEmpty())
        return false;

    auto* renderer = this->renderer();
    if (renderer && renderer->failedToLoadPosterImage())
        return false;

    return true;
}

void HTMLVideoElement::mediaPlayerFirstVideoFrameAvailable()
{
    INFO_LOG(LOGIDENTIFIER, "m_showPoster = ", showPosterFlag());

    if (showPosterFlag())
        return;

    invalidateStyleAndLayerComposition();

    if (auto player = this->player())
        player->prepareForRendering();

    if (auto* renderer = this->renderer())
        renderer->updateFromElement();
}

RefPtr<ImageBuffer> HTMLVideoElement::createBufferForPainting(const FloatSize& size, RenderingMode renderingMode, const DestinationColorSpace& colorSpace, PixelFormat pixelFormat) const
{
    auto* hostWindow = document().view() && document().view()->root() ? document().view()->root()->hostWindow() : nullptr;
    auto shouldUseDisplayList = document().settings().displayListDrawingEnabled() ? ShouldUseDisplayList::Yes : ShouldUseDisplayList::No;
    return ImageBuffer::create(size, renderingMode, shouldUseDisplayList, RenderingPurpose::MediaPainting, 1, colorSpace, pixelFormat, hostWindow);
}

void HTMLVideoElement::paintCurrentFrameInContext(GraphicsContext& context, const FloatRect& destRect)
{
    RefPtr<MediaPlayer> player = HTMLMediaElement::player();
    if (!player)
        return;

    player->setVisibleForCanvas(true); // Make player visible or it won't draw.
    context.paintFrameForMedia(*player, destRect);
}

bool HTMLVideoElement::hasAvailableVideoFrame() const
{
    if (!player())
        return false;

    return player()->hasVideo() && player()->hasAvailableVideoFrame();
}

RefPtr<NativeImage> HTMLVideoElement::nativeImageForCurrentTime()
{
    if (!player())
        return nullptr;

    return player()->nativeImageForCurrentTime();
}

void HTMLVideoElement::didMoveToNewDocument(Document& oldDocument, Document& newDocument)
{
    if (m_imageLoader)
        m_imageLoader->elementDidMoveToNewDocument(oldDocument);
    HTMLMediaElement::didMoveToNewDocument(oldDocument, newDocument);
}

#if ENABLE(MEDIA_STATISTICS)
unsigned HTMLVideoElement::webkitDecodedFrameCount() const
{
    if (!player())
        return 0;

    return player()->decodedFrameCount();
}

unsigned HTMLVideoElement::webkitDroppedFrameCount() const
{
    if (!player())
        return 0;

    return player()->droppedFrameCount();
}
#endif

URL HTMLVideoElement::posterImageURL() const
{
    String url = stripLeadingAndTrailingHTMLSpaces(imageSourceURL());
    if (url.isEmpty())
        return URL();
    return document().completeURL(url);
}

}

#endif
