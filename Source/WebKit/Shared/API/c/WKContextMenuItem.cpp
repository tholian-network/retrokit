/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
#include "WKContextMenuItem.h"

#include "APIArray.h"
#include "WebContextMenuItem.h"
#include "WebContextMenuItemData.h"
#include "WKAPICast.h"
#include "WKContextMenuItemTypes.h"

WKTypeID WKContextMenuItemGetTypeID()
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(WebKit::WebContextMenuItem::APIType);
#else
    return WebKit::toAPI(API::Object::Type::Null);
#endif
}

WKContextMenuItemRef WKContextMenuItemCreateAsAction(WKContextMenuItemTag tag, WKStringRef title, bool enabled)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(&WebKit::WebContextMenuItem::create(WebKit::WebContextMenuItemData(WebCore::ActionType, WebKit::toImpl(tag), WebKit::toImpl(title)->string(), enabled, false)).leakRef());
#else
    UNUSED_PARAM(tag);
    UNUSED_PARAM(title);
    UNUSED_PARAM(enabled);
    return nullptr;
#endif
}

WKContextMenuItemRef WKContextMenuItemCreateAsCheckableAction(WKContextMenuItemTag tag, WKStringRef title, bool enabled, bool checked)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(&WebKit::WebContextMenuItem::create(WebKit::WebContextMenuItemData(WebCore::CheckableActionType, WebKit::toImpl(tag), WebKit::toImpl(title)->string(), enabled, checked)).leakRef());
#else
    UNUSED_PARAM(tag);
    UNUSED_PARAM(title);
    UNUSED_PARAM(enabled);
    UNUSED_PARAM(checked);
    return nullptr;
#endif
}

WKContextMenuItemRef WKContextMenuItemCreateAsSubmenu(WKStringRef title, bool enabled, WKArrayRef submenuItems)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(&WebKit::WebContextMenuItem::create(WebKit::toImpl(title)->string(), enabled, WebKit::toImpl(submenuItems)).leakRef());
#else
    UNUSED_PARAM(title);
    UNUSED_PARAM(enabled);
    UNUSED_PARAM(submenuItems);
    return nullptr;
#endif
}

WKContextMenuItemRef WKContextMenuItemSeparatorItem()
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(WebKit::WebContextMenuItem::separatorItem());
#else
    return nullptr;
#endif
}

WKContextMenuItemTag WKContextMenuItemGetTag(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(WebKit::toImpl(itemRef)->data().action());
#else
    UNUSED_PARAM(itemRef);
    return WebKit::toAPI(WebCore::ContextMenuItemTagNoAction);
#endif
}

WKContextMenuItemType WKContextMenuItemGetType(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(WebKit::toImpl(itemRef)->data().type());
#else
    UNUSED_PARAM(itemRef);
    return WebKit::toAPI(WebCore::ActionType);
#endif
}

WKStringRef WKContextMenuItemCopyTitle(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toCopiedAPI(WebKit::toImpl(itemRef)->data().title().impl());
#else
    UNUSED_PARAM(itemRef);
    return 0;
#endif
}

bool WKContextMenuItemGetEnabled(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toImpl(itemRef)->data().enabled();
#else
    UNUSED_PARAM(itemRef);
    return false;
#endif
}

bool WKContextMenuItemGetChecked(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toImpl(itemRef)->data().checked();
#else
    UNUSED_PARAM(itemRef);
    return false;
#endif
}

WKArrayRef WKContextMenuCopySubmenuItems(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(&WebKit::toImpl(itemRef)->submenuItemsAsAPIArray().leakRef());
#else
    UNUSED_PARAM(itemRef);
    return 0;
#endif
}

WKTypeRef WKContextMenuItemGetUserData(WKContextMenuItemRef itemRef)
{
#if ENABLE(CONTEXT_MENUS)
    return WebKit::toAPI(WebKit::toImpl(itemRef)->userData());
#else
    UNUSED_PARAM(itemRef);
    return 0;
#endif
}

void WKContextMenuItemSetUserData(WKContextMenuItemRef itemRef, WKTypeRef userDataRef)
{
#if ENABLE(CONTEXT_MENUS)
    WebKit::toImpl(itemRef)->setUserData(WebKit::toImpl(userDataRef));
#else
    UNUSED_PARAM(itemRef);
    UNUSED_PARAM(userDataRef);
#endif
}

#if PLATFORM(COCOA)

#define STATIC_ASSERT_EQUALS(a, b, c) \
    static_assert(a == b, ""); \
    static_assert(a == WebCore::c, "");

// These values must remain equal to retain binary compatibility.
STATIC_ASSERT_EQUALS(0, kWKContextMenuItemTagNoAction, ContextMenuItemTagNoAction);
STATIC_ASSERT_EQUALS(1, kWKContextMenuItemTagOpenLinkInNewWindow, ContextMenuItemTagOpenLinkInNewWindow);
STATIC_ASSERT_EQUALS(2, kWKContextMenuItemTagDownloadLinkToDisk, ContextMenuItemTagDownloadLinkToDisk);
STATIC_ASSERT_EQUALS(3, kWKContextMenuItemTagCopyLinkToClipboard, ContextMenuItemTagCopyLinkToClipboard);
STATIC_ASSERT_EQUALS(4, kWKContextMenuItemTagOpenImageInNewWindow, ContextMenuItemTagOpenImageInNewWindow);
STATIC_ASSERT_EQUALS(5, kWKContextMenuItemTagDownloadImageToDisk, ContextMenuItemTagDownloadImageToDisk);
STATIC_ASSERT_EQUALS(6, kWKContextMenuItemTagCopyImageToClipboard, ContextMenuItemTagCopyImageToClipboard);
STATIC_ASSERT_EQUALS(7, kWKContextMenuItemTagOpenFrameInNewWindow, ContextMenuItemTagOpenFrameInNewWindow);
STATIC_ASSERT_EQUALS(8, kWKContextMenuItemTagCopy, ContextMenuItemTagCopy);
STATIC_ASSERT_EQUALS(9, kWKContextMenuItemTagGoBack, ContextMenuItemTagGoBack);
STATIC_ASSERT_EQUALS(10, kWKContextMenuItemTagGoForward, ContextMenuItemTagGoForward);
STATIC_ASSERT_EQUALS(11, kWKContextMenuItemTagStop, ContextMenuItemTagStop);
STATIC_ASSERT_EQUALS(12, kWKContextMenuItemTagReload, ContextMenuItemTagReload);
STATIC_ASSERT_EQUALS(13, kWKContextMenuItemTagCut, ContextMenuItemTagCut);
STATIC_ASSERT_EQUALS(14, kWKContextMenuItemTagPaste, ContextMenuItemTagPaste);
STATIC_ASSERT_EQUALS(15, kWKContextMenuItemTagSpellingGuess, ContextMenuItemTagSpellingGuess);
STATIC_ASSERT_EQUALS(16, kWKContextMenuItemTagNoGuessesFound, ContextMenuItemTagNoGuessesFound);
STATIC_ASSERT_EQUALS(17, kWKContextMenuItemTagIgnoreSpelling, ContextMenuItemTagIgnoreSpelling);
STATIC_ASSERT_EQUALS(18, kWKContextMenuItemTagLearnSpelling, ContextMenuItemTagLearnSpelling);
STATIC_ASSERT_EQUALS(19, kWKContextMenuItemTagOther, ContextMenuItemTagOther);
STATIC_ASSERT_EQUALS(20, kWKContextMenuItemTagOpenWithDefaultApplication, ContextMenuItemTagOpenWithDefaultApplication);
STATIC_ASSERT_EQUALS(21, kWKContextMenuItemTagPDFActualSize, ContextMenuItemPDFActualSize);
STATIC_ASSERT_EQUALS(22, kWKContextMenuItemTagPDFZoomIn, ContextMenuItemPDFZoomIn);
STATIC_ASSERT_EQUALS(23, kWKContextMenuItemTagPDFZoomOut, ContextMenuItemPDFZoomOut);
STATIC_ASSERT_EQUALS(24, kWKContextMenuItemTagPDFAutoSize, ContextMenuItemPDFAutoSize);
STATIC_ASSERT_EQUALS(25, kWKContextMenuItemTagPDFSinglePage, ContextMenuItemPDFSinglePage);
STATIC_ASSERT_EQUALS(26, kWKContextMenuItemTagPDFFacingPages, ContextMenuItemPDFFacingPages);
STATIC_ASSERT_EQUALS(27, kWKContextMenuItemTagPDFContinuous, ContextMenuItemPDFContinuous);
STATIC_ASSERT_EQUALS(28, kWKContextMenuItemTagPDFNextPage, ContextMenuItemPDFNextPage);
STATIC_ASSERT_EQUALS(29, kWKContextMenuItemTagPDFPreviousPage, ContextMenuItemPDFPreviousPage);
STATIC_ASSERT_EQUALS(30, kWKContextMenuItemTagOpenLink, ContextMenuItemTagOpenLink);
STATIC_ASSERT_EQUALS(31, kWKContextMenuItemTagIgnoreGrammar, ContextMenuItemTagIgnoreGrammar);
STATIC_ASSERT_EQUALS(32, kWKContextMenuItemTagSpellingMenu, ContextMenuItemTagSpellingMenu);
STATIC_ASSERT_EQUALS(33, kWKContextMenuItemTagShowSpellingPanel, ContextMenuItemTagShowSpellingPanel);
STATIC_ASSERT_EQUALS(34, kWKContextMenuItemTagCheckSpelling, ContextMenuItemTagCheckSpelling);
STATIC_ASSERT_EQUALS(35, kWKContextMenuItemTagCheckSpellingWhileTyping, ContextMenuItemTagCheckSpellingWhileTyping);
STATIC_ASSERT_EQUALS(36, kWKContextMenuItemTagCheckGrammarWithSpelling, ContextMenuItemTagCheckGrammarWithSpelling);
STATIC_ASSERT_EQUALS(37, kWKContextMenuItemTagFontMenu, ContextMenuItemTagFontMenu);
STATIC_ASSERT_EQUALS(38, kWKContextMenuItemTagShowFonts, ContextMenuItemTagShowFonts);
STATIC_ASSERT_EQUALS(39, kWKContextMenuItemTagBold, ContextMenuItemTagBold);
STATIC_ASSERT_EQUALS(40, kWKContextMenuItemTagItalic, ContextMenuItemTagItalic);
STATIC_ASSERT_EQUALS(41, kWKContextMenuItemTagUnderline, ContextMenuItemTagUnderline);
STATIC_ASSERT_EQUALS(42, kWKContextMenuItemTagOutline, ContextMenuItemTagOutline);
STATIC_ASSERT_EQUALS(43, kWKContextMenuItemTagStyles, ContextMenuItemTagStyles);
STATIC_ASSERT_EQUALS(44, kWKContextMenuItemTagShowColors, ContextMenuItemTagShowColors);
STATIC_ASSERT_EQUALS(45, kWKContextMenuItemTagWritingDirectionMenu, ContextMenuItemTagWritingDirectionMenu);
STATIC_ASSERT_EQUALS(46, kWKContextMenuItemTagDefaultDirection, ContextMenuItemTagDefaultDirection);
STATIC_ASSERT_EQUALS(47, kWKContextMenuItemTagLeftToRight, ContextMenuItemTagLeftToRight);
STATIC_ASSERT_EQUALS(48, kWKContextMenuItemTagRightToLeft, ContextMenuItemTagRightToLeft);
STATIC_ASSERT_EQUALS(49, kWKContextMenuItemTagPDFSinglePageScrolling, ContextMenuItemTagPDFSinglePageScrolling);
STATIC_ASSERT_EQUALS(50, kWKContextMenuItemTagPDFFacingPagesScrolling, ContextMenuItemTagPDFFacingPagesScrolling);
STATIC_ASSERT_EQUALS(51, kWKContextMenuItemTagInspectElement, ContextMenuItemTagInspectElement);
STATIC_ASSERT_EQUALS(52, kWKContextMenuItemTagTextDirectionMenu, ContextMenuItemTagTextDirectionMenu);
STATIC_ASSERT_EQUALS(53, kWKContextMenuItemTagTextDirectionDefault, ContextMenuItemTagTextDirectionDefault);
STATIC_ASSERT_EQUALS(54, kWKContextMenuItemTagTextDirectionLeftToRight, ContextMenuItemTagTextDirectionLeftToRight);
STATIC_ASSERT_EQUALS(55, kWKContextMenuItemTagTextDirectionRightToLeft, ContextMenuItemTagTextDirectionRightToLeft);
STATIC_ASSERT_EQUALS(56, kWKContextMenuItemTagCorrectSpellingAutomatically, ContextMenuItemTagCorrectSpellingAutomatically);
STATIC_ASSERT_EQUALS(57, kWKContextMenuItemTagSubstitutionsMenu, ContextMenuItemTagSubstitutionsMenu);
STATIC_ASSERT_EQUALS(58, kWKContextMenuItemTagShowSubstitutions, ContextMenuItemTagShowSubstitutions);
STATIC_ASSERT_EQUALS(59, kWKContextMenuItemTagSmartCopyPaste, ContextMenuItemTagSmartCopyPaste);
STATIC_ASSERT_EQUALS(60, kWKContextMenuItemTagSmartQuotes, ContextMenuItemTagSmartQuotes);
STATIC_ASSERT_EQUALS(61, kWKContextMenuItemTagSmartDashes, ContextMenuItemTagSmartDashes);
STATIC_ASSERT_EQUALS(62, kWKContextMenuItemTagSmartLinks, ContextMenuItemTagSmartLinks);
STATIC_ASSERT_EQUALS(63, kWKContextMenuItemTagTextReplacement, ContextMenuItemTagTextReplacement);
STATIC_ASSERT_EQUALS(64, kWKContextMenuItemTagTransformationsMenu, ContextMenuItemTagTransformationsMenu);
STATIC_ASSERT_EQUALS(65, kWKContextMenuItemTagMakeUpperCase, ContextMenuItemTagMakeUpperCase);
STATIC_ASSERT_EQUALS(66, kWKContextMenuItemTagMakeLowerCase, ContextMenuItemTagMakeLowerCase);
STATIC_ASSERT_EQUALS(67, kWKContextMenuItemTagCapitalize, ContextMenuItemTagCapitalize);
STATIC_ASSERT_EQUALS(68, kWKContextMenuItemTagChangeBack, ContextMenuItemTagChangeBack);
STATIC_ASSERT_EQUALS(69, kWKContextMenuItemTagOpenMediaInNewWindow, ContextMenuItemTagOpenMediaInNewWindow);
STATIC_ASSERT_EQUALS(70, kWKContextMenuItemTagDownloadMediaToDisk, ContextMenuItemTagDownloadMediaToDisk);
STATIC_ASSERT_EQUALS(71, kWKContextMenuItemTagCopyMediaLinkToClipboard, ContextMenuItemTagCopyMediaLinkToClipboard);
STATIC_ASSERT_EQUALS(72, kWKContextMenuItemTagToggleMediaControls, ContextMenuItemTagToggleMediaControls);
STATIC_ASSERT_EQUALS(73, kWKContextMenuItemTagToggleMediaLoop, ContextMenuItemTagToggleMediaLoop);
STATIC_ASSERT_EQUALS(74, kWKContextMenuItemTagMediaPlayPause, ContextMenuItemTagMediaPlayPause);
STATIC_ASSERT_EQUALS(75, kWKContextMenuItemTagMediaMute, ContextMenuItemTagMediaMute);
STATIC_ASSERT_EQUALS(76, kWKContextMenuItemTagDictationAlternative, ContextMenuItemTagDictationAlternative);

#endif // PLATFORM(COCOA)
