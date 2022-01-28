/*
 * Copyright (C) 2010-2021 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
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
#include "WebPage.h"

#include "APIArray.h"
#include "APIGeometry.h"
#include "DataReference.h"
#include "DragControllerAction.h"
#include "DrawingArea.h"
#include "DrawingAreaMessages.h"
#include "EditorState.h"
#include "EventDispatcher.h"
#include "FindController.h"
#include "FormDataReference.h"
#include "FrameTreeNodeData.h"
#include "InjectUserScriptImmediately.h"
#include "InjectedBundle.h"
#include "InjectedBundleScriptWorld.h"
#include "LoadParameters.h"
#include "Logging.h"
#include "MediaPlaybackState.h"
#include "NetworkConnectionToWebProcessMessages.h"
#include "NetworkProcessConnection.h"
#include "NotificationPermissionRequestManager.h"
#include "PageBanner.h"
#include "PrintInfo.h"
#include "RemoteRenderingBackendProxy.h"
#include "RemoteWebInspectorUI.h"
#include "RemoteWebInspectorUIMessages.h"
#include "SessionState.h"
#include "SessionStateConversion.h"
#include "ShareableBitmap.h"
#include "ShareableBitmapUtilities.h"
#include "SharedBufferDataReference.h"
#include "TextRecognitionUpdateResult.h"
#include "ViewGestureGeometryCollector.h"
#include "VisitedLinkTableController.h"
#include "WKBundleAPICast.h"
#include "WKRetainPtr.h"
#include "WKSharedAPICast.h"
#include "WebAlternativeTextClient.h"
#include "WebBackForwardListItem.h"
#include "WebBackForwardListProxy.h"
#include "WebBroadcastChannelRegistry.h"
#include "WebCacheStorageProvider.h"
#include "WebChromeClient.h"
#include "WebColorChooser.h"
#include "WebContextMenu.h"
#include "WebContextMenuClient.h"
#include "WebCookieJar.h"
#include "WebCoreArgumentCoders.h"
#include "WebDataListSuggestionPicker.h"
#include "WebDatabaseProvider.h"
#include "WebDateTimeChooser.h"
#include "WebDiagnosticLoggingClient.h"
#include "WebDocumentLoader.h"
#include "WebDragClient.h"
#include "WebEditorClient.h"
#include "WebEventConversion.h"
#include "WebEventFactory.h"
#include "WebFrame.h"
#include "WebFrameLoaderClient.h"
#include "WebImage.h"
#include "WebInspector.h"
#include "WebInspectorClient.h"
#include "WebInspectorMessages.h"
#include "WebInspectorUI.h"
#include "WebInspectorUIMessages.h"
#include "WebKeyboardEvent.h"
#include "WebLoaderStrategy.h"
#include "WebMediaStrategy.h"
#include "WebMouseEvent.h"
#include "WebNotificationClient.h"
#include "WebOpenPanelResultListener.h"
#include "WebPageCreationParameters.h"
#include "WebPageGroupProxy.h"
#include "WebPageInspectorTargetController.h"
#include "WebPageMessages.h"
#include "WebPageOverlay.h"
#include "WebPageProxyMessages.h"
#include "WebPerformanceLoggingClient.h"
#include "WebPermissionController.h"
#include "WebPopupMenu.h"
#include "WebPreferencesDefinitions.h"
#include "WebPreferencesKeys.h"
#include "WebPreferencesStore.h"
#include "WebProcess.h"
#include "WebProcessPoolMessages.h"
#include "WebProcessProxyMessages.h"
#include "WebProgressTrackerClient.h"
#include "WebServiceWorkerProvider.h"
#include "WebSocketProvider.h"
#include "WebStorageNamespaceProvider.h"
#include "WebTouchEvent.h"
#include "WebURLSchemeHandlerProxy.h"
#include "WebUndoStep.h"
#include "WebUserContentController.h"
#include "WebValidationMessageClient.h"
#include "WebWheelEvent.h"
#include "WebsiteDataStoreParameters.h"
#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/JSCJSValue.h>
#include <JavaScriptCore/JSLock.h>
#include <JavaScriptCore/ProfilerDatabase.h>
#include <JavaScriptCore/SamplingProfiler.h>
#include <WebCore/AppHighlight.h>
#include <WebCore/ApplicationCacheStorage.h>
#include <WebCore/ArchiveResource.h>
#include <WebCore/BackForwardCache.h>
#include <WebCore/BackForwardController.h>
#include <WebCore/CachedPage.h>
#include <WebCore/Chrome.h>
#include <WebCore/CommonVM.h>
#include <WebCore/ContactsRequestData.h>
#include <WebCore/ContextMenuController.h>
#include <WebCore/DOMPasteAccess.h>
#include <WebCore/DataTransfer.h>
#include <WebCore/DatabaseManager.h>
#include <WebCore/DeprecatedGlobalSettings.h>
#include <WebCore/Document.h>
#include <WebCore/DocumentFragment.h>
#include <WebCore/DocumentLoader.h>
#include <WebCore/DocumentMarkerController.h>
#include <WebCore/DocumentStorageAccess.h>
#include <WebCore/DragController.h>
#include <WebCore/DragData.h>
#include <WebCore/Editing.h>
#include <WebCore/Editor.h>
#include <WebCore/ElementIterator.h>
#include <WebCore/EventHandler.h>
#include <WebCore/EventNames.h>
#include <WebCore/File.h>
#include <WebCore/FocusController.h>
#include <WebCore/FontAttributeChanges.h>
#include <WebCore/FontAttributes.h>
#include <WebCore/FormState.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameLoadRequest.h>
#include <WebCore/FrameLoaderTypes.h>
#include <WebCore/FrameView.h>
#include <WebCore/GeometryUtilities.h>
#include <WebCore/HTMLAttachmentElement.h>
#include <WebCore/HTMLFormElement.h>
#include <WebCore/HTMLImageElement.h>
#include <WebCore/HTMLInputElement.h>
#include <WebCore/HTMLMenuElement.h>
#include <WebCore/HTMLMenuItemElement.h>
#include <WebCore/HTMLSelectElement.h>
#include <WebCore/HTMLTextFormControlElement.h>
#include <WebCore/Highlight.h>
#include <WebCore/HighlightRegister.h>
#include <WebCore/HistoryController.h>
#include <WebCore/HistoryItem.h>
#include <WebCore/HitTestResult.h>
#include <WebCore/InspectorController.h>
#include <WebCore/JSDOMExceptionHandling.h>
#include <WebCore/JSDOMWindow.h>
#include <WebCore/KeyboardEvent.h>
#include <WebCore/LegacySchemeRegistry.h>
#include <WebCore/LocalizedStrings.h>
#include <WebCore/MIMETypeRegistry.h>
#include <WebCore/MouseEvent.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/Page.h>
#include <WebCore/PageConfiguration.h>
#include <WebCore/PingLoader.h>
#include <WebCore/PlatformKeyboardEvent.h>
#include <WebCore/PlatformMediaSessionManager.h>
#include <WebCore/PlatformStrategies.h>
#include <WebCore/PointerCaptureController.h>
#include <WebCore/PrintContext.h>
#include <WebCore/PromisedAttachmentInfo.h>
#include <WebCore/Range.h>
#include <WebCore/RegistrableDomain.h>
#include <WebCore/RemoteDOMWindow.h>
#include <WebCore/RemoteFrame.h>
#include <WebCore/RenderImage.h>
#include <WebCore/RenderLayer.h>
#include <WebCore/RenderTheme.h>
#include <WebCore/RenderTreeAsText.h>
#include <WebCore/RenderView.h>
#include <WebCore/ResourceLoadStatistics.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <WebCore/RunJavaScriptParameters.h>
#include <WebCore/RuntimeEnabledFeatures.h>
#include <WebCore/SWClientConnection.h>
#include <WebCore/ScriptController.h>
#include <WebCore/SecurityPolicy.h>
#include <WebCore/SelectionRestorationMode.h>
#include <WebCore/SerializedScriptValue.h>
#include <WebCore/Settings.h>
#include <WebCore/ShadowRoot.h>
#include <WebCore/SharedBuffer.h>
#include <WebCore/StaticRange.h>
#include <WebCore/StyleProperties.h>
#include <WebCore/SubframeLoader.h>
#include <WebCore/SubstituteData.h>
#include <WebCore/TextIterator.h>
#include <WebCore/UserContentURLPattern.h>
#include <WebCore/UserGestureIndicator.h>
#include <WebCore/UserInputBridge.h>
#include <WebCore/UserScript.h>
#include <WebCore/UserStyleSheet.h>
#include <WebCore/UserTypingGestureIndicator.h>
#include <WebCore/VisiblePosition.h>
#include <WebCore/VisibleUnits.h>
#include <WebCore/WritingDirection.h>
#include <WebCore/markup.h>
#include <pal/SessionID.h>
#include <wtf/ProcessID.h>
#include <wtf/RunLoop.h>
#include <wtf/Scope.h>
#include <wtf/SetForScope.h>
#include <wtf/SystemTracing.h>
#include <wtf/text/TextStream.h>

#if ENABLE(APP_HIGHLIGHTS)
#include <WebCore/AppHighlightStorage.h>
#endif

#if ENABLE(DATA_DETECTION)
#include "DataDetectionResult.h"
#endif

#if ENABLE(MHTML)
#include <WebCore/MHTMLArchive.h>
#endif

#if ENABLE(POINTER_LOCK)
#include <WebCore/PointerLockController.h>
#endif

#if PLATFORM(COCOA)
#include "DefaultWebBrowserChecks.h"
#include "InsertTextOptions.h"
#include "PlaybackSessionManager.h"
#include "RemoteLayerTreeTransaction.h"
#include "RemoteObjectRegistryMessages.h"
#include "TextCheckingControllerProxy.h"
#include "WKStringCF.h"
#include "WebRemoteObjectRegistry.h"
#include <WebCore/LegacyWebArchive.h>
#include <WebCore/UTIRegistry.h>
#include <wtf/MachSendRight.h>
#include <wtf/spi/darwin/SandboxSPI.h>
#endif

#if PLATFORM(GTK)
#include "GtkSettingsManagerProxy.h"
#include "WebPrintOperationGtk.h"
#include <WebCore/SelectionData.h>
#include <gtk/gtk.h>
#endif

#if PLATFORM(IOS_FAMILY)
#include "InteractionInformationAtPosition.h"
#include "InteractionInformationRequest.h"
#include "RemoteLayerTreeDrawingArea.h"
#include "WebAutocorrectionContext.h"
#include <CoreGraphics/CoreGraphics.h>
#include <WebCore/Icon.h>
#include <pal/spi/cf/CoreTextSPI.h>
#endif

#if PLATFORM(MAC)
#include <WebCore/LocalDefaultSystemAppearance.h>
#include <pal/spi/cf/CFUtilitiesSPI.h>
#endif

#ifndef NDEBUG
#include <wtf/RefCountedLeakCounter.h>
#endif

#if ENABLE(DATA_DETECTION)
#include <WebCore/DataDetection.h>
#include <WebCore/DataDetectionResultsStorage.h>
#endif

#if ENABLE(VIDEO) && USE(GSTREAMER)
#include <WebCore/MediaPlayerRequestInstallMissingPluginsCallback.h>
#endif

#if ENABLE(WEB_AUTHN)
#include "WebAuthenticatorCoordinator.h"
#include <WebCore/AuthenticatorCoordinator.h>
#endif

#if ENABLE(GPU_PROCESS)
#include "RemoteMediaPlayerManager.h"
#if HAVE(AVASSETREADER)
#include "RemoteImageDecoderAVFManager.h"
#endif
#endif

#if ENABLE(IMAGE_ANALYSIS)
#include <WebCore/TextRecognitionResult.h>
#endif

#if ENABLE(MEDIA_SESSION_COORDINATOR)
#include "RemoteMediaSessionCoordinator.h"
#include <WebCore/MediaSessionCoordinator.h>
#include <WebCore/NavigatorMediaSession.h>
#endif

#if PLATFORM(IOS)
#include "WebPreferencesDefaultValuesIOS.h"
#endif

#if USE(CG)
// FIXME: Move the CG-specific PDF painting code out of WebPage.cpp.
#include <WebCore/GraphicsContextCG.h>
#endif

namespace WebKit {
using namespace JSC;
using namespace WebCore;

static const Seconds pageScrollHysteresisDuration { 300_ms };
static const Seconds initialLayerVolatilityTimerInterval { 20_ms };
static const Seconds maximumLayerVolatilityTimerInterval { 2_s };

#define WEBPAGE_RELEASE_LOG(channel, fmt, ...) RELEASE_LOG(channel, "%p - [webPageID=%" PRIu64 "] WebPage::" fmt, this, m_identifier.toUInt64(), ##__VA_ARGS__)
#define WEBPAGE_RELEASE_LOG_ERROR(channel, fmt, ...) RELEASE_LOG_ERROR(channel, "%p - [webPageID=%" PRIu64 "] WebPage::" fmt, this, m_identifier.toUInt64(), ##__VA_ARGS__)

class SendStopResponsivenessTimer {
public:
    ~SendStopResponsivenessTimer()
    {
        WebProcess::singleton().parentProcessConnection()->send(Messages::WebProcessProxy::StopResponsivenessTimer(), 0);
    }
};

class DeferredPageDestructor {
public:
    static void createDeferredPageDestructor(std::unique_ptr<Page> page, WebPage* webPage)
    {
        new DeferredPageDestructor(WTFMove(page), webPage);
    }

private:
    DeferredPageDestructor(std::unique_ptr<Page> page, WebPage* webPage)
        : m_page(WTFMove(page))
        , m_webPage(webPage)
    {
        tryDestruction();
    }

    void tryDestruction()
    {
        if (m_page->insideNestedRunLoop()) {
            m_page->whenUnnested([this] { tryDestruction(); });
            return;
        }

        m_page = nullptr;
        m_webPage = nullptr;
        delete this;
    }

    std::unique_ptr<Page> m_page;
    RefPtr<WebPage> m_webPage;
};

DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webPageCounter, ("WebPage"));

Ref<WebPage> WebPage::create(PageIdentifier pageID, WebPageCreationParameters&& parameters)
{
    Ref<WebPage> page = adoptRef(*new WebPage(pageID, WTFMove(parameters)));

    if (WebProcess::singleton().injectedBundle())
        WebProcess::singleton().injectedBundle()->didCreatePage(page.ptr());

    return page;
}

static Vector<UserContentURLPattern> parseAndAllowAccessToCORSDisablingPatterns(const Vector<String>& input)
{
    Vector<UserContentURLPattern> parsedPatterns;
    parsedPatterns.reserveInitialCapacity(input.size());
    for (const auto& pattern : input) {
        UserContentURLPattern parsedPattern(pattern);
        if (parsedPattern.isValid()) {
            WebCore::SecurityPolicy::allowAccessTo(parsedPattern);
            parsedPatterns.uncheckedAppend(WTFMove(parsedPattern));
        }
    }
    parsedPatterns.shrinkToFit();
    return parsedPatterns;
}

WebPage::WebPage(PageIdentifier pageID, WebPageCreationParameters&& parameters)
    : m_identifier(pageID)
    , m_mainFrame(WebFrame::create())
    , m_viewSize(parameters.viewSize)
    , m_drawingAreaType(parameters.drawingAreaType)
    , m_alwaysShowsHorizontalScroller { parameters.alwaysShowsHorizontalScroller }
    , m_alwaysShowsVerticalScroller { parameters.alwaysShowsVerticalScroller }
    , m_shouldRenderCanvasInGPUProcess { parameters.shouldRenderCanvasInGPUProcess }
    , m_shouldRenderDOMInGPUProcess { parameters.shouldRenderDOMInGPUProcess }
    , m_shouldPlayMediaInGPUProcess { parameters.shouldPlayMediaInGPUProcess }
    , m_layerHostingMode(parameters.layerHostingMode)
#if ENABLE(PLATFORM_DRIVEN_TEXT_CHECKING)
    , m_textCheckingControllerProxy(makeUniqueRef<TextCheckingControllerProxy>(*this))
#endif
#if PLATFORM(COCOA) || PLATFORM(GTK)
    , m_viewGestureGeometryCollector(makeUnique<ViewGestureGeometryCollector>(*this))
#elif ENABLE(ACCESSIBILITY) && PLATFORM(GTK)
    , m_accessibilityObject(nullptr)
#endif
#if PLATFORM(WIN)
    , m_nativeWindowHandle(parameters.nativeWindowHandle)
#endif
    , m_setCanStartMediaTimer(RunLoop::main(), this, &WebPage::setCanStartMediaTimerFired)
#if ENABLE(CONTEXT_MENUS)
    , m_contextMenuClient(makeUnique<API::InjectedBundle::PageContextMenuClient>())
#endif
    , m_editorClient { makeUnique<API::InjectedBundle::EditorClient>() }
    , m_formClient(makeUnique<API::InjectedBundle::FormClient>())
    , m_loaderClient(makeUnique<API::InjectedBundle::PageLoaderClient>())
    , m_resourceLoadClient(makeUnique<API::InjectedBundle::ResourceLoadClient>())
    , m_uiClient(makeUnique<API::InjectedBundle::PageUIClient>())
    , m_findController(makeUniqueRef<FindController>(this))
    , m_inspectorTargetController(makeUnique<WebPageInspectorTargetController>(*this))
    , m_userContentController(WebUserContentController::getOrCreate(parameters.userContentControllerParameters.identifier))
    , m_pageScrolledHysteresis([this](PAL::HysteresisState state) { if (state == PAL::HysteresisState::Stopped) pageStoppedScrolling(); }, pageScrollHysteresisDuration)
    , m_canRunBeforeUnloadConfirmPanel(parameters.canRunBeforeUnloadConfirmPanel)
    , m_canRunModal(parameters.canRunModal)
#if ENABLE(META_VIEWPORT)
    , m_forceAlwaysUserScalable(parameters.ignoresViewportScaleLimits)
#endif
#if PLATFORM(IOS_FAMILY)
    , m_screenSize(parameters.screenSize)
    , m_availableScreenSize(parameters.availableScreenSize)
    , m_overrideScreenSize(parameters.overrideScreenSize)
    , m_keyboardIsAttached(parameters.keyboardIsAttached)
    , m_canShowWhileLocked(parameters.canShowWhileLocked)
#endif
    , m_layerVolatilityTimer(*this, &WebPage::layerVolatilityTimerFired)
    , m_activityState(parameters.activityState)
    , m_userActivity("App nap disabled for page due to user activity")
    , m_userInterfaceLayoutDirection(parameters.userInterfaceLayoutDirection)
    , m_overrideContentSecurityPolicy { parameters.overrideContentSecurityPolicy }
    , m_cpuLimit(parameters.cpuLimit)
#if USE(WPE_RENDERER)
    , m_hostFileDescriptor(WTFMove(parameters.hostFileDescriptor))
#endif
    , m_webPageProxyIdentifier(parameters.webPageProxyIdentifier)
#if ENABLE(TEXT_AUTOSIZING)
    , m_textAutoSizingAdjustmentTimer(*this, &WebPage::textAutoSizingAdjustmentTimerFired)
#endif
    , m_overriddenMediaType(parameters.overriddenMediaType)
    , m_processDisplayName(parameters.processDisplayName)
#if ENABLE(APP_BOUND_DOMAINS)
    , m_limitsNavigationsToAppBoundDomains(parameters.limitsNavigationsToAppBoundDomains)
#endif
    , m_lastNavigationWasAppInitiated(parameters.lastNavigationWasAppInitiated)
#if ENABLE(APP_HIGHLIGHTS)
    , m_appHighlightsVisible(parameters.appHighlightsVisible)
#endif
{
    ASSERT(m_identifier);
    WEBPAGE_RELEASE_LOG(Loading, "constructor:");

    m_pageGroup = WebProcess::singleton().webPageGroup(parameters.pageGroupData);

    PageConfiguration pageConfiguration(
        WebProcess::singleton().sessionID(),
        makeUniqueRef<WebEditorClient>(this),
        WebSocketProvider::create(parameters.webPageProxyIdentifier),
        WebProcess::singleton().cacheStorageProvider(),
        m_userContentController,
        WebBackForwardListProxy::create(*this),
        WebProcess::singleton().cookieJar(),
        makeUniqueRef<WebProgressTrackerClient>(*this),
        makeUniqueRef<WebFrameLoaderClient>(m_mainFrame.copyRef()),
        WebProcess::singleton().broadcastChannelRegistry(),
        WebPermissionController::create(*this)
    );
    pageConfiguration.chromeClient = new WebChromeClient(*this);
#if ENABLE(CONTEXT_MENUS)
    pageConfiguration.contextMenuClient = new WebContextMenuClient(this);
#endif
#if ENABLE(DRAG_SUPPORT)
    pageConfiguration.dragClient = makeUnique<WebDragClient>(this);
#endif
    pageConfiguration.inspectorClient = new WebInspectorClient(this);
#if USE(AUTOCORRECTION_PANEL)
    pageConfiguration.alternativeTextClient = makeUnique<WebAlternativeTextClient>(this);
#endif

    pageConfiguration.diagnosticLoggingClient = makeUnique<WebDiagnosticLoggingClient>(*this);
    pageConfiguration.performanceLoggingClient = makeUnique<WebPerformanceLoggingClient>(*this);

#if PLATFORM(COCOA)
    pageConfiguration.validationMessageClient = makeUnique<WebValidationMessageClient>(*this);
#endif

    pageConfiguration.applicationCacheStorage = &WebProcess::singleton().applicationCacheStorage();
    pageConfiguration.databaseProvider = WebDatabaseProvider::getOrCreate(m_pageGroup->pageGroupID());
    pageConfiguration.storageNamespaceProvider = WebStorageNamespaceProvider::getOrCreate(*m_pageGroup);
    pageConfiguration.visitedLinkStore = VisitedLinkTableController::getOrCreate(parameters.visitedLinkTableID);

#if ENABLE(WEB_AUTHN)
    pageConfiguration.authenticatorCoordinatorClient = makeUnique<WebAuthenticatorCoordinator>(*this);
#endif

#if ENABLE(APPLICATION_MANIFEST)
    pageConfiguration.applicationManifest = parameters.applicationManifest;
#endif


    m_corsDisablingPatterns = WTFMove(parameters.corsDisablingPatterns);
    if (!m_corsDisablingPatterns.isEmpty())
        synchronizeCORSDisablingPatternsWithNetworkProcess();
    pageConfiguration.corsDisablingPatterns = parseAndAllowAccessToCORSDisablingPatterns(m_corsDisablingPatterns);

    pageConfiguration.userScriptsShouldWaitUntilNotification = parameters.userScriptsShouldWaitUntilNotification;
    pageConfiguration.loadsSubresources = parameters.loadsSubresources;
    pageConfiguration.allowedNetworkHosts = parameters.allowedNetworkHosts;
    pageConfiguration.shouldRelaxThirdPartyCookieBlocking = parameters.shouldRelaxThirdPartyCookieBlocking;
    pageConfiguration.httpsUpgradeEnabled = parameters.httpsUpgradeEnabled;

    if (!parameters.crossOriginAccessControlCheckEnabled)
        CrossOriginAccessControlCheckDisabler::singleton().setCrossOriginAccessControlCheckEnabled(false);

#if ENABLE(ATTACHMENT_ELEMENT)
    if (parameters.attachmentElementExtensionHandles)
        SandboxExtension::consumePermanently(*parameters.attachmentElementExtensionHandles);
#endif
#if PLATFORM(COCOA)
    // FIXME(207716): The following should be removed when the GPU process is complete.
    static bool hasConsumedMediaExtensionHandles = false;
    if (!hasConsumedMediaExtensionHandles && parameters.mediaExtensionHandles.size()) {
        SandboxExtension::consumePermanently(parameters.mediaExtensionHandles);
        SandboxExtension::consumePermanently(parameters.mediaIOKitExtensionHandles);
        hasConsumedMediaExtensionHandles = true;
    }
    static bool hasConsumedGPUExtensionHandles = false;
    if (!hasConsumedGPUExtensionHandles) {
        SandboxExtension::consumePermanently(parameters.gpuIOKitExtensionHandles);
        SandboxExtension::consumePermanently(parameters.gpuMachExtensionHandles);
        hasConsumedGPUExtensionHandles = true;
    }
#endif

#if HAVE(STATIC_FONT_REGISTRY)
    if (parameters.fontMachExtensionHandle)
        WebProcess::singleton().switchFromStaticFontRegistryToUserFontRegistry(WTFMove(*parameters.fontMachExtensionHandle));
#endif

    m_page = makeUnique<Page>(WTFMove(pageConfiguration));

#if PLATFORM(IOS)
    setAllowsDeprecatedSynchronousXMLHttpRequestDuringUnload(parameters.allowsDeprecatedSynchronousXMLHttpRequestDuringUnload);
#endif

    updatePreferences(parameters.store);

#if PLATFORM(IOS_FAMILY) || ENABLE(ROUTING_ARBITRATION)
    DeprecatedGlobalSettings::setShouldManageAudioSessionCategory(true);
#endif

    m_backgroundColor = parameters.backgroundColor;

    m_drawingArea = DrawingArea::create(*this, parameters);
    m_drawingArea->setShouldScaleViewToFitDocument(parameters.shouldScaleViewToFitDocument);

    if (parameters.isProcessSwap)
        freezeLayerTree(LayerTreeFreezeReason::ProcessSwap);

#if ENABLE(ASYNC_SCROLLING)
    m_useAsyncScrolling = parameters.store.getBoolValueForKey(WebPreferencesKey::threadedScrollingEnabledKey());
    if (!m_drawingArea->supportsAsyncScrolling())
        m_useAsyncScrolling = false;
    m_page->settings().setScrollingCoordinatorEnabled(m_useAsyncScrolling);
#endif

    // Disable Back/Forward cache expiration in the WebContent process since management happens in the UIProcess
    // in modern WebKit.
    m_page->settings().setBackForwardCacheExpirationInterval(Seconds::infinity());

    m_mainFrame->initWithCoreMainFrame(*this, m_page->mainFrame());

    m_drawingArea->updatePreferences(parameters.store);

    setBackgroundExtendsBeyondPage(parameters.backgroundExtendsBeyondPage);
    setPageAndTextZoomFactors(parameters.pageZoomFactor, parameters.textZoomFactor);

#if ENABLE(NOTIFICATIONS)
    WebCore::provideNotification(m_page.get(), new WebNotificationClient(this));
#endif

    m_page->setHasResourceLoadClient(parameters.hasResourceLoadClient);

    m_page->setCanStartMedia(false);
    m_mayStartMediaWhenInWindow = parameters.mayStartMediaWhenInWindow;
    if (parameters.mediaPlaybackIsSuspended)
        m_page->suspendAllMediaPlayback();

    m_page->setGroupName(m_pageGroup->identifier());
    m_page->setDeviceScaleFactor(parameters.deviceScaleFactor);
    m_page->setUserInterfaceLayoutDirection(m_userInterfaceLayoutDirection);
#if PLATFORM(IOS_FAMILY)
    m_page->setTextAutosizingWidth(parameters.textAutosizingWidth);
    setOverrideViewportArguments(parameters.overrideViewportArguments);
#endif

    platformInitialize();

    setUseFixedLayout(parameters.useFixedLayout);

    setUnderlayColor(parameters.underlayColor);

    setPaginationMode(parameters.paginationMode);
    setPaginationBehavesLikeColumns(parameters.paginationBehavesLikeColumns);
    setPageLength(parameters.pageLength);
    setGapBetweenPages(parameters.gapBetweenPages);
    setPaginationLineGridEnabled(parameters.paginationLineGridEnabled);

#if PLATFORM(GTK)
    GtkSettingsManagerProxy::singleton().applySettings(WTFMove(parameters.gtkSettings));
#endif

    effectiveAppearanceDidChange(parameters.useDarkAppearance, parameters.useElevatedUserInterfaceLevel);

    if (parameters.isEditable)
        setEditable(true);

#if PLATFORM(MAC)
    setUseSystemAppearance(parameters.useSystemAppearance);
#endif

    // If the page is created off-screen, its visibilityState should be prerender.
    m_page->setActivityState(m_activityState);
    if (!isVisible())
        m_page->setIsPrerender();

    updateIsInWindow(true);

    setMinimumSizeForAutoLayout(parameters.minimumSizeForAutoLayout);
    setSizeToContentAutoSizeMaximumSize(parameters.sizeToContentAutoSizeMaximumSize);
    setAutoSizingShouldExpandToViewHeight(parameters.autoSizingShouldExpandToViewHeight);
    setViewportSizeForCSSViewportUnits(parameters.viewportSizeForCSSViewportUnits);
    
    setScrollPinningBehavior(parameters.scrollPinningBehavior);
    if (parameters.scrollbarOverlayStyle)
        m_scrollbarOverlayStyle = static_cast<ScrollbarOverlayStyle>(parameters.scrollbarOverlayStyle.value());
    else
        m_scrollbarOverlayStyle = std::optional<ScrollbarOverlayStyle>();

    setTopContentInset(parameters.topContentInset);

    m_userAgent = parameters.userAgent;
    
    // Do not overwrite existing items. Due to process swapping and back/forward cache support, there may be other
    // (suspended) WebPages in this process for the same WebPageProxy in the UIProcess. Overwriting the HistoryItems
    // would break back/forward cache for those other pages since the HistoryItems hold the CachedPage.
    if (!parameters.itemStates.isEmpty())
        restoreSessionInternal(parameters.itemStates, parameters.itemStatesWereRestoredByAPIRequest ? WasRestoredByAPIRequest::Yes : WasRestoredByAPIRequest::No, WebBackForwardListProxy::OverwriteExistingItem::No);

    m_drawingArea->enablePainting();
    
    setMediaVolume(parameters.mediaVolume);

    setMuted(parameters.muted, [] { });

    // We use the DidFirstVisuallyNonEmptyLayout milestone to determine when to unfreeze the layer tree.
    m_page->addLayoutMilestones({ DidFirstLayout, DidFirstVisuallyNonEmptyLayout });

    auto& webProcess = WebProcess::singleton();
    webProcess.addMessageReceiver(Messages::WebPage::messageReceiverName(), m_identifier, *this);

    // FIXME: This should be done in the object constructors, and the objects themselves should be message receivers.
    webProcess.addMessageReceiver(Messages::WebInspector::messageReceiverName(), m_identifier, *this);
    webProcess.addMessageReceiver(Messages::WebInspectorUI::messageReceiverName(), m_identifier, *this);
    webProcess.addMessageReceiver(Messages::RemoteWebInspectorUI::messageReceiverName(), m_identifier, *this);

#ifndef NDEBUG
    webPageCounter.increment();
#endif

#if ENABLE(SCROLLING_THREAD)
    if (m_useAsyncScrolling)
        webProcess.eventDispatcher().addScrollingTreeForPage(this);
#endif

    for (auto& mimeType : parameters.mimeTypesWithCustomContentProviders)
        m_mimeTypesWithCustomContentProviders.add(mimeType);


    if (parameters.viewScaleFactor != 1)
        scaleView(parameters.viewScaleFactor);

    m_page->addLayoutMilestones(parameters.observedLayoutMilestones);

#if PLATFORM(COCOA)
    setSmartInsertDeleteEnabled(parameters.smartInsertDeleteEnabled);
    WebCore::setAdditionalSupportedImageTypes(parameters.additionalSupportedImageTypes);
#endif

#if HAVE(APP_ACCENT_COLORS)
    setAccentColor(parameters.accentColor);
#endif

    m_needsFontAttributes = parameters.needsFontAttributes;

    for (const auto& iterator : parameters.urlSchemeHandlers)
        registerURLSchemeHandler(iterator.value, iterator.key);

    m_userContentController->addContentWorlds(parameters.userContentControllerParameters.userContentWorlds);
    m_userContentController->addUserScripts(WTFMove(parameters.userContentControllerParameters.userScripts), InjectUserScriptImmediately::No);
    m_userContentController->addUserStyleSheets(parameters.userContentControllerParameters.userStyleSheets);
    m_userContentController->addUserScriptMessageHandlers(parameters.userContentControllerParameters.messageHandlers);
#if ENABLE(CONTENT_EXTENSIONS)
    m_userContentController->addContentRuleLists(WTFMove(parameters.userContentControllerParameters.contentRuleLists));
#endif

#if PLATFORM(IOS_FAMILY)
    setViewportConfigurationViewLayoutSize(parameters.viewportConfigurationViewLayoutSize, parameters.viewportConfigurationLayoutSizeScaleFactor, parameters.viewportConfigurationMinimumEffectiveDeviceWidth);
    setMaximumUnobscuredSize(parameters.maximumUnobscuredSize);
#endif

#if USE(AUDIO_SESSION)
    PlatformMediaSessionManager::setShouldDeactivateAudioSession(true);
#endif

#if HAVE(VISIBILITY_PROPAGATION_VIEW)
    m_contextForVisibilityPropagation = LayerHostingContext::createForExternalHostingProcess({
        m_canShowWhileLocked
    });
    WEBPAGE_RELEASE_LOG(Process, "WebPage: Created context with ID %u for visibility propagation from UIProcess", m_contextForVisibilityPropagation->contextID());
    send(Messages::WebPageProxy::DidCreateContextInWebProcessForVisibilityPropagation(m_contextForVisibilityPropagation->contextID()));
#endif // HAVE(VISIBILITY_PROPAGATION_VIEW)

#if ENABLE(IPC_TESTING_API)
    m_visitedLinkTableID = parameters.visitedLinkTableID;
#endif

#if ENABLE(VP9)
    PlatformMediaSessionManager::setShouldEnableVP8Decoder(parameters.shouldEnableVP8Decoder);
    PlatformMediaSessionManager::setShouldEnableVP9Decoder(parameters.shouldEnableVP9Decoder);
    PlatformMediaSessionManager::setShouldEnableVP9SWDecoder(parameters.shouldEnableVP9SWDecoder);
    if (m_shouldPlayMediaInGPUProcess && WebProcess::singleton().existingGPUProcessConnection())
        WebProcess::singleton().existingGPUProcessConnection()->enableVP9Decoders(parameters.shouldEnableVP8Decoder, parameters.shouldEnableVP9Decoder, parameters.shouldEnableVP9SWDecoder);
#endif

    m_page->setCanUseCredentialStorage(parameters.canUseCredentialStorage);

    updateThrottleState();
}

#if ENABLE(GPU_PROCESS)
void WebPage::gpuProcessConnectionDidBecomeAvailable(GPUProcessConnection& gpuProcessConnection)
{
#if HAVE(VISIBILITY_PROPAGATION_VIEW)
    gpuProcessConnection.createVisibilityPropagationContextForPage(*this);
#else
    UNUSED_PARAM(gpuProcessConnection);
#endif
}
#endif

void WebPage::requestMediaPlaybackState(CompletionHandler<void(WebKit::MediaPlaybackState)>&& completionHandler)
{
    if (!m_page->mediaPlaybackExists())
        return completionHandler(MediaPlaybackState::NoMediaPlayback);
    if (m_page->mediaPlaybackIsPaused())
        return completionHandler(MediaPlaybackState::MediaPlaybackPaused);
    if (m_page->mediaPlaybackIsSuspended())
        return completionHandler(MediaPlaybackState::MediaPlaybackSuspended);

    completionHandler(MediaPlaybackState::MediaPlaybackPlaying);
}

void WebPage::pauseAllMediaPlayback(CompletionHandler<void()>&& completionHandler)
{
    m_page->pauseAllMediaPlayback();
    completionHandler();
}

void WebPage::suspendAllMediaPlayback(CompletionHandler<void()>&& completionHandler)
{
    m_page->suspendAllMediaPlayback();
    completionHandler();
}

void WebPage::resumeAllMediaPlayback(CompletionHandler<void()>&& completionHandler)
{
    m_page->resumeAllMediaPlayback();
    completionHandler();
}

void WebPage::suspendAllMediaBuffering()
{
    m_page->suspendAllMediaBuffering();
}

void WebPage::resumeAllMediaBuffering()
{
    m_page->resumeAllMediaBuffering();
}


void WebPage::reinitializeWebPage(WebPageCreationParameters&& parameters)
{
    ASSERT(m_drawingArea);

    setSize(parameters.viewSize);

    // If the UIProcess created a new DrawingArea, then we need to do the same.
    if (m_drawingArea->identifier() != parameters.drawingAreaIdentifier) {
        auto oldDrawingArea = std::exchange(m_drawingArea, nullptr);
        oldDrawingArea->removeMessageReceiverIfNeeded();

        m_drawingArea = DrawingArea::create(*this, parameters);
        m_drawingArea->setShouldScaleViewToFitDocument(parameters.shouldScaleViewToFitDocument);
        m_drawingArea->updatePreferences(parameters.store);
        m_drawingArea->enablePainting();

        m_drawingArea->adoptLayersFromDrawingArea(*oldDrawingArea);
        m_drawingArea->adoptDisplayRefreshMonitorsFromDrawingArea(*oldDrawingArea);

        unfreezeLayerTree(LayerTreeFreezeReason::PageSuspended);
    }

    setMinimumSizeForAutoLayout(parameters.minimumSizeForAutoLayout);
    setSizeToContentAutoSizeMaximumSize(parameters.sizeToContentAutoSizeMaximumSize);

    if (m_activityState != parameters.activityState)
        setActivityState(parameters.activityState, ActivityStateChangeAsynchronous, [] { });
    if (m_layerHostingMode != parameters.layerHostingMode)
        setLayerHostingMode(parameters.layerHostingMode);

#if PLATFORM(GTK)
    GtkSettingsManagerProxy::singleton().applySettings(WTFMove(parameters.gtkSettings));
#endif

    platformReinitialize();
}

void WebPage::updateThrottleState()
{
    bool isThrottleable = this->isThrottleable();

    // The UserActivity prevents App Nap. So if we want to allow App Nap of the page, stop the activity.
    // If the page should not be app nap'd, start it.
    if (isThrottleable)
        m_userActivity.stop();
    else
        m_userActivity.start();

#if ENABLE(SERVICE_WORKER)
    if (RuntimeEnabledFeatures::sharedFeatures().serviceWorkerEnabled()) {
        RunLoop::main().dispatch([isThrottleable] {
            WebServiceWorkerProvider::singleton().updateThrottleState(isThrottleable);
        });
    }
#endif
}

bool WebPage::isThrottleable() const
{
    bool isActive = m_activityState.containsAny({ ActivityState::IsLoading, ActivityState::IsAudible, ActivityState::WindowIsActive });
    bool isVisuallyIdle = m_activityState.contains(ActivityState::IsVisuallyIdle);

    return m_isAppNapEnabled && !isActive && isVisuallyIdle;
}

WebPage::~WebPage()
{
    ASSERT(!m_page);
    WEBPAGE_RELEASE_LOG(Loading, "destructor:");

    if (!m_corsDisablingPatterns.isEmpty()) {
        m_corsDisablingPatterns.clear();
        synchronizeCORSDisablingPatternsWithNetworkProcess();
    }

    platformDetach();

    m_sandboxExtensionTracker.invalidate();


#if !PLATFORM(IOS_FAMILY)
    if (m_headerBanner)
        m_headerBanner->detachFromPage();
    if (m_footerBanner)
        m_footerBanner->detachFromPage();
#endif // !PLATFORM(IOS_FAMILY)

#ifndef NDEBUG
    webPageCounter.decrement();
#endif

#if ENABLE(GPU_PROCESS) && HAVE(VISIBILITY_PROPAGATION_VIEW)
    if (auto* gpuProcessConnection = WebProcess::singleton().existingGPUProcessConnection())
        gpuProcessConnection->destroyVisibilityPropagationContextForPage(*this);
#endif // ENABLE(GPU_PROCESS)
    

    for (auto& completionHandler : std::exchange(m_markLayersAsVolatileCompletionHandlers, { }))
        completionHandler(false);
    
#if ENABLE(APPLICATION_MANIFEST)
    for (auto& completionHandler : m_applicationManifestFetchCallbackMap.values()) {
        if (completionHandler)
            completionHandler(std::nullopt);
    }
#endif
}

IPC::Connection* WebPage::messageSenderConnection() const
{
    return WebProcess::singleton().parentProcessConnection();
}

uint64_t WebPage::messageSenderDestinationID() const
{
    return identifier().toUInt64();
}

#if ENABLE(CONTEXT_MENUS)
void WebPage::setInjectedBundleContextMenuClient(std::unique_ptr<API::InjectedBundle::PageContextMenuClient>&& contextMenuClient)
{
    if (!contextMenuClient) {
        m_contextMenuClient = makeUnique<API::InjectedBundle::PageContextMenuClient>();
        return;
    }

    m_contextMenuClient = WTFMove(contextMenuClient);
}
#endif

void WebPage::setInjectedBundleEditorClient(std::unique_ptr<API::InjectedBundle::EditorClient>&& editorClient)
{
    if (!editorClient) {
        m_editorClient = makeUnique<API::InjectedBundle::EditorClient>();
        return;
    }

    m_editorClient = WTFMove(editorClient);
}

void WebPage::setInjectedBundleFormClient(std::unique_ptr<API::InjectedBundle::FormClient>&& formClient)
{
    if (!formClient) {
        m_formClient = makeUnique<API::InjectedBundle::FormClient>();
        return;
    }

    m_formClient = WTFMove(formClient);
}

void WebPage::setInjectedBundlePageLoaderClient(std::unique_ptr<API::InjectedBundle::PageLoaderClient>&& loaderClient)
{
    if (!loaderClient) {
        m_loaderClient = makeUnique<API::InjectedBundle::PageLoaderClient>();
        return;
    }

    m_loaderClient = WTFMove(loaderClient);

    // It would be nice to get rid of this code and transition all clients to using didLayout instead of
    // didFirstLayoutInFrame and didFirstVisuallyNonEmptyLayoutInFrame. In the meantime, this is required
    // for backwards compatibility.
    if (auto milestones = m_loaderClient->layoutMilestones())
        listenForLayoutMilestones(milestones);
}

void WebPage::initializeInjectedBundlePolicyClient(WKBundlePagePolicyClientBase* client)
{
    m_policyClient.initialize(client);
}

void WebPage::setInjectedBundleResourceLoadClient(std::unique_ptr<API::InjectedBundle::ResourceLoadClient>&& client)
{
    if (!m_resourceLoadClient)
        m_resourceLoadClient = makeUnique<API::InjectedBundle::ResourceLoadClient>();
    else
        m_resourceLoadClient = WTFMove(client);
}

void WebPage::setInjectedBundleUIClient(std::unique_ptr<API::InjectedBundle::PageUIClient>&& uiClient)
{
    if (!uiClient) {
        m_uiClient = makeUnique<API::InjectedBundle::PageUIClient>();
        return;
    }

    m_uiClient = WTFMove(uiClient);
}

bool WebPage::hasPendingEditorStateUpdate() const
{
    return m_pendingEditorStateUpdateStatus != PendingEditorStateUpdateStatus::NotScheduled;
}

EditorState WebPage::editorState(ShouldPerformLayout shouldPerformLayout) const
{
    // Ref the frame because this function may perform layout, which may cause frame destruction.
    Ref<Frame> frame = m_page->focusController().focusedOrMainFrame();

    EditorState result;

    const VisibleSelection& selection = frame->selection().selection();
    auto& editor = frame->editor();

    result.identifier = m_lastEditorStateIdentifier.increment();
    result.selectionIsNone = selection.isNone();
    result.selectionIsRange = selection.isRange();
    result.isContentEditable = selection.isContentEditable();
    result.isContentRichlyEditable = selection.isContentRichlyEditable();
    result.isInPasswordField = selection.isInPasswordField();
    result.hasComposition = editor.hasComposition();
    result.shouldIgnoreSelectionChanges = editor.ignoreSelectionChanges() || (editor.client() && !editor.client()->shouldRevealCurrentSelectionAfterInsertion());
    result.triggeredByAccessibilitySelectionChange = m_pendingEditorStateUpdateStatus == PendingEditorStateUpdateStatus::ScheduledDuringAccessibilitySelectionChange || m_isChangingSelectionForAccessibility;

    Ref<Document> document = *frame->document();
    result.originIdentifierForPasteboard = document->originIdentifierForPasteboard();

    if (result.selectionIsRange) {
        auto selectionRange = selection.range();
        result.selectionIsRangeInsideImageOverlay = selectionRange && HTMLElement::isInsideImageOverlay(*selectionRange);
    }

    if (shouldPerformLayout == ShouldPerformLayout::Yes || platformNeedsLayoutForEditorState(frame))
        document->updateLayout(); // May cause document destruction

    if (auto* frameView = document->view(); frameView && !frameView->needsLayout()) {
        result.isMissingPostLayoutData = false;

        auto& postLayoutData = result.postLayoutData();
        postLayoutData.canCut = editor.canCut();
        postLayoutData.canCopy = editor.canCopy();
        postLayoutData.canPaste = editor.canPaste();

        if (m_needsFontAttributes)
            postLayoutData.fontAttributes = editor.fontAttributesAtSelectionStart();
    }

    getPlatformEditorState(frame, result);

    m_lastEditorStateWasContentEditable = result.isContentEditable ? EditorStateIsContentEditable::Yes : EditorStateIsContentEditable::No;

    return result;
}

void WebPage::changeFontAttributes(WebCore::FontAttributeChanges&& changes)
{
    auto& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.selection().selection().isContentEditable())
        frame.editor().applyStyleToSelection(changes.createEditingStyle(), changes.editAction(), Editor::ColorFilterMode::InvertColor);
}

void WebPage::changeFont(WebCore::FontChanges&& changes)
{
    auto& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.selection().selection().isContentEditable())
        frame.editor().applyStyleToSelection(changes.createEditingStyle(), EditAction::SetFont, Editor::ColorFilterMode::InvertColor);
}

void WebPage::executeEditCommandWithCallback(const String& commandName, const String& argument, CompletionHandler<void()>&& completionHandler)
{
    executeEditCommand(commandName, argument);
    completionHandler();
}

void WebPage::selectAll()
{
    executeEditingCommand("SelectAll"_s, { });
    platformDidSelectAll();
}

#if !PLATFORM(IOS_FAMILY)

void WebPage::platformDidSelectAll()
{
}

#endif // !PLATFORM(IOS_FAMILY)

void WebPage::updateEditorStateAfterLayoutIfEditabilityChanged()
{
    // FIXME: We should update EditorStateIsContentEditable to track whether the state is richly
    // editable or plainttext-only.
    if (m_lastEditorStateWasContentEditable == EditorStateIsContentEditable::Unset)
        return;

    Frame& frame = m_page->focusController().focusedOrMainFrame();
    EditorStateIsContentEditable editorStateIsContentEditable = frame.selection().selection().isContentEditable() ? EditorStateIsContentEditable::Yes : EditorStateIsContentEditable::No;
    if (m_lastEditorStateWasContentEditable != editorStateIsContentEditable)
        scheduleFullEditorStateUpdate();
}

static OptionSet<RenderAsTextFlag> toRenderAsTextFlags(unsigned options)
{
    OptionSet<RenderAsTextFlag> flags;

    if (options & RenderTreeShowAllLayers)
        flags.add(RenderAsTextFlag::ShowAllLayers);
    if (options & RenderTreeShowLayerNesting)
        flags.add(RenderAsTextFlag::ShowLayerNesting);
    if (options & RenderTreeShowCompositedLayers)
        flags.add(RenderAsTextFlag::ShowCompositedLayers);
    if (options & RenderTreeShowOverflow)
        flags.add(RenderAsTextFlag::ShowOverflow);
    if (options & RenderTreeShowSVGGeometry)
        flags.add(RenderAsTextFlag::ShowSVGGeometry);
    if (options & RenderTreeShowLayerFragments)
        flags.add(RenderAsTextFlag::ShowLayerFragments);

    return flags;
}

String WebPage::renderTreeExternalRepresentation(unsigned options) const
{
    return externalRepresentation(m_mainFrame->coreFrame(), toRenderAsTextFlags(options));
}

String WebPage::renderTreeExternalRepresentationForPrinting() const
{
    return externalRepresentation(m_mainFrame->coreFrame(), { RenderAsTextFlag::PrintingMode });
}

uint64_t WebPage::renderTreeSize() const
{
    if (!m_page)
        return 0;
    return m_page->renderTreeSize();
}

void WebPage::setHasResourceLoadClient(bool has)
{
    if (m_page)
        m_page->setHasResourceLoadClient(has);
}

void WebPage::setCanUseCredentialStorage(bool has)
{
    if (m_page)
        m_page->setCanUseCredentialStorage(has);
}

void WebPage::setTracksRepaints(bool trackRepaints)
{
    if (FrameView* view = mainFrameView())
        view->setTracksRepaints(trackRepaints);
}

bool WebPage::isTrackingRepaints() const
{
    if (FrameView* view = mainFrameView())
        return view->isTrackingRepaints();

    return false;
}

void WebPage::resetTrackedRepaints()
{
    if (FrameView* view = mainFrameView())
        view->resetTrackedRepaints();
}

Ref<API::Array> WebPage::trackedRepaintRects()
{
    FrameView* view = mainFrameView();
    if (!view)
        return API::Array::create();

    Vector<RefPtr<API::Object>> repaintRects;
    repaintRects.reserveInitialCapacity(view->trackedRepaintRects().size());

    for (const auto& repaintRect : view->trackedRepaintRects())
        repaintRects.uncheckedAppend(API::Rect::create(toAPI(repaintRect)));

    return API::Array::create(WTFMove(repaintRects));
}

void WebPage::executeEditingCommand(const String& commandName, const String& argument)
{
    platformWillPerformEditingCommand();

    auto& frame = m_page->focusController().focusedOrMainFrame();
    frame.editor().command(commandName).execute(argument);
}

void WebPage::setEditable(bool editable)
{
    m_page->setEditable(editable);
    m_page->setTabKeyCyclesThroughElements(!editable);
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (editable) {
        frame.editor().applyEditingStyleToBodyElement();
        // If the page is made editable and the selection is empty, set it to something.
        if (frame.selection().isNone())
            frame.selection().setSelectionFromNone();
    }
}

void WebPage::increaseListLevel()
{
    m_page->focusController().focusedOrMainFrame().editor().increaseSelectionListLevel();
}

void WebPage::decreaseListLevel()
{
    m_page->focusController().focusedOrMainFrame().editor().decreaseSelectionListLevel();
}

void WebPage::changeListType()
{
    m_page->focusController().focusedOrMainFrame().editor().changeSelectionListType();
}

void WebPage::setBaseWritingDirection(WritingDirection direction)
{
    m_page->focusController().focusedOrMainFrame().editor().setBaseWritingDirection(direction);
}

bool WebPage::isEditingCommandEnabled(const String& commandName)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();

    Editor::Command command = frame.editor().command(commandName);
    return command.isSupported() && command.isEnabled();
}
    
void WebPage::clearMainFrameName()
{
    if (Frame* frame = mainFrame())
        frame->tree().clearName();
}

void WebPage::enterAcceleratedCompositingMode(GraphicsLayer* layer)
{
    m_drawingArea->setRootCompositingLayer(layer);
}

void WebPage::exitAcceleratedCompositingMode()
{
    m_drawingArea->setRootCompositingLayer(nullptr);
}

void WebPage::close()
{
    if (m_isClosed)
        return;

    WEBPAGE_RELEASE_LOG(Loading, "close:");

    WebProcess::singleton().ensureNetworkProcessConnection().connection().send(Messages::NetworkConnectionToWebProcess::ClearPageSpecificData(m_identifier), 0);

    m_isClosed = true;

    // If there is still no URL, then we never loaded anything in this page, so nothing to report.
    if (!mainWebFrame().url().isEmpty())
        reportUsedFeatures();

    if (WebProcess::singleton().injectedBundle())
        WebProcess::singleton().injectedBundle()->willDestroyPage(this);

    if (m_inspector) {
        m_inspector->disconnectFromPage();
        m_inspector = nullptr;
    }

    m_page->inspectorController().disconnectAllFrontends();

    if (m_activePopupMenu) {
        m_activePopupMenu->disconnectFromPage();
        m_activePopupMenu = nullptr;
    }

    if (m_activeOpenPanelResultListener) {
        m_activeOpenPanelResultListener->disconnectFromPage();
        m_activeOpenPanelResultListener = nullptr;
    }

#if ENABLE(INPUT_TYPE_COLOR)
    if (m_activeColorChooser) {
        m_activeColorChooser->disconnectFromPage();
        m_activeColorChooser = nullptr;
    }
#endif

#if PLATFORM(GTK)
    if (m_printOperation) {
        m_printOperation->disconnectFromPage();
        m_printOperation = nullptr;
    }
#endif

#if ENABLE(VIDEO) && USE(GSTREAMER)
    if (m_installMediaPluginsCallback) {
        m_installMediaPluginsCallback->invalidate();
        m_installMediaPluginsCallback = nullptr;
    }
#endif

    m_sandboxExtensionTracker.invalidate();

#if ENABLE(TEXT_AUTOSIZING)
    m_textAutoSizingAdjustmentTimer.stop();
#endif

#if ENABLE(CONTEXT_MENUS)
    m_contextMenuClient = makeUnique<API::InjectedBundle::PageContextMenuClient>();
#endif
    m_editorClient = makeUnique<API::InjectedBundle::EditorClient>();
    m_formClient = makeUnique<API::InjectedBundle::FormClient>();
    m_loaderClient = makeUnique<API::InjectedBundle::PageLoaderClient>();
    m_policyClient.initialize(0);
    m_resourceLoadClient = makeUnique<API::InjectedBundle::ResourceLoadClient>();
    m_uiClient = makeUnique<API::InjectedBundle::PageUIClient>();

    m_printContext = nullptr;
    m_mainFrame->coreFrame()->loader().detachFromParent();
    m_drawingArea = nullptr;

    DeferredPageDestructor::createDeferredPageDestructor(WTFMove(m_page), this);

    bool isRunningModal = m_isRunningModal;
    m_isRunningModal = false;

    auto& webProcess = WebProcess::singleton();
#if PLATFORM(COCOA)
    if (m_remoteObjectRegistry)
        m_remoteObjectRegistry->close();
    ASSERT(!m_remoteObjectRegistry);
#endif
#if ENABLE(SCROLLING_THREAD)
    if (m_useAsyncScrolling)
        webProcess.eventDispatcher().removeScrollingTreeForPage(this);
#endif
    webProcess.removeMessageReceiver(Messages::WebPage::messageReceiverName(), m_identifier);
    // FIXME: This should be done in the object destructors, and the objects themselves should be message receivers.
    webProcess.removeMessageReceiver(Messages::WebInspector::messageReceiverName(), m_identifier);
    webProcess.removeMessageReceiver(Messages::WebInspectorUI::messageReceiverName(), m_identifier);
    webProcess.removeMessageReceiver(Messages::RemoteWebInspectorUI::messageReceiverName(), m_identifier);

#if PLATFORM(COCOA) || PLATFORM(GTK)
    m_viewGestureGeometryCollector = nullptr;
#endif

    // The WebPage can be destroyed by this call.
    WebProcess::singleton().removeWebPage(m_identifier);

    WebProcess::singleton().updateActivePages(m_processDisplayName);

    if (isRunningModal)
        RunLoop::main().stop();
}

void WebPage::tryClose(CompletionHandler<void(bool)>&& completionHandler)
{
    bool shouldClose = corePage()->userInputBridge().tryClosePage();
    completionHandler(shouldClose);
}

void WebPage::sendClose()
{
    send(Messages::WebPageProxy::ClosePage());
}

void WebPage::suspendForProcessSwap()
{
    auto failedToSuspend = [this, protectedThis = makeRef(*this)] {
        send(Messages::WebPageProxy::DidFailToSuspendAfterProcessSwap());
    };

    auto* currentHistoryItem = m_mainFrame->coreFrame()->loader().history().currentItem();
    if (!currentHistoryItem) {
        failedToSuspend();
        return;
    }

    if (!BackForwardCache::singleton().addIfCacheable(*currentHistoryItem, corePage())) {
        failedToSuspend();
        return;
    }

    // Back/forward cache does not break the opener link for the main frame (only does so for the subframes) because the
    // main frame is normally re-used for the navigation. However, in the case of process-swapping, the main frame
    // is now hosted in another process and the one in this process is in the cache.
    if (m_mainFrame->coreFrame())
        m_mainFrame->coreFrame()->loader().detachFromAllOpenedFrames();

    send(Messages::WebPageProxy::DidSuspendAfterProcessSwap());
}

void WebPage::loadURLInFrame(URL&& url, const String& referrer, FrameIdentifier frameID)
{
    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;

    frame->coreFrame()->loader().load(FrameLoadRequest(*frame->coreFrame(), ResourceRequest(url, referrer)));
}

void WebPage::loadDataInFrame(IPC::DataReference&& data, String&& MIMEType, String&& encodingName, URL&& baseURL, FrameIdentifier frameID)
{
    auto* frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;
    ASSERT(&mainWebFrame() != frame);

    auto sharedBuffer = SharedBuffer::create(data.data(), data.size());
    ResourceResponse response(baseURL, MIMEType, sharedBuffer->size(), encodingName);
    SubstituteData substituteData(WTFMove(sharedBuffer), baseURL, WTFMove(response), SubstituteData::SessionHistoryVisibility::Hidden);
    frame->coreFrame()->loader().load(FrameLoadRequest(*frame->coreFrame(), ResourceRequest(baseURL), WTFMove(substituteData)));
}

#if !PLATFORM(COCOA)
void WebPage::platformDidReceiveLoadParameters(const LoadParameters& loadParameters)
{
}
#endif

void WebPage::loadRequest(LoadParameters&& loadParameters)
{
    setLastNavigationWasAppInitiated(loadParameters.request.isAppInitiated());

#if ENABLE(APP_BOUND_DOMAINS)
    setIsNavigatingToAppBoundDomain(loadParameters.isNavigatingToAppBoundDomain, &m_mainFrame.get());
#endif

    WebProcess::singleton().webLoaderStrategy().setExistingNetworkResourceLoadIdentifierToResume(loadParameters.existingNetworkResourceLoadIdentifierToResume);
    auto resumingLoadScope = makeScopeExit([] {
        WebProcess::singleton().webLoaderStrategy().setExistingNetworkResourceLoadIdentifierToResume(std::nullopt);
    });

    SendStopResponsivenessTimer stopper;

    m_pendingNavigationID = loadParameters.navigationID;
    m_pendingWebsitePolicies = WTFMove(loadParameters.websitePolicies);

    m_sandboxExtensionTracker.beginLoad(m_mainFrame.ptr(), WTFMove(loadParameters.sandboxExtensionHandle));

    // Let the InjectedBundle know we are about to start the load, passing the user data from the UIProcess
    // to all the client to set up any needed state.
    m_loaderClient->willLoadURLRequest(*this, loadParameters.request, WebProcess::singleton().transformHandlesToObjects(loadParameters.userData.object()).get());

    platformDidReceiveLoadParameters(loadParameters);

    // Initate the load in WebCore.
    FrameLoadRequest frameLoadRequest { *m_mainFrame->coreFrame(), loadParameters.request };
    frameLoadRequest.setShouldOpenExternalURLsPolicy(loadParameters.shouldOpenExternalURLsPolicy);
    frameLoadRequest.setShouldTreatAsContinuingLoad(loadParameters.shouldTreatAsContinuingLoad);
    frameLoadRequest.setLockHistory(loadParameters.lockHistory);
    frameLoadRequest.setLockBackForwardList(loadParameters.lockBackForwardList);
    frameLoadRequest.setClientRedirectSourceForHistory(loadParameters.clientRedirectSourceForHistory);
    frameLoadRequest.setIsRequestFromClientOrUserInput();

    corePage()->userInputBridge().loadRequest(WTFMove(frameLoadRequest));

    ASSERT(!m_pendingNavigationID);
    ASSERT(!m_pendingWebsitePolicies);
}

// LoadRequestWaitingForProcessLaunch should never be sent to the WebProcess. It must always be converted to a LoadRequest message.
NO_RETURN void WebPage::loadRequestWaitingForProcessLaunch(LoadParameters&&, URL&&, WebPageProxyIdentifier, bool)
{
    RELEASE_ASSERT_NOT_REACHED();
}

void WebPage::loadDataImpl(uint64_t navigationID, ShouldTreatAsContinuingLoad shouldTreatAsContinuingLoad, std::optional<WebsitePoliciesData>&& websitePolicies, Ref<SharedBuffer>&& sharedBuffer, ResourceRequest&& request, ResourceResponse&& response, const URL& unreachableURL, const UserData& userData, std::optional<NavigatingToAppBoundDomain> isNavigatingToAppBoundDomain, SubstituteData::SessionHistoryVisibility sessionHistoryVisibility, ShouldOpenExternalURLsPolicy shouldOpenExternalURLsPolicy)
{
#if ENABLE(APP_BOUND_DOMAINS)
    setIsNavigatingToAppBoundDomain(isNavigatingToAppBoundDomain, &m_mainFrame.get());
#endif

    SendStopResponsivenessTimer stopper;

    m_pendingNavigationID = navigationID;
    m_pendingWebsitePolicies = WTFMove(websitePolicies);

    SubstituteData substituteData(WTFMove(sharedBuffer), unreachableURL, response, sessionHistoryVisibility);

    // Let the InjectedBundle know we are about to start the load, passing the user data from the UIProcess
    // to all the client to set up any needed state.
    m_loaderClient->willLoadDataRequest(*this, request, const_cast<SharedBuffer*>(substituteData.content()), substituteData.mimeType(), substituteData.textEncoding(), substituteData.failingURL(), WebProcess::singleton().transformHandlesToObjects(userData.object()).get());

    // Initate the load in WebCore.
    FrameLoadRequest frameLoadRequest(*m_mainFrame->coreFrame(), request, substituteData);
    frameLoadRequest.setShouldOpenExternalURLsPolicy(shouldOpenExternalURLsPolicy);
    frameLoadRequest.setShouldTreatAsContinuingLoad(shouldTreatAsContinuingLoad);
    frameLoadRequest.setIsRequestFromClientOrUserInput();
    m_mainFrame->coreFrame()->loader().load(WTFMove(frameLoadRequest));
}

void WebPage::loadData(LoadParameters&& loadParameters)
{
    platformDidReceiveLoadParameters(loadParameters);

    auto sharedBuffer = SharedBuffer::create(loadParameters.data.data(), loadParameters.data.size());

    URL baseURL;
    if (loadParameters.baseURLString.isEmpty())
        baseURL = aboutBlankURL();
    else {
        baseURL = URL(URL(), loadParameters.baseURLString);
        if (baseURL.isValid() && !baseURL.protocolIsInHTTPFamily())
            LegacySchemeRegistry::registerURLSchemeAsHandledBySchemeHandler(baseURL.protocol().toString());
    }

    ResourceResponse response(URL(), loadParameters.MIMEType, sharedBuffer->size(), loadParameters.encodingName);
    loadDataImpl(loadParameters.navigationID, loadParameters.shouldTreatAsContinuingLoad, WTFMove(loadParameters.websitePolicies), WTFMove(sharedBuffer), ResourceRequest(baseURL), WTFMove(response), URL(), loadParameters.userData, loadParameters.isNavigatingToAppBoundDomain, loadParameters.sessionHistoryVisibility, loadParameters.shouldOpenExternalURLsPolicy);
}

void WebPage::loadAlternateHTML(LoadParameters&& loadParameters)
{
    platformDidReceiveLoadParameters(loadParameters);

    URL baseURL = loadParameters.baseURLString.isEmpty() ? aboutBlankURL() : URL(URL(), loadParameters.baseURLString);
    URL unreachableURL = loadParameters.unreachableURLString.isEmpty() ? URL() : URL(URL(), loadParameters.unreachableURLString);
    URL provisionalLoadErrorURL = loadParameters.provisionalLoadErrorURLString.isEmpty() ? URL() : URL(URL(), loadParameters.provisionalLoadErrorURLString);
    auto sharedBuffer = SharedBuffer::create(loadParameters.data.data(), loadParameters.data.size());
    m_mainFrame->coreFrame()->loader().setProvisionalLoadErrorBeingHandledURL(provisionalLoadErrorURL);

    ResourceResponse response(URL(), loadParameters.MIMEType, sharedBuffer->size(), loadParameters.encodingName);
    loadDataImpl(loadParameters.navigationID, loadParameters.shouldTreatAsContinuingLoad, WTFMove(loadParameters.websitePolicies), WTFMove(sharedBuffer), ResourceRequest(baseURL), WTFMove(response), unreachableURL, loadParameters.userData, loadParameters.isNavigatingToAppBoundDomain, WebCore::SubstituteData::SessionHistoryVisibility::Hidden);
    m_mainFrame->coreFrame()->loader().setProvisionalLoadErrorBeingHandledURL({ });
}

void WebPage::loadSimulatedRequestAndResponse(LoadParameters&& loadParameters, ResourceResponse&& simulatedResponse)
{
    setLastNavigationWasAppInitiated(loadParameters.request.isAppInitiated());
    auto sharedBuffer = SharedBuffer::create(loadParameters.data.data(), loadParameters.data.size());
    loadDataImpl(loadParameters.navigationID, loadParameters.shouldTreatAsContinuingLoad, WTFMove(loadParameters.websitePolicies), WTFMove(sharedBuffer), WTFMove(loadParameters.request), WTFMove(simulatedResponse), URL(), loadParameters.userData, loadParameters.isNavigatingToAppBoundDomain, SubstituteData::SessionHistoryVisibility::Visible);
}

void WebPage::navigateToPDFLinkWithSimulatedClick(const String& url, IntPoint documentPoint, IntPoint screenPoint)
{
    Frame* mainFrame = m_mainFrame->coreFrame();
    Document* mainFrameDocument = mainFrame->document();
    if (!mainFrameDocument)
        return;

    const int singleClick = 1;
    // FIXME: Set modifier keys.
    // FIXME: This should probably set IsSimulated::Yes.
    auto mouseEvent = MouseEvent::create(eventNames().clickEvent, Event::CanBubble::Yes, Event::IsCancelable::Yes, Event::IsComposed::Yes,
        MonotonicTime::now(), nullptr, singleClick, screenPoint, documentPoint, { }, { }, 0, 0, nullptr, 0, WebCore::NoTap);

    mainFrame->loader().changeLocation(mainFrameDocument->completeURL(url), emptyString(), mouseEvent.ptr(), ReferrerPolicy::NoReferrer, ShouldOpenExternalURLsPolicy::ShouldAllow);
}

void WebPage::stopLoadingFrame(FrameIdentifier frameID)
{
    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;

    auto* coreFrame = frame->coreFrame();
    if (!coreFrame || !m_page)
        return;

    m_page->userInputBridge().stopLoadingFrame(*coreFrame);
}

void WebPage::stopLoading()
{
    if (!m_page || !m_mainFrame->coreFrame())
        return;

    SendStopResponsivenessTimer stopper;
    m_page->userInputBridge().stopLoadingFrame(*m_mainFrame->coreFrame());
}

bool WebPage::defersLoading() const
{
    return m_page->defersLoading();
}

void WebPage::setDefersLoading(bool defersLoading)
{
    m_page->setDefersLoading(defersLoading);
}

void WebPage::reload(uint64_t navigationID, uint32_t reloadOptions, SandboxExtension::Handle&& sandboxExtensionHandle)
{
    SendStopResponsivenessTimer stopper;

    ASSERT(!m_mainFrame->coreFrame()->loader().frameHasLoaded() || !m_pendingNavigationID);
    m_pendingNavigationID = navigationID;

    m_sandboxExtensionTracker.beginReload(m_mainFrame.ptr(), WTFMove(sandboxExtensionHandle));
    if (m_page && m_mainFrame->coreFrame())
        m_page->userInputBridge().reloadFrame(*m_mainFrame->coreFrame(), OptionSet<ReloadOption>::fromRaw(reloadOptions));
    else
        ASSERT_NOT_REACHED();

    if (m_pendingNavigationID) {
        // This can happen if FrameLoader::reload() returns early because the document URL is empty.
        // The reload does nothing so we need to reset the pending navigation. See webkit.org/b/153210.
        m_pendingNavigationID = 0;
    }
}

void WebPage::goToBackForwardItem(uint64_t navigationID, const BackForwardItemIdentifier& backForwardItemID, FrameLoadType backForwardType, ShouldTreatAsContinuingLoad shouldTreatAsContinuingLoad, std::optional<WebsitePoliciesData>&& websitePolicies, bool lastNavigationWasAppInitiated)
{
    SendStopResponsivenessTimer stopper;

    m_lastNavigationWasAppInitiated = lastNavigationWasAppInitiated;

    ASSERT(isBackForwardLoadType(backForwardType));

    HistoryItem* item = WebBackForwardListProxy::itemForID(backForwardItemID);
    ASSERT(item);
    if (!item)
        return;

    LOG(Loading, "In WebProcess pid %i, WebPage %" PRIu64 " is navigating to back/forward URL %s", getCurrentProcessID(), m_identifier.toUInt64(), item->url().string().utf8().data());

    ASSERT(!m_pendingNavigationID);
    m_pendingNavigationID = navigationID;
    m_pendingWebsitePolicies = WTFMove(websitePolicies);

    m_page->goToItem(*item, backForwardType, shouldTreatAsContinuingLoad);
}

void WebPage::tryRestoreScrollPosition()
{
    m_page->mainFrame().loader().history().restoreScrollPositionAndViewState();
}

WebPage& WebPage::fromCorePage(Page& page)
{
    return static_cast<WebChromeClient&>(page.chrome().client()).page();
}

static std::optional<FrameTreeNodeData> frameTreeNodeData(Frame& frame)
{
    std::optional<FrameTreeNodeData> info;
    if (auto* webFrame = WebFrame::fromCoreFrame(frame)) {
        Vector<FrameTreeNodeData> children;
        for (auto* childFrame = frame.tree().firstChild(); childFrame; childFrame = childFrame->tree().nextSibling()) {
            if (auto childInfo = frameTreeNodeData(*childFrame))
                children.append(WTFMove(*childInfo));
        }
        info = FrameTreeNodeData {
            webFrame->info(),
            WTFMove(children)
        };
    }
    return info;
}

void WebPage::getAllFrames(CompletionHandler<void(FrameTreeNodeData&&)>&& completionHandler)
{
    completionHandler(*frameTreeNodeData(m_page->mainFrame()));
}

void WebPage::setSize(const WebCore::IntSize& viewSize)
{
    if (m_viewSize == viewSize)
        return;

    m_viewSize = viewSize;
    FrameView* view = m_page->mainFrame().view();
    view->resize(viewSize);
    m_drawingArea->setNeedsDisplay();

#if USE(COORDINATED_GRAPHICS)
    if (view->useFixedLayout())
        sendViewportAttributesChanged(m_page->viewportArguments());
#endif
}

#if USE(COORDINATED_GRAPHICS)
void WebPage::sendViewportAttributesChanged(const ViewportArguments& viewportArguments)
{
    FrameView* view = m_page->mainFrame().view();
    ASSERT(view && view->useFixedLayout());

    // Viewport properties have no impact on zero sized fixed viewports.
    if (m_viewSize.isEmpty())
        return;

    // Recalculate the recommended layout size, when the available size (device pixel) changes.
    Settings& settings = m_page->settings();

    int minimumLayoutFallbackWidth = std::max<int>(settings.layoutFallbackWidth(), m_viewSize.width());

    // If unset  we use the viewport dimensions. This fits with the behavior of desktop browsers.
    int deviceWidth = (settings.deviceWidth() > 0) ? settings.deviceWidth() : m_viewSize.width();
    int deviceHeight = (settings.deviceHeight() > 0) ? settings.deviceHeight() : m_viewSize.height();

    ViewportAttributes attr = computeViewportAttributes(viewportArguments, minimumLayoutFallbackWidth, deviceWidth, deviceHeight, 1, m_viewSize);

    // If no layout was done yet set contentFixedOrigin to (0,0).
    IntPoint contentFixedOrigin = view->didFirstLayout() ? view->fixedVisibleContentRect().location() : IntPoint();

    // Put the width and height to the viewport width and height. In css units however.
    // Use FloatSize to avoid truncated values during scale.
    FloatSize contentFixedSize = m_viewSize;

    contentFixedSize.scale(1 / attr.initialScale);
    view->setFixedVisibleContentRect(IntRect(contentFixedOrigin, roundedIntSize(contentFixedSize)));

    attr.initialScale = m_page->viewportArguments().zoom; // Resets auto (-1) if no value was set by user.

    // This also takes care of the relayout.
    setFixedLayoutSize(roundedIntSize(attr.layoutSize));

#if USE(COORDINATED_GRAPHICS)
    m_drawingArea->didChangeViewportAttributes(WTFMove(attr));
#else
    send(Messages::WebPageProxy::DidChangeViewportProperties(attr));
#endif
}
#endif

void WebPage::scrollMainFrameIfNotAtMaxScrollPosition(const IntSize& scrollOffset)
{
    FrameView* frameView = m_page->mainFrame().view();

    ScrollPosition scrollPosition = frameView->scrollPosition();
    ScrollPosition maximumScrollPosition = frameView->maximumScrollPosition();

    // If the current scroll position in a direction is the max scroll position 
    // we don't want to scroll at all.
    IntSize newScrollOffset;
    if (scrollPosition.x() < maximumScrollPosition.x())
        newScrollOffset.setWidth(scrollOffset.width());
    if (scrollPosition.y() < maximumScrollPosition.y())
        newScrollOffset.setHeight(scrollOffset.height());

    if (newScrollOffset.isZero())
        return;

    frameView->setScrollPosition(frameView->scrollPosition() + newScrollOffset);
}

void WebPage::drawRect(GraphicsContext& graphicsContext, const IntRect& rect)
{
#if PLATFORM(MAC)
    FrameView* mainFrameView = m_page->mainFrame().view();
    LocalDefaultSystemAppearance localAppearance(mainFrameView ? mainFrameView->useDarkAppearance() : false);
#endif

    GraphicsContextStateSaver stateSaver(graphicsContext);
    graphicsContext.clip(rect);

    m_mainFrame->coreFrame()->view()->paint(graphicsContext, rect);

#if PLATFORM(GTK) || PLATFORM(WIN)
    if (!m_page->settings().acceleratedCompositingEnabled() && m_page->inspectorController().enabled() && m_page->inspectorController().shouldShowOverlay()) {
        graphicsContext.beginTransparencyLayer(1);
        m_page->inspectorController().drawHighlight(graphicsContext);
        graphicsContext.endTransparencyLayer();
    }
#endif
}

double WebPage::textZoomFactor() const
{
    Frame* frame = m_mainFrame->coreFrame();
    if (!frame)
        return 1;
    return frame->textZoomFactor();
}

void WebPage::setTextZoomFactor(double zoomFactor)
{
    Frame* frame = m_mainFrame->coreFrame();
    if (!frame)
        return;
    frame->setTextZoomFactor(static_cast<float>(zoomFactor));
}

double WebPage::pageZoomFactor() const
{
    Frame* frame = m_mainFrame->coreFrame();
    if (!frame)
        return 1;
    return frame->pageZoomFactor();
}

void WebPage::setPageZoomFactor(double zoomFactor)
{
    Frame* frame = m_mainFrame->coreFrame();
    if (!frame)
        return;
    frame->setPageZoomFactor(static_cast<float>(zoomFactor));
}

static void dumpHistoryItem(HistoryItem& item, size_t indent, bool isCurrentItem, StringBuilder& stringBuilder, const String& directoryName)
{
    if (isCurrentItem)
        stringBuilder.append("curr->  ");
    else {
        for (size_t i = 0; i < indent; ++i)
            stringBuilder.append(' ');
    }

    auto url = item.url();
    if (url.protocolIs("file")) {
        size_t start = url.string().find(directoryName);
        if (start == WTF::notFound)
            start = 0;
        else
            start += directoryName.length();
        stringBuilder.append("(file test):", StringView { url.string() }.substring(start));
    } else
        stringBuilder.append(url.string());

    auto& target = item.target();
    if (target.length())
        stringBuilder.append(" (in frame \"", target, "\")");

    if (item.isTargetItem())
        stringBuilder.append("  **nav target**");

    stringBuilder.append('\n');

    Vector<Ref<HistoryItem>> children;
    children.reserveInitialCapacity(item.children().size());
    for (auto& child : item.children())
        children.uncheckedAppend(child.copyRef());
    std::stable_sort(children.begin(), children.end(), [] (auto& a, auto& b) {
        return codePointCompare(a->target(), b->target()) < 0;
    });
    for (auto& child : children)
        dumpHistoryItem(child, indent + 4, false, stringBuilder, directoryName);
}

String WebPage::dumpHistoryForTesting(const String& directory)
{
    if (!m_page)
        return { };

    auto& list = m_page->backForward();
    
    StringBuilder builder;
    int begin = -list.backCount();
    if (list.itemAtIndex(begin)->url() == aboutBlankURL())
        ++begin;
    for (int i = begin; i <= static_cast<int>(list.forwardCount()); ++i)
        dumpHistoryItem(*list.itemAtIndex(i), 8, !i, builder, directory);
    return builder.toString();
}

void WebPage::clearHistory()
{
    if (!m_page)
        return;

    static_cast<WebBackForwardListProxy&>(m_page->backForward().client()).clear();
}

void WebPage::setPageAndTextZoomFactors(double pageZoomFactor, double textZoomFactor)
{
    Frame* frame = m_mainFrame->coreFrame();
    if (!frame)
        return;
    return frame->setPageAndTextZoomFactors(static_cast<float>(pageZoomFactor), static_cast<float>(textZoomFactor));
}

void WebPage::windowScreenDidChange(PlatformDisplayID displayID, std::optional<unsigned> nominalFramesPerSecond)
{
    m_page->chrome().windowScreenDidChange(displayID, nominalFramesPerSecond);

#if PLATFORM(MAC)
    WebProcess::singleton().updatePageScreenProperties();
#endif
}

void WebPage::scalePage(double scale, const IntPoint& origin)
{
    double totalScale = scale * viewScaleFactor();
    bool willChangeScaleFactor = totalScale != totalScaleFactor();

#if PLATFORM(IOS_FAMILY)
    if (willChangeScaleFactor) {
        if (!m_inDynamicSizeUpdate)
            m_dynamicSizeUpdateHistory.clear();
        m_scaleWasSetByUIProcess = false;
    }
#endif

    m_page->setPageScaleFactor(totalScale, origin);

    // We can't early return before setPageScaleFactor because the origin might be different.
    if (!willChangeScaleFactor)
        return;

#if USE(COORDINATED_GRAPHICS) || USE(TEXTURE_MAPPER)
    m_drawingArea->deviceOrPageScaleFactorChanged();
#endif

    send(Messages::WebPageProxy::PageScaleFactorDidChange(scale));
    platformDidScalePage();
}

#if !PLATFORM(IOS_FAMILY)

void WebPage::platformDidScalePage()
{
}

#endif

void WebPage::scalePageInViewCoordinates(double scale, IntPoint centerInViewCoordinates)
{
    double totalScale = scale * viewScaleFactor();
    if (totalScale == totalScaleFactor())
        return;

    IntPoint scrollPositionAtNewScale = mainFrameView()->rootViewToContents(-centerInViewCoordinates);
    double scaleRatio = scale / pageScaleFactor();
    scrollPositionAtNewScale.scale(scaleRatio);
    scalePage(scale, scrollPositionAtNewScale);
}

double WebPage::totalScaleFactor() const
{
    return m_page->pageScaleFactor();
}

double WebPage::pageScaleFactor() const
{
    return totalScaleFactor() / viewScaleFactor();
}

double WebPage::viewScaleFactor() const
{
    return m_page->viewScaleFactor();
}

void WebPage::scaleView(double scale)
{
    if (viewScaleFactor() == scale)
        return;

    float pageScale = pageScaleFactor();

    IntPoint scrollPositionAtNewScale;
    if (FrameView* mainFrameView = m_page->mainFrame().view()) {
        double scaleRatio = scale / viewScaleFactor();
        scrollPositionAtNewScale = mainFrameView->scrollPosition();
        scrollPositionAtNewScale.scale(scaleRatio);
    }

    m_page->setViewScaleFactor(scale);
    scalePage(pageScale, scrollPositionAtNewScale);
}

void WebPage::setDeviceScaleFactor(float scaleFactor)
{
    if (scaleFactor == m_page->deviceScaleFactor())
        return;

    m_page->setDeviceScaleFactor(scaleFactor);

    if (findController().isShowingOverlay()) {
        // We must have updated layout to get the selection rects right.
        layoutIfNeeded();
        findController().deviceScaleFactorDidChange();
    }

#if USE(COORDINATED_GRAPHICS) || USE(TEXTURE_MAPPER)
    m_drawingArea->deviceOrPageScaleFactorChanged();
#endif
}

float WebPage::deviceScaleFactor() const
{
    return m_page->deviceScaleFactor();
}

void WebPage::accessibilitySettingsDidChange()
{
    m_page->accessibilitySettingsDidChange();
}

void WebPage::screenPropertiesDidChange()
{
    m_page->screenPropertiesDidChange();
}

void WebPage::setUseFixedLayout(bool fixed)
{
    // Do not overwrite current settings if initially setting it to false.
    if (m_useFixedLayout == fixed)
        return;
    m_useFixedLayout = fixed;

#if !PLATFORM(IOS_FAMILY)
    m_page->settings().setFixedElementsLayoutRelativeToFrame(fixed);
#endif

    FrameView* view = mainFrameView();
    if (!view)
        return;

    view->setUseFixedLayout(fixed);
    if (!fixed)
        setFixedLayoutSize(IntSize());

    send(Messages::WebPageProxy::UseFixedLayoutDidChange(fixed));
}

bool WebPage::setFixedLayoutSize(const IntSize& size)
{
    FrameView* view = mainFrameView();
    if (!view || view->fixedLayoutSize() == size)
        return false;

    LOG_WITH_STREAM(VisibleRects, stream << "WebPage " << m_identifier.toUInt64() << " setFixedLayoutSize " << size);
    view->setFixedLayoutSize(size);

    send(Messages::WebPageProxy::FixedLayoutSizeDidChange(size));
    return true;
}

IntSize WebPage::fixedLayoutSize() const
{
    FrameView* view = mainFrameView();
    if (!view)
        return IntSize();
    return view->fixedLayoutSize();
}

void WebPage::disabledAdaptationsDidChange(const OptionSet<DisabledAdaptations>& disabledAdaptations)
{
#if PLATFORM(IOS_FAMILY)
    if (m_viewportConfiguration.setDisabledAdaptations(disabledAdaptations))
        viewportConfigurationChanged();
#else
    UNUSED_PARAM(disabledAdaptations);
#endif
}

void WebPage::viewportPropertiesDidChange(const ViewportArguments& viewportArguments)
{
#if PLATFORM(IOS_FAMILY)
    if (m_viewportConfiguration.setViewportArguments(viewportArguments))
        viewportConfigurationChanged();
#endif

#if USE(COORDINATED_GRAPHICS)
    FrameView* view = m_page->mainFrame().view();
    if (view && view->useFixedLayout())
        sendViewportAttributesChanged(viewportArguments);
    else
        m_drawingArea->didChangeViewportAttributes(ViewportAttributes());
#endif

#if !PLATFORM(IOS_FAMILY) && !USE(COORDINATED_GRAPHICS)
    UNUSED_PARAM(viewportArguments);
#endif
}

void WebPage::listenForLayoutMilestones(OptionSet<WebCore::LayoutMilestone> milestones)
{
    if (!m_page)
        return;
    m_page->addLayoutMilestones(milestones);
}

void WebPage::setSuppressScrollbarAnimations(bool suppressAnimations)
{
    m_page->setShouldSuppressScrollbarAnimations(suppressAnimations);
}
    
void WebPage::setEnableVerticalRubberBanding(bool enableVerticalRubberBanding)
{
    m_page->setVerticalScrollElasticity(enableVerticalRubberBanding ? ScrollElasticityAllowed : ScrollElasticityNone);
}
    
void WebPage::setEnableHorizontalRubberBanding(bool enableHorizontalRubberBanding)
{
    m_page->setHorizontalScrollElasticity(enableHorizontalRubberBanding ? ScrollElasticityAllowed : ScrollElasticityNone);
}

void WebPage::setBackgroundExtendsBeyondPage(bool backgroundExtendsBeyondPage)
{
    if (m_page->settings().backgroundShouldExtendBeyondPage() != backgroundExtendsBeyondPage)
        m_page->settings().setBackgroundShouldExtendBeyondPage(backgroundExtendsBeyondPage);
}

void WebPage::setPaginationMode(uint32_t mode)
{
    Pagination pagination = m_page->pagination();
    pagination.mode = static_cast<Pagination::Mode>(mode);
    m_page->setPagination(pagination);
}

void WebPage::setPaginationBehavesLikeColumns(bool behavesLikeColumns)
{
    Pagination pagination = m_page->pagination();
    pagination.behavesLikeColumns = behavesLikeColumns;
    m_page->setPagination(pagination);
}

void WebPage::setPageLength(double pageLength)
{
    Pagination pagination = m_page->pagination();
    pagination.pageLength = pageLength;
    m_page->setPagination(pagination);
}

void WebPage::setGapBetweenPages(double gap)
{
    Pagination pagination = m_page->pagination();
    pagination.gap = gap;
    m_page->setPagination(pagination);
}

void WebPage::setPaginationLineGridEnabled(bool lineGridEnabled)
{
    m_page->setPaginationLineGridEnabled(lineGridEnabled);
}

void WebPage::postInjectedBundleMessage(const String& messageName, const UserData& userData)
{
    auto& webProcess = WebProcess::singleton();
    InjectedBundle* injectedBundle = webProcess.injectedBundle();
    if (!injectedBundle)
        return;

    injectedBundle->didReceiveMessageToPage(this, messageName, webProcess.transformHandlesToObjects(userData.object()).get());
}

void WebPage::setUnderPageBackgroundColorOverride(WebCore::Color&& underPageBackgroundColorOverride)
{
    m_page->setUnderPageBackgroundColorOverride(WTFMove(underPageBackgroundColorOverride));
}

#if !PLATFORM(IOS_FAMILY)

void WebPage::setHeaderPageBanner(PageBanner* pageBanner)
{
    if (m_headerBanner)
        m_headerBanner->detachFromPage();

    m_headerBanner = pageBanner;

    if (m_headerBanner)
        m_headerBanner->addToPage(PageBanner::Header, this);
}

PageBanner* WebPage::headerPageBanner()
{
    return m_headerBanner.get();
}

void WebPage::setFooterPageBanner(PageBanner* pageBanner)
{
    if (m_footerBanner)
        m_footerBanner->detachFromPage();

    m_footerBanner = pageBanner;

    if (m_footerBanner)
        m_footerBanner->addToPage(PageBanner::Footer, this);
}

PageBanner* WebPage::footerPageBanner()
{
    return m_footerBanner.get();
}

void WebPage::hidePageBanners()
{
    if (m_headerBanner)
        m_headerBanner->hide();
    if (m_footerBanner)
        m_footerBanner->hide();
}

void WebPage::showPageBanners()
{
    if (m_headerBanner)
        m_headerBanner->showIfHidden();
    if (m_footerBanner)
        m_footerBanner->showIfHidden();
}

void WebPage::setHeaderBannerHeightForTesting(int height)
{
    corePage()->setHeaderHeight(height);
}

void WebPage::setFooterBannerHeightForTesting(int height)
{
    corePage()->setFooterHeight(height);
}

#endif // !PLATFORM(IOS_FAMILY)

void WebPage::takeSnapshot(IntRect snapshotRect, IntSize bitmapSize, uint32_t options, CompletionHandler<void(const WebKit::ShareableBitmap::Handle&)>&& completionHandler)
{
    SnapshotOptions snapshotOptions = static_cast<SnapshotOptions>(options);
    snapshotOptions |= SnapshotOptionsShareable;

    RefPtr<WebImage> image = snapshotAtSize(snapshotRect, bitmapSize, snapshotOptions);

    ShareableBitmap::Handle handle;
    if (image)
        image->bitmap().createHandle(handle, SharedMemory::Protection::ReadOnly);

    completionHandler(handle);
}

RefPtr<WebImage> WebPage::scaledSnapshotWithOptions(const IntRect& rect, double additionalScaleFactor, SnapshotOptions options)
{
    IntRect snapshotRect = rect;
    IntSize bitmapSize = snapshotRect.size();
    if (options & SnapshotOptionsPrinting) {
        ASSERT(additionalScaleFactor == 1);
        Frame* coreFrame = m_mainFrame->coreFrame();
        if (!coreFrame)
            return nullptr;
        bitmapSize.setHeight(PrintContext::numberOfPages(*coreFrame, bitmapSize) * (bitmapSize.height() + 1) - 1);
    } else {
        double scaleFactor = additionalScaleFactor;
        if (!(options & SnapshotOptionsExcludeDeviceScaleFactor))
            scaleFactor *= corePage()->deviceScaleFactor();
        bitmapSize.scale(scaleFactor);
    }

    return snapshotAtSize(rect, bitmapSize, options);
}

void WebPage::paintSnapshotAtSize(const IntRect& rect, const IntSize& bitmapSize, SnapshotOptions options, Frame& frame, FrameView& frameView, GraphicsContext& graphicsContext)
{
    TraceScope snapshotScope(PaintSnapshotStart, PaintSnapshotEnd, options);

    IntRect snapshotRect = rect;
    float horizontalScaleFactor = static_cast<float>(bitmapSize.width()) / rect.width();
    float verticalScaleFactor = static_cast<float>(bitmapSize.height()) / rect.height();
    float scaleFactor = std::max(horizontalScaleFactor, verticalScaleFactor);

    if (options & SnapshotOptionsPrinting) {
        PrintContext::spoolAllPagesWithBoundaries(frame, graphicsContext, snapshotRect.size());
        return;
    }

    Color documentBackgroundColor = frameView.documentBackgroundColor();
    Color backgroundColor = (frame.settings().backgroundShouldExtendBeyondPage() && documentBackgroundColor.isValid()) ? documentBackgroundColor : frameView.baseBackgroundColor();
    graphicsContext.fillRect(IntRect(IntPoint(), bitmapSize), backgroundColor);

    if (!(options & SnapshotOptionsExcludeDeviceScaleFactor)) {
        double deviceScaleFactor = frame.page()->deviceScaleFactor();
        graphicsContext.applyDeviceScaleFactor(deviceScaleFactor);
        scaleFactor /= deviceScaleFactor;
    }

    graphicsContext.scale(scaleFactor);
    graphicsContext.translate(-snapshotRect.location());

    FrameView::SelectionInSnapshot shouldPaintSelection = FrameView::IncludeSelection;
    if (options & SnapshotOptionsExcludeSelectionHighlighting)
        shouldPaintSelection = FrameView::ExcludeSelection;

    FrameView::CoordinateSpaceForSnapshot coordinateSpace = FrameView::DocumentCoordinates;
    if (options & SnapshotOptionsInViewCoordinates)
        coordinateSpace = FrameView::ViewCoordinates;

    frameView.paintContentsForSnapshot(graphicsContext, snapshotRect, shouldPaintSelection, coordinateSpace);

    if (options & SnapshotOptionsPaintSelectionRectangle) {
        FloatRect selectionRectangle = frame.selection().selectionBounds();
        graphicsContext.setStrokeColor(Color::red);
        graphicsContext.strokeRect(selectionRectangle, 1);
    }
}

static ShareableBitmap::Configuration snapshotOptionsToBitmapConfiguration(SnapshotOptions options, WebPage& page)
{
    ShareableBitmap::Configuration configuration;
#if USE(CG)
    if (options & SnapshotOptionsUseScreenColorSpace)
        configuration.colorSpace = screenColorSpace(page.corePage()->mainFrame().view());
#endif
    return configuration;
}

RefPtr<WebImage> WebPage::snapshotAtSize(const IntRect& rect, const IntSize& bitmapSize, SnapshotOptions options)
{
    Frame* coreFrame = m_mainFrame->coreFrame();
    if (!coreFrame)
        return nullptr;

    FrameView* frameView = coreFrame->view();
    if (!frameView)
        return nullptr;

    auto snapshot = WebImage::create(bitmapSize, snapshotOptionsToImageOptions(options), snapshotOptionsToBitmapConfiguration(options, *this));
    if (!snapshot)
        return nullptr;
    auto graphicsContext = snapshot->bitmap().createGraphicsContext();
    if (!graphicsContext)
        return nullptr;

    paintSnapshotAtSize(rect, bitmapSize, options, *coreFrame, *frameView, *graphicsContext);

    return snapshot;
}

RefPtr<WebImage> WebPage::snapshotNode(WebCore::Node& node, SnapshotOptions options, unsigned maximumPixelCount)
{
    Frame* coreFrame = m_mainFrame->coreFrame();
    if (!coreFrame)
        return nullptr;

    FrameView* frameView = coreFrame->view();
    if (!frameView)
        return nullptr;

    if (!node.renderer())
        return nullptr;

    LayoutRect topLevelRect;
    IntRect snapshotRect = snappedIntRect(node.renderer()->paintingRootRect(topLevelRect));
    if (snapshotRect.isEmpty())
        return nullptr;

    double scaleFactor = 1;
    IntSize snapshotSize = snapshotRect.size();
    unsigned maximumHeight = maximumPixelCount / snapshotSize.width();
    if (maximumHeight < static_cast<unsigned>(snapshotSize.height())) {
        scaleFactor = static_cast<double>(maximumHeight) / snapshotSize.height();
        snapshotSize = IntSize(snapshotSize.width() * scaleFactor, maximumHeight);
    }

    auto snapshot = WebImage::create(snapshotSize, snapshotOptionsToImageOptions(options), snapshotOptionsToBitmapConfiguration(options, *this));
    if (!snapshot)
        return nullptr;
    auto graphicsContext = snapshot->bitmap().createGraphicsContext();
    if (!graphicsContext)
        return nullptr;

    if (!(options & SnapshotOptionsExcludeDeviceScaleFactor)) {
        double deviceScaleFactor = corePage()->deviceScaleFactor();
        graphicsContext->applyDeviceScaleFactor(deviceScaleFactor);
        scaleFactor /= deviceScaleFactor;
    }

    graphicsContext->scale(scaleFactor);
    graphicsContext->translate(-snapshotRect.location());

    Color savedBackgroundColor = frameView->baseBackgroundColor();
    frameView->setBaseBackgroundColor(Color::transparentBlack);
    frameView->setNodeToDraw(&node);

    frameView->paintContentsForSnapshot(*graphicsContext, snapshotRect, FrameView::ExcludeSelection, FrameView::DocumentCoordinates);

    frameView->setBaseBackgroundColor(savedBackgroundColor);
    frameView->setNodeToDraw(nullptr);

    return snapshot;
}

void WebPage::pageDidScroll()
{
#if PLATFORM(IOS_FAMILY)
    if (!m_inDynamicSizeUpdate)
        m_dynamicSizeUpdateHistory.clear();
#endif
    m_uiClient->pageDidScroll(this);

    m_pageScrolledHysteresis.impulse();

    auto scrollPosition = m_page->mainFrame().view()->scrollPosition();
    send(Messages::WebPageProxy::PageDidScroll(scrollPosition));
}

void WebPage::pageStoppedScrolling()
{
    // Maintain the current history item's scroll position up-to-date.
    if (Frame* frame = m_mainFrame->coreFrame())
        frame->loader().history().saveScrollPositionAndViewStateToItem(frame->loader().history().currentItem());
}

#if ENABLE(CONTEXT_MENUS)
WebContextMenu& WebPage::contextMenu()
{
    if (!m_contextMenu)
        m_contextMenu = WebContextMenu::create(*this);
    return *m_contextMenu;
}

WebContextMenu* WebPage::contextMenuAtPointInWindow(const IntPoint& point)
{
    corePage()->contextMenuController().clearContextMenu();

    // Simulate a mouse click to generate the correct menu.
    PlatformMouseEvent mousePressEvent(point, point, RightButton, PlatformEvent::MousePressed, 1, false, false, false, false, WallTime::now(), WebCore::ForceAtClick, WebCore::NoTap);
    corePage()->userInputBridge().handleMousePressEvent(mousePressEvent);
    Ref mainFrame = corePage()->mainFrame();
    bool handled = corePage()->userInputBridge().handleContextMenuEvent(mousePressEvent, mainFrame);
    auto* menu = handled ? &contextMenu() : nullptr;
    PlatformMouseEvent mouseReleaseEvent(point, point, RightButton, PlatformEvent::MouseReleased, 1, false, false, false, false, WallTime::now(), WebCore::ForceAtClick, WebCore::NoTap);
    corePage()->userInputBridge().handleMouseReleaseEvent(mouseReleaseEvent);

    return menu;
}
#endif

// Events 

static const WebEvent* g_currentEvent = 0;

// FIXME: WebPage::currentEvent is used by the plug-in code to avoid having to convert from DOM events back to
// WebEvents. When we get the event handling sorted out, this should go away and the Widgets should get the correct
// platform events passed to the event handler code.
const WebEvent* WebPage::currentEvent()
{
    return g_currentEvent;
}

void WebPage::freezeLayerTree(LayerTreeFreezeReason reason)
{
    auto oldReasons = m_layerTreeFreezeReasons.toRaw();
    UNUSED_PARAM(oldReasons);
    m_layerTreeFreezeReasons.add(reason);
    WEBPAGE_RELEASE_LOG(ProcessSuspension, "freezeLayerTree: Adding a reason to freeze layer tree (reason=%d, new=%d, old=%d)", static_cast<unsigned>(reason), m_layerTreeFreezeReasons.toRaw(), oldReasons);
    updateDrawingAreaLayerTreeFreezeState();
}

void WebPage::unfreezeLayerTree(LayerTreeFreezeReason reason)
{
    auto oldReasons = m_layerTreeFreezeReasons.toRaw();
    UNUSED_PARAM(oldReasons);
    m_layerTreeFreezeReasons.remove(reason);
    WEBPAGE_RELEASE_LOG(ProcessSuspension, "unfreezeLayerTree: Removing a reason to freeze layer tree (reason=%d, new=%d, old=%d)", static_cast<unsigned>(reason), m_layerTreeFreezeReasons.toRaw(), oldReasons);
    updateDrawingAreaLayerTreeFreezeState();
}

void WebPage::updateDrawingAreaLayerTreeFreezeState()
{
    if (!m_drawingArea)
        return;

    m_drawingArea->setLayerTreeStateIsFrozen(!!m_layerTreeFreezeReasons);
}

void WebPage::callVolatilityCompletionHandlers(bool succeeded)
{
    auto completionHandlers = std::exchange(m_markLayersAsVolatileCompletionHandlers, { });
    for (auto& completionHandler : completionHandlers)
        completionHandler(succeeded);
}

void WebPage::layerVolatilityTimerFired()
{
    Seconds newInterval = m_layerVolatilityTimer.repeatInterval() * 2.;
    bool didSucceed = markLayersVolatileImmediatelyIfPossible();
    if (didSucceed || newInterval > maximumLayerVolatilityTimerInterval) {
        m_layerVolatilityTimer.stop();
        if (didSucceed)
            WEBPAGE_RELEASE_LOG(Layers, "layerVolatilityTimerFired: Succeeded in marking layers as volatile");
        else
            WEBPAGE_RELEASE_LOG(Layers, "layerVolatilityTimerFired: Failed to mark layers as volatile within %gms", maximumLayerVolatilityTimerInterval.milliseconds());
        callVolatilityCompletionHandlers(didSucceed);
        return;
    }

    WEBPAGE_RELEASE_LOG_ERROR(Layers, "layerVolatilityTimerFired: Failed to mark all layers as volatile, will retry in %g ms", newInterval.milliseconds());
    m_layerVolatilityTimer.startRepeating(newInterval);
}

bool WebPage::markLayersVolatileImmediatelyIfPossible()
{
    return !drawingArea() || drawingArea()->markLayersVolatileImmediatelyIfPossible();
}

void WebPage::markLayersVolatile(CompletionHandler<void(bool)>&& completionHandler)
{
    WEBPAGE_RELEASE_LOG(Layers, "markLayersVolatile:");

    if (m_layerVolatilityTimer.isActive())
        m_layerVolatilityTimer.stop();

    if (completionHandler)
        m_markLayersAsVolatileCompletionHandlers.append(WTFMove(completionHandler));

    bool didSucceed = markLayersVolatileImmediatelyIfPossible();
    if (didSucceed || m_isSuspendedUnderLock) {
        if (didSucceed)
            WEBPAGE_RELEASE_LOG(Layers, "markLayersVolatile: Successfully marked layers as volatile");
        else {
            // If we get suspended when locking the screen, it is expected that some IOSurfaces cannot be marked as purgeable so we do not keep retrying.
            WEBPAGE_RELEASE_LOG(Layers, "markLayersVolatile: Did what we could to mark IOSurfaces as purgeable after locking the screen");
        }
        callVolatilityCompletionHandlers(didSucceed);
        return;
    }

    WEBPAGE_RELEASE_LOG(Layers, "markLayersVolatile: Failed to mark all layers as volatile, will retry in %g ms", initialLayerVolatilityTimerInterval.milliseconds());
    m_layerVolatilityTimer.startRepeating(initialLayerVolatilityTimerInterval);
}

void WebPage::cancelMarkLayersVolatile()
{
    WEBPAGE_RELEASE_LOG(Layers, "cancelMarkLayersVolatile:");
    m_layerVolatilityTimer.stop();
    callVolatilityCompletionHandlers(false);
}

class CurrentEvent {
public:
    explicit CurrentEvent(const WebEvent& event)
        : m_previousCurrentEvent(g_currentEvent)
    {
        g_currentEvent = &event;
    }

    ~CurrentEvent()
    {
        g_currentEvent = m_previousCurrentEvent;
    }

private:
    const WebEvent* m_previousCurrentEvent;
};

#if ENABLE(CONTEXT_MENUS)

void WebPage::didShowContextMenu()
{
    m_waitingForContextMenuToShow = false;
}

void WebPage::didDismissContextMenu()
{
    corePage()->contextMenuController().didDismissContextMenu();
}

#endif // ENABLE(CONTEXT_MENUS)

#if ENABLE(CONTEXT_MENU_EVENT)
static bool isContextClick(const PlatformMouseEvent& event)
{
#if USE(APPKIT)
    return WebEventFactory::shouldBeHandledAsContextClick(event);
#else
    return event.button() == WebCore::RightButton;
#endif
}

static bool handleContextMenuEvent(const PlatformMouseEvent& platformMouseEvent, WebPage* page)
{
    IntPoint point = page->corePage()->mainFrame().view()->windowToContents(platformMouseEvent.position());
    constexpr OptionSet<HitTestRequest::Type> hitType { HitTestRequest::Type::ReadOnly, HitTestRequest::Type::Active, HitTestRequest::Type::DisallowUserAgentShadowContent,  HitTestRequest::Type::AllowChildFrameContent };
    HitTestResult result = page->corePage()->mainFrame().eventHandler().hitTestResultAtPoint(point, hitType);

    Ref frame = page->corePage()->mainFrame();
    if (result.innerNonSharedNode())
        frame = *result.innerNonSharedNode()->document().frame();

    bool handled = page->corePage()->userInputBridge().handleContextMenuEvent(platformMouseEvent, frame);
#if ENABLE(CONTEXT_MENUS)
    if (handled)
        page->contextMenu().show();
#endif
    return handled;
}

void WebPage::contextMenuForKeyEvent()
{
#if ENABLE(CONTEXT_MENUS)
    corePage()->contextMenuController().clearContextMenu();
#endif

    Frame& frame = m_page->focusController().focusedOrMainFrame();
    bool handled = frame.eventHandler().sendContextMenuEventForKey();
#if ENABLE(CONTEXT_MENUS)
    if (handled)
        contextMenu().show();
#else
    UNUSED_PARAM(handled);
#endif
}
#endif

static bool handleMouseEvent(const WebMouseEvent& mouseEvent, WebPage* page)
{
    Frame& frame = page->corePage()->mainFrame();
    if (!frame.view())
        return false;

    PlatformMouseEvent platformMouseEvent = platform(mouseEvent);

    switch (platformMouseEvent.type()) {
        case PlatformEvent::MousePressed: {
#if ENABLE(CONTEXT_MENUS)
            if (isContextClick(platformMouseEvent))
                page->corePage()->contextMenuController().clearContextMenu();
#endif

            bool handled = page->corePage()->userInputBridge().handleMousePressEvent(platformMouseEvent);
#if ENABLE(CONTEXT_MENU_EVENT)
            if (isContextClick(platformMouseEvent))
                handled = handleContextMenuEvent(platformMouseEvent, page);
#endif
            return handled;
        }
        case PlatformEvent::MouseReleased:
            if (mouseEvent.gestureWasCancelled() == GestureWasCancelled::Yes)
                frame.eventHandler().invalidateClick();
            return page->corePage()->userInputBridge().handleMouseReleaseEvent(platformMouseEvent);

        case PlatformEvent::MouseMoved:
#if PLATFORM(COCOA)
            // We need to do a full, normal hit test during this mouse event if the page is active or if a mouse
            // button is currently pressed. It is possible that neither of those things will be true since on
            // Lion when legacy scrollbars are enabled, WebKit receives mouse events all the time. If it is one
            // of those cases where the page is not active and the mouse is not pressed, then we can fire a more
            // efficient scrollbars-only version of the event.
            if (!(page->corePage()->focusController().isActive() || (mouseEvent.button() != WebMouseEvent::NoButton)))
                return page->corePage()->userInputBridge().handleMouseMoveOnScrollbarEvent(platformMouseEvent);
#endif
            return page->corePage()->userInputBridge().handleMouseMoveEvent(platformMouseEvent);

        case PlatformEvent::MouseForceChanged:
        case PlatformEvent::MouseForceDown:
        case PlatformEvent::MouseForceUp:
            return page->corePage()->userInputBridge().handleMouseForceEvent(platformMouseEvent);

        default:
            ASSERT_NOT_REACHED();
            return false;
    }
}

void WebPage::mouseEvent(const WebMouseEvent& mouseEvent, std::optional<Vector<SandboxExtension::Handle>>&& sandboxExtensions)
{
    SetForScope<bool> userIsInteractingChange { m_userIsInteracting, true };

    m_userActivity.impulse();

    bool shouldHandleEvent = true;

#if ENABLE(CONTEXT_MENUS)
    // Don't try to handle any pending mouse events if a context menu is showing.
    if (m_waitingForContextMenuToShow)
        shouldHandleEvent = false;
#endif
#if ENABLE(DRAG_SUPPORT)
    if (m_isStartingDrag)
        shouldHandleEvent = false;
#endif

    if (!shouldHandleEvent) {
        send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(mouseEvent.type()), false));
        return;
    }

    Vector<RefPtr<SandboxExtension>> mouseEventSandboxExtensions;
    if (sandboxExtensions)
        mouseEventSandboxExtensions = consumeSandboxExtensions(WTFMove(*sandboxExtensions));

    bool handled = false;

#if !PLATFORM(IOS_FAMILY)
    if (!handled && m_headerBanner)
        handled = m_headerBanner->mouseEvent(mouseEvent);
    if (!handled && m_footerBanner)
        handled = m_footerBanner->mouseEvent(mouseEvent);
#endif // !PLATFORM(IOS_FAMILY)

    if (!handled) {
        CurrentEvent currentEvent(mouseEvent);
        handled = handleMouseEvent(mouseEvent, this);
    }

    send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(mouseEvent.type()), handled));

    revokeSandboxExtensions(mouseEventSandboxExtensions);
}

static bool handleWheelEvent(const WebWheelEvent& wheelEvent, Page* page, OptionSet<WheelEventProcessingSteps> processingSteps)
{
    Frame& frame = page->mainFrame();
    if (!frame.view())
        return false;

    PlatformWheelEvent platformWheelEvent = platform(wheelEvent);
    return page->userInputBridge().handleWheelEvent(platformWheelEvent, processingSteps);
}

bool WebPage::wheelEvent(const WebWheelEvent& wheelEvent, OptionSet<WheelEventProcessingSteps> processingSteps)
{
    m_userActivity.impulse();

    CurrentEvent currentEvent(wheelEvent);

    bool handled = handleWheelEvent(wheelEvent, m_page.get(), processingSteps);

    if (processingSteps.contains(WheelEventProcessingSteps::MainThreadForScrolling))
        send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(wheelEvent.type()), handled));

    return handled;
}

void WebPage::dispatchWheelEventWithoutScrolling(const WebWheelEvent& wheelEvent, CompletionHandler<void(bool)>&& completionHandler)
{
#if ENABLE(KINETIC_SCROLLING)
    auto gestureState = m_page->mainFrame().eventHandler().wheelScrollGestureState();
    bool isCancelable = !gestureState || gestureState == WheelScrollGestureState::Blocking || wheelEvent.phase() == WebWheelEvent::PhaseBegan;
#else
    bool isCancelable = true;
#endif
    bool handled = this->wheelEvent(wheelEvent, { isCancelable ? WheelEventProcessingSteps::MainThreadForBlockingDOMEventDispatch : WheelEventProcessingSteps::MainThreadForNonBlockingDOMEventDispatch });
    completionHandler(handled);
}

static bool handleKeyEvent(const WebKeyboardEvent& keyboardEvent, Page* page)
{
    if (!page->mainFrame().view())
        return false;

    if (keyboardEvent.type() == WebEvent::Char && keyboardEvent.isSystemKey())
        return page->userInputBridge().handleAccessKeyEvent(platform(keyboardEvent));
    return page->userInputBridge().handleKeyEvent(platform(keyboardEvent));
}

void WebPage::keyEvent(const WebKeyboardEvent& keyboardEvent)
{
    SetForScope<bool> userIsInteractingChange { m_userIsInteracting, true };

    m_userActivity.impulse();

    PlatformKeyboardEvent::setCurrentModifierState(platform(keyboardEvent).modifiers());

    CurrentEvent currentEvent(keyboardEvent);

    bool handled = handleKeyEvent(keyboardEvent, m_page.get());
    // FIXME: Platform default behaviors should be performed during normal DOM event dispatch (in most cases, in default keydown event handler).
    if (!handled)
        handled = performDefaultBehaviorForKeyEvent(keyboardEvent);

    send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(keyboardEvent.type()), handled));
}

bool WebPage::handleKeyEventByRelinquishingFocusToChrome(const KeyboardEvent& event)
{
    if (m_page->tabKeyCyclesThroughElements())
        return false;

    if (event.charCode() != '\t')
        return false;

    if (!event.shiftKey() || event.ctrlKey() || event.metaKey() || event.altGraphKey())
        return false;

    ASSERT(event.type() == eventNames().keypressEvent);
    // Allow a shift-tab keypress event to relinquish focus even if we don't allow tab to cycle between
    // elements inside the view. We can only do this for shift-tab, not tab itself because
    // tabKeyCyclesThroughElements is used to make tab character insertion work in editable web views.
    return m_page->focusController().relinquishFocusToChrome(FocusDirection::Backward);
}

void WebPage::validateCommand(const String& commandName, CompletionHandler<void(bool, int32_t)>&& completionHandler)
{
    bool isEnabled = false;
    int32_t state = 0;
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    Editor::Command command = frame.editor().command(commandName);
    state = (command.state() != TriState::False);
    isEnabled = command.isSupported() && command.isEnabled();

    completionHandler(isEnabled, state);
}

void WebPage::executeEditCommand(const String& commandName, const String& argument)
{
    executeEditingCommand(commandName, argument);
}

void WebPage::setNeedsFontAttributes(bool needsFontAttributes)
{
    if (m_needsFontAttributes == needsFontAttributes)
        return;

    m_needsFontAttributes = needsFontAttributes;

    if (m_needsFontAttributes)
        scheduleFullEditorStateUpdate();
}

void WebPage::restoreSessionInternal(const Vector<BackForwardListItemState>& itemStates, WasRestoredByAPIRequest restoredByAPIRequest, WebBackForwardListProxy::OverwriteExistingItem overwrite)
{
    // Since we're merely restoring HistoryItems from the UIProcess, there is no need to send HistoryItem update notifications back to the UIProcess.
    // Also, with process-swap on navigation, these updates may actually overwrite important state in the UIProcess such as the scroll position.
    SetForScope<void (*)(WebCore::HistoryItem&)> bypassHistoryItemUpdateNotifications(WebCore::notifyHistoryItemChanged, [](WebCore::HistoryItem&){});
    for (const auto& itemState : itemStates) {
        auto historyItem = toHistoryItem(itemState);
        historyItem->setWasRestoredFromSession(restoredByAPIRequest == WasRestoredByAPIRequest::Yes);
        static_cast<WebBackForwardListProxy&>(corePage()->backForward().client()).addItemFromUIProcess(itemState.identifier, WTFMove(historyItem), m_identifier, overwrite);
    }
}

void WebPage::restoreSession(const Vector<BackForwardListItemState>& itemStates)
{
    restoreSessionInternal(itemStates, WasRestoredByAPIRequest::Yes, WebBackForwardListProxy::OverwriteExistingItem::No);
}

void WebPage::updateBackForwardListForReattach(const Vector<WebKit::BackForwardListItemState>& itemStates)
{
    restoreSessionInternal(itemStates, WasRestoredByAPIRequest::No, WebBackForwardListProxy::OverwriteExistingItem::Yes);
}

void WebPage::setCurrentHistoryItemForReattach(WebKit::BackForwardListItemState&& itemState)
{
    auto historyItem = toHistoryItem(itemState);
    auto& historyItemRef = historyItem.get();
    static_cast<WebBackForwardListProxy&>(corePage()->backForward().client()).addItemFromUIProcess(itemState.identifier, WTFMove(historyItem), m_identifier, WebBackForwardListProxy::OverwriteExistingItem::Yes);
    corePage()->mainFrame().loader().history().setCurrentItem(historyItemRef);
}

void WebPage::requestFontAttributesAtSelectionStart(CompletionHandler<void(const WebCore::FontAttributes&)>&& completionHandler)
{
    completionHandler(m_page->focusController().focusedOrMainFrame().editor().fontAttributesAtSelectionStart());
}

void WebPage::cancelGesturesBlockedOnSynchronousReplies()
{
#if ENABLE(IOS_TOUCH_EVENTS)
    if (auto reply = WTFMove(m_pendingSynchronousTouchEventReply))
        reply(true);
#endif

#if PLATFORM(IOS_FAMILY)
    if (auto reply = WTFMove(m_pendingSynchronousPositionInformationReply))
        reply(InteractionInformationAtPosition::invalidInformation());
#endif
}

#if ENABLE(TOUCH_EVENTS)
static bool handleTouchEvent(const WebTouchEvent& touchEvent, Page* page)
{
    if (!page->mainFrame().view())
        return false;

    return page->mainFrame().eventHandler().handleTouchEvent(platform(touchEvent));
}
#endif

#if ENABLE(IOS_TOUCH_EVENTS)
bool WebPage::dispatchTouchEvent(const WebTouchEvent& touchEvent)
{
    SetForScope<bool> userIsInteractingChange { m_userIsInteracting, true };
    m_lastInteractionLocation = touchEvent.position();
    CurrentEvent currentEvent(touchEvent);
    bool handled = handleTouchEvent(touchEvent, m_page.get());
    updatePotentialTapSecurityOrigin(touchEvent, handled);
    return handled;
}

void WebPage::touchEventSync(const WebTouchEvent& touchEvent, CompletionHandler<void(bool)>&& reply)
{
    // Avoid UIProcess hangs when the WebContent process is stuck on a sync IPC.
    if (IPC::UnboundedSynchronousIPCScope::hasOngoingUnboundedSyncIPC()) {
        WEBPAGE_RELEASE_LOG_ERROR(Process, "touchEventSync: Not processing because the process is stuck on unbounded sync IPC");
        return reply(true);
    }

    m_pendingSynchronousTouchEventReply = WTFMove(reply);

    EventDispatcher::TouchEventQueue queuedEvents;
    WebProcess::singleton().eventDispatcher().takeQueuedTouchEventsForPage(*this, queuedEvents);
    dispatchAsynchronousTouchEvents(WTFMove(queuedEvents));

    bool handled = dispatchTouchEvent(touchEvent);

    if (auto reply = WTFMove(m_pendingSynchronousTouchEventReply))
        reply(handled);
}

void WebPage::resetPotentialTapSecurityOrigin()
{
    m_potentialTapSecurityOrigin = nullptr;
}

void WebPage::updatePotentialTapSecurityOrigin(const WebTouchEvent& touchEvent, bool wasHandled)
{
    if (wasHandled)
        return;

    if (!touchEvent.isPotentialTap())
        return;

    if (touchEvent.type() != WebEvent::TouchStart)
        return;

    auto& mainFrame = m_page->mainFrame();
    auto document = mainFrame.document();
    if (!document)
        return;

    if (!document->handlingTouchEvent())
        return;

    Frame* touchEventTargetFrame = &mainFrame;
    while (auto subframe = touchEventTargetFrame->eventHandler().touchEventTargetSubframe())
        touchEventTargetFrame = subframe;

    auto& touches = touchEventTargetFrame->eventHandler().touches();
    if (touches.isEmpty())
        return;

    ASSERT(touches.size() == 1);

    if (auto targetDocument = touchEventTargetFrame->document())
        m_potentialTapSecurityOrigin = &targetDocument->securityOrigin();
}
#elif ENABLE(TOUCH_EVENTS)
void WebPage::touchEvent(const WebTouchEvent& touchEvent)
{
    CurrentEvent currentEvent(touchEvent);

    bool handled = handleTouchEvent(touchEvent, m_page.get());

    send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(touchEvent.type()), handled));
}
#endif

void WebPage::cancelPointer(WebCore::PointerID pointerId, const WebCore::IntPoint& documentPoint)
{
    m_page->pointerCaptureController().cancelPointer(pointerId, documentPoint);
}

void WebPage::touchWithIdentifierWasRemoved(WebCore::PointerID pointerId)
{
    m_page->pointerCaptureController().touchWithIdentifierWasRemoved(pointerId);
}

#if ENABLE(MAC_GESTURE_EVENTS)
static bool handleGestureEvent(const WebGestureEvent& event, Page* page)
{
    if (!page->mainFrame().view())
        return false;

    return page->mainFrame().eventHandler().handleGestureEvent(platform(event));
}

void WebPage::gestureEvent(const WebGestureEvent& gestureEvent)
{
    CurrentEvent currentEvent(gestureEvent);
    bool handled = handleGestureEvent(gestureEvent, m_page.get());
    send(Messages::WebPageProxy::DidReceiveEvent(static_cast<uint32_t>(gestureEvent.type()), handled));
}
#endif

bool WebPage::scroll(Page* page, ScrollDirection direction, ScrollGranularity granularity)
{
    return page->userInputBridge().scrollRecursively(direction, granularity);
}

bool WebPage::logicalScroll(Page* page, ScrollLogicalDirection direction, ScrollGranularity granularity)
{
    return page->userInputBridge().logicalScrollRecursively(direction, granularity);
}

bool WebPage::scrollBy(uint32_t scrollDirection, uint32_t scrollGranularity)
{
    return scroll(m_page.get(), static_cast<ScrollDirection>(scrollDirection), static_cast<ScrollGranularity>(scrollGranularity));
}

void WebPage::centerSelectionInVisibleArea()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.selection().revealSelection(SelectionRevealMode::Reveal, ScrollAlignment::alignCenterAlways);
    findController().showFindIndicatorInSelection();
}

void WebPage::connectInspector(const String& targetId, Inspector::FrontendChannel::ConnectionType connectionType)
{
    m_inspectorTargetController->connectInspector(targetId, connectionType);
}

void WebPage::disconnectInspector(const String& targetId)
{
    m_inspectorTargetController->disconnectInspector(targetId);
}

void WebPage::sendMessageToTargetBackend(const String& targetId, const String& message)
{
    m_inspectorTargetController->sendMessageToTargetBackend(targetId, message);
}

void WebPage::insertNewlineInQuotedContent()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.selection().isNone())
        return;
    frame.editor().insertParagraphSeparatorInQuotedContent();
}

#if ENABLE(REMOTE_INSPECTOR)
void WebPage::setIndicating(bool indicating)
{
    m_page->inspectorController().setIndicating(indicating);
}
#endif

void WebPage::setBackgroundColor(const std::optional<WebCore::Color>& backgroundColor)
{
    if (m_backgroundColor == backgroundColor)
        return;

    m_backgroundColor = backgroundColor;

    if (FrameView* frameView = mainFrameView())
        frameView->updateBackgroundRecursively(backgroundColor);

    m_drawingArea->setNeedsDisplay();
}

#if PLATFORM(COCOA)
void WebPage::setTopContentInsetFenced(float contentInset, const WTF::MachSendRight& machSendRight)
{
    m_drawingArea->addFence(machSendRight);
    setTopContentInset(contentInset);
}
#endif

void WebPage::setTopContentInset(float contentInset)
{
    if (contentInset == m_page->topContentInset())
        return;

    m_page->setTopContentInset(contentInset);
}

void WebPage::viewWillStartLiveResize()
{
    if (!m_page)
        return;

    // FIXME: This should propagate to all ScrollableAreas.
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (FrameView* view = frame.view())
        view->willStartLiveResize();
}

void WebPage::viewWillEndLiveResize()
{
    if (!m_page)
        return;

    // FIXME: This should propagate to all ScrollableAreas.
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (FrameView* view = frame.view())
        view->willEndLiveResize();
}

void WebPage::setInitialFocus(bool forward, bool isKeyboardEventValid, const WebKeyboardEvent& event, CompletionHandler<void()>&& completionHandler)
{
    if (!m_page)
        return completionHandler();

    SetForScope<bool> userIsInteractingChange { m_userIsInteracting, true };

    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.document()->setFocusedElement(0);

    if (isKeyboardEventValid && event.type() == WebEvent::KeyDown) {
        PlatformKeyboardEvent platformEvent(platform(event));
        platformEvent.disambiguateKeyDownEvent(PlatformEvent::RawKeyDown);
        m_page->focusController().setInitialFocus(forward ? FocusDirection::Forward : FocusDirection::Backward, &KeyboardEvent::create(platformEvent, &frame.windowProxy()).get());
        completionHandler();
        return;
    }

    m_page->focusController().setInitialFocus(forward ? FocusDirection::Forward : FocusDirection::Backward, nullptr);
    completionHandler();
}

void WebPage::setCanStartMediaTimerFired()
{
    if (m_page)
        m_page->setCanStartMedia(true);
}

void WebPage::updateIsInWindow(bool isInitialState)
{
    bool isInWindow = m_activityState.contains(WebCore::ActivityState::IsInWindow);

    if (!isInWindow) {
        m_setCanStartMediaTimer.stop();
        m_page->setCanStartMedia(false);
        
        // The WebProcess does not yet know about this page; no need to tell it we're leaving the window.
        if (!isInitialState)
            WebProcess::singleton().pageWillLeaveWindow(m_identifier);
    } else {
        // Defer the call to Page::setCanStartMedia() since it ends up sending a synchronous message to the UI process
        // in order to get plug-in connections, and the UI process will be waiting for the Web process to update the backing
        // store after moving the view into a window, until it times out and paints white. See <rdar://problem/9242771>.
        if (m_mayStartMediaWhenInWindow)
            m_setCanStartMediaTimer.startOneShot(0_s);

        WebProcess::singleton().pageDidEnterWindow(m_identifier);
    }

    if (isInWindow)
        layoutIfNeeded();
}

void WebPage::visibilityDidChange()
{
    bool isVisible = m_activityState.contains(ActivityState::IsVisible);
    if (!isVisible) {
        // We save the document / scroll state when backgrounding a tab so that we are able to restore it
        // if it gets terminated while in the background.
        if (auto* frame = m_mainFrame->coreFrame())
            frame->loader().history().saveDocumentAndScrollState();
    }
}

void WebPage::setActivityState(OptionSet<ActivityState::Flag> activityState, ActivityStateChangeID activityStateChangeID, CompletionHandler<void()>&& callback)
{
    LOG_WITH_STREAM(ActivityState, stream << "WebPage " << identifier().toUInt64() << " setActivityState to " << activityState);

    auto changed = m_activityState ^ activityState;
    m_activityState = activityState;

    if (changed)
        updateThrottleState();

    ASSERT_WITH_MESSAGE(m_page, "setActivityState called on %" PRIu64 " but WebCore page was null", identifier().toUInt64());
    if (m_page) {
        SetForScope<OptionSet<ActivityState::Flag>> currentlyChangingActivityState { m_lastActivityStateChanges, changed };
        m_page->setActivityState(activityState);
    }

    m_drawingArea->activityStateDidChange(changed, activityStateChangeID, WTFMove(callback));
    WebProcess::singleton().pageActivityStateDidChange(m_identifier, changed);

    if (changed & ActivityState::IsInWindow)
        updateIsInWindow();

    if (changed & ActivityState::IsVisible)
        visibilityDidChange();
}

void WebPage::setLayerHostingMode(LayerHostingMode layerHostingMode)
{
    m_layerHostingMode = layerHostingMode;

    m_drawingArea->setLayerHostingMode(m_layerHostingMode);
}

void WebPage::didReceivePolicyDecision(FrameIdentifier frameID, uint64_t listenerID, PolicyDecision&& policyDecision, const Vector<SandboxExtension::Handle>& networkExtensionsHandles)
{
    consumeNetworkExtensionSandboxExtensions(networkExtensionsHandles);

    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    WEBPAGE_RELEASE_LOG(Loading, "didReceivePolicyDecision: policyAction=%u - frameID=%llu - webFrame=%p - mainFrame=%d", (unsigned)policyDecision.policyAction, frameID.toUInt64(), frame, frame ? frame->isMainFrame() : 0);

    if (!frame)
        return;
#if ENABLE(APP_BOUND_DOMAINS)
    setIsNavigatingToAppBoundDomain(policyDecision.isNavigatingToAppBoundDomain, frame);
#endif
    frame->didReceivePolicyDecision(listenerID, WTFMove(policyDecision));
}

void WebPage::continueWillSubmitForm(FrameIdentifier frameID, FormSubmitListenerIdentifier listenerID)
{
    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;
    frame->continueWillSubmitForm(listenerID);
}

void WebPage::didStartPageTransition()
{
    freezeLayerTree(LayerTreeFreezeReason::PageTransition);

#if PLATFORM(MAC)
    bool hasPreviouslyFocusedDueToUserInteraction = m_hasEverFocusedElementDueToUserInteractionSincePageTransition;
#endif
    m_hasEverFocusedElementDueToUserInteractionSincePageTransition = false;
    m_lastEditorStateWasContentEditable = EditorStateIsContentEditable::Unset;

#if PLATFORM(MAC)
    if (hasPreviouslyFocusedDueToUserInteraction)
        send(Messages::WebPageProxy::SetHasHadSelectionChangesFromUserInteraction(m_hasEverFocusedElementDueToUserInteractionSincePageTransition));
#endif

#if PLATFORM(IOS_FAMILY)
    m_isShowingInputViewForFocusedElement = false;
    // This is used to enable a first-tap quirk.
    m_hasHandledSyntheticClick = false;
#endif
}

void WebPage::didCompletePageTransition()
{
    unfreezeLayerTree(LayerTreeFreezeReason::PageTransition);
}

void WebPage::show()
{
    send(Messages::WebPageProxy::ShowPage());
}

void WebPage::setIsTakingSnapshotsForApplicationSuspension(bool isTakingSnapshotsForApplicationSuspension)
{
    WEBPAGE_RELEASE_LOG(Resize, "setIsTakingSnapshotsForApplicationSuspension(%d)", isTakingSnapshotsForApplicationSuspension);

    if (m_page)
        m_page->setIsTakingSnapshotsForApplicationSuspension(isTakingSnapshotsForApplicationSuspension);
}

void WebPage::setNeedsDOMWindowResizeEvent()
{
    if (!m_page)
        return;

    if (auto* document = m_page->mainFrame().document())
        document->setNeedsDOMWindowResizeEvent();
}

String WebPage::userAgent(const URL& webCoreURL) const
{
    String userAgent = platformUserAgent(webCoreURL);
    if (!userAgent.isEmpty())
        return userAgent;
    return m_userAgent;
}

void WebPage::setUserAgent(const String& userAgent)
{
    if (m_userAgent == userAgent)
        return;

    m_userAgent = userAgent;

    if (m_page)
        m_page->userAgentChanged();
}

void WebPage::suspendActiveDOMObjectsAndAnimations()
{
    m_page->suspendActiveDOMObjectsAndAnimations();
}

void WebPage::resumeActiveDOMObjectsAndAnimations()
{
    m_page->resumeActiveDOMObjectsAndAnimations();
}

void WebPage::suspend(CompletionHandler<void(bool)>&& completionHandler)
{
    WEBPAGE_RELEASE_LOG(Loading, "suspend: m_page=%p", m_page.get());
    if (!m_page)
        return completionHandler(false);

    freezeLayerTree(LayerTreeFreezeReason::PageSuspended);

    m_cachedPage = BackForwardCache::singleton().suspendPage(*m_page);
    ASSERT(m_cachedPage);
    if (auto mainFrame = m_mainFrame->coreFrame())
        mainFrame->loader().detachFromAllOpenedFrames();
    completionHandler(true);
}

void WebPage::resume(CompletionHandler<void(bool)>&& completionHandler)
{
    WEBPAGE_RELEASE_LOG(Loading, "resume: m_page=%p", m_page.get());
    if (!m_page)
        return completionHandler(false);

    auto cachedPage = std::exchange(m_cachedPage, nullptr);
    ASSERT(cachedPage);
    if (!cachedPage)
        return completionHandler(false);

    cachedPage->restore(*m_page);
    unfreezeLayerTree(LayerTreeFreezeReason::PageSuspended);
    completionHandler(true);
}

IntPoint WebPage::screenToRootView(const IntPoint& point)
{
    IntPoint windowPoint;
    sendSync(Messages::WebPageProxy::ScreenToRootView(point), Messages::WebPageProxy::ScreenToRootView::Reply(windowPoint));
    return windowPoint;
}
    
IntRect WebPage::rootViewToScreen(const IntRect& rect)
{
    IntRect screenRect;
    sendSync(Messages::WebPageProxy::RootViewToScreen(rect), Messages::WebPageProxy::RootViewToScreen::Reply(screenRect));
    return screenRect;
}
    
IntPoint WebPage::accessibilityScreenToRootView(const IntPoint& point)
{
    IntPoint windowPoint;
    sendSync(Messages::WebPageProxy::AccessibilityScreenToRootView(point), Messages::WebPageProxy::AccessibilityScreenToRootView::Reply(windowPoint));
    return windowPoint;
}

IntRect WebPage::rootViewToAccessibilityScreen(const IntRect& rect)
{
    IntRect screenRect;
    sendSync(Messages::WebPageProxy::RootViewToAccessibilityScreen(rect), Messages::WebPageProxy::RootViewToAccessibilityScreen::Reply(screenRect));
    return screenRect;
}

KeyboardUIMode WebPage::keyboardUIMode()
{
    bool fullKeyboardAccessEnabled = WebProcess::singleton().fullKeyboardAccessEnabled();
    return static_cast<KeyboardUIMode>((fullKeyboardAccessEnabled ? KeyboardAccessFull : KeyboardAccessDefault) | (m_tabToLinks ? KeyboardAccessTabsToLinks : 0));
}

void WebPage::runJavaScript(WebFrame* frame, RunJavaScriptParameters&& parameters, ContentWorldIdentifier worldIdentifier, CompletionHandler<void(const IPC::DataReference&, const std::optional<WebCore::ExceptionDetails>&)>&& completionHandler)
{
    // NOTE: We need to be careful when running scripts that the objects we depend on don't
    // disappear during script execution.

    if (!frame || !frame->coreFrame()) {
        completionHandler({ }, ExceptionDetails { "Unable to execute JavaScript: Target frame could not be found in the page"_s, 0, 0, ExceptionDetails::Type::InvalidTargetFrame });
        return;
    }

    auto* world = m_userContentController->worldForIdentifier(worldIdentifier);
    if (!world) {
        completionHandler({ }, ExceptionDetails { "Unable to execute JavaScript: Cannot find specified content world"_s });
        return;
    }

#if ENABLE(APP_BOUND_DOMAINS)
    if (frame->shouldEnableInAppBrowserPrivacyProtections()) {
        completionHandler({ }, ExceptionDetails { "Unable to execute JavaScript in a frame that is not in an app-bound domain"_s, 0, 0, ExceptionDetails::Type::AppBoundDomain });
        if (auto* document = m_page->mainFrame().document())
            document->addConsoleMessage(MessageSource::Security, MessageLevel::Warning, "Ignoring user script injection for non-app bound domain.");
        WEBPAGE_RELEASE_LOG_ERROR(Loading, "runJavaScript: Ignoring user script injection for non app-bound domain");
        return;
    }
#endif

    auto resolveFunction = [world = makeRef(*world), frame = makeRef(*frame), completionHandler = WTFMove(completionHandler)] (ValueOrException result) mutable {
        RefPtr<SerializedScriptValue> serializedResultValue;
        if (result) {
            serializedResultValue = SerializedScriptValue::create(frame->jsContextForWorld(world.ptr()),
                toRef(frame->coreFrame()->script().globalObject(world->coreWorld()), result.value()), nullptr);
        }

        IPC::DataReference dataReference;
        if (serializedResultValue)
            dataReference = serializedResultValue->data();

        std::optional<ExceptionDetails> details;
        if (!result)
            details = result.error();

        completionHandler(dataReference, details);
    };
    JSLockHolder lock(commonVM());
    frame->coreFrame()->script().executeAsynchronousUserAgentScriptInWorld(world->coreWorld(), WTFMove(parameters), WTFMove(resolveFunction));
}

void WebPage::runJavaScriptInFrameInScriptWorld(RunJavaScriptParameters&& parameters, std::optional<WebCore::FrameIdentifier> frameID, const std::pair<ContentWorldIdentifier, String>& worldData, CompletionHandler<void(const IPC::DataReference&, const std::optional<WebCore::ExceptionDetails>&)>&& completionHandler)
{
    WEBPAGE_RELEASE_LOG(Process, "runJavaScriptInFrameInScriptWorld: frameID=%" PRIu64, frameID.value_or(WebCore::FrameIdentifier { }).toUInt64());
    auto webFrame = makeRefPtr(frameID ? WebProcess::singleton().webFrame(*frameID) : &mainWebFrame());

    if (auto* newWorld = m_userContentController->addContentWorld(worldData)) {
        auto& coreWorld = newWorld->coreWorld();
        for (RefPtr<Frame> frame = mainFrame(); frame; frame = frame->tree().traverseNext())
            frame->loader().client().dispatchGlobalObjectAvailable(coreWorld);
    }

    runJavaScript(webFrame.get(), WTFMove(parameters), worldData.first, [this, protectedThis = makeRef(*this), completionHandler = WTFMove(completionHandler)](const IPC::DataReference& result, const std::optional<WebCore::ExceptionDetails>& exception) mutable {
        if (exception)
            WEBPAGE_RELEASE_LOG_ERROR(Process, "runJavaScriptInFrameInScriptWorld: Request to run JavaScript failed with error %{private}s", exception->message.utf8().data());
        else
            WEBPAGE_RELEASE_LOG(Process, "runJavaScriptInFrameInScriptWorld: Request to run JavaScript succeeded");
        completionHandler(result, exception);
    });
}

void WebPage::getContentsAsString(ContentAsStringIncludesChildFrames includeChildFrames, CompletionHandler<void(const String&)>&& callback)
{
    switch (includeChildFrames) {
    case ContentAsStringIncludesChildFrames::No:
        callback(m_mainFrame->contentsAsString());
        break;
    case ContentAsStringIncludesChildFrames::Yes:
        StringBuilder builder;
        for (auto frame = makeRefPtr(m_mainFrame->coreFrame()); frame; frame = frame->tree().traverseNextRendered()) {
            if (auto webFrame = WebFrame::fromCoreFrame(*frame))
                builder.append(builder.isEmpty() ? "" : "\n\n", webFrame->contentsAsString());
        }
        callback(builder.toString());
        break;
    }
}

#if ENABLE(MHTML)
void WebPage::getContentsAsMHTMLData(CompletionHandler<void(const IPC::SharedBufferDataReference&)>&& callback)
{
    callback({ MHTMLArchive::generateMHTMLData(m_page.get()) });
}
#endif

void WebPage::getRenderTreeExternalRepresentation(CompletionHandler<void(const String&)>&& callback)
{
    callback(renderTreeExternalRepresentation());
}

static Frame* frameWithSelection(Page* page)
{
    for (Frame* frame = &page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->selection().isRange())
            return frame;
    }
    return nullptr;
}

void WebPage::getSelectionAsWebArchiveData(CompletionHandler<void(const std::optional<IPC::DataReference>&)>&& callback)
{
#if PLATFORM(COCOA)
    RetainPtr<CFDataRef> data;
    if (Frame* frame = frameWithSelection(m_page.get()))
        data = LegacyWebArchive::createFromSelection(frame)->rawDataRepresentation();
#endif

    IPC::DataReference dataReference;
#if PLATFORM(COCOA)
    if (data)
        dataReference = { CFDataGetBytePtr(data.get()), static_cast<size_t>(CFDataGetLength(data.get())) };
#endif
    callback(dataReference);
}

void WebPage::getSelectionOrContentsAsString(CompletionHandler<void(const String&)>&& callback)
{
    WebFrame* focusedOrMainFrame = WebFrame::fromCoreFrame(m_page->focusController().focusedOrMainFrame());
    String resultString = focusedOrMainFrame->selectionAsString();
    if (resultString.isEmpty())
        resultString = focusedOrMainFrame->contentsAsString();
    callback(resultString);
}

void WebPage::getSourceForFrame(FrameIdentifier frameID, CompletionHandler<void(const String&)>&& callback)
{
    String resultString;
    if (WebFrame* frame = WebProcess::singleton().webFrame(frameID))
       resultString = frame->source();

    callback(resultString);
}

void WebPage::getMainResourceDataOfFrame(FrameIdentifier frameID, CompletionHandler<void(const std::optional<IPC::SharedBufferDataReference>&)>&& callback)
{
    RefPtr<SharedBuffer> buffer;
    if (WebFrame* frame = WebProcess::singleton().webFrame(frameID)) {
        if (DocumentLoader* loader = frame->coreFrame()->loader().documentLoader())
            buffer = loader->mainResourceData();
    }

    IPC::SharedBufferDataReference dataReference;
    if (buffer)
        dataReference = { *buffer };
    callback(dataReference);
}

static RefPtr<SharedBuffer> resourceDataForFrame(Frame* frame, const URL& resourceURL)
{
    DocumentLoader* loader = frame->loader().documentLoader();
    if (!loader)
        return nullptr;

    RefPtr<ArchiveResource> subresource = loader->subresource(resourceURL);
    if (!subresource)
        return nullptr;

    return &subresource->data();
}

void WebPage::getResourceDataFromFrame(FrameIdentifier frameID, const String& resourceURLString, CompletionHandler<void(const std::optional<IPC::SharedBufferDataReference>&)>&& callback)
{
    RefPtr<SharedBuffer> buffer;
    if (auto* frame = WebProcess::singleton().webFrame(frameID)) {
        URL resourceURL(URL(), resourceURLString);
        buffer = resourceDataForFrame(frame->coreFrame(), resourceURL);
    }

    IPC::SharedBufferDataReference dataReference;
    if (buffer)
        dataReference = { *buffer };
    callback(dataReference);
}

void WebPage::getWebArchiveOfFrame(FrameIdentifier frameID, CompletionHandler<void(const std::optional<IPC::DataReference>&)>&& callback)
{
#if PLATFORM(COCOA)
    RetainPtr<CFDataRef> data;
    if (WebFrame* frame = WebProcess::singleton().webFrame(frameID))
        data = frame->webArchiveData(nullptr, nullptr);
#else
    UNUSED_PARAM(frameID);
#endif

    IPC::DataReference dataReference;
#if PLATFORM(COCOA)
    if (data)
        dataReference = { CFDataGetBytePtr(data.get()), static_cast<size_t>(CFDataGetLength(data.get())) };
#endif
    callback(dataReference);
}

void WebPage::forceRepaintWithoutCallback()
{
    m_drawingArea->forceRepaint();
}

void WebPage::forceRepaint(CompletionHandler<void()>&& completionHandler)
{
    m_drawingArea->forceRepaintAsync(*this, WTFMove(completionHandler));
}

void WebPage::preferencesDidChange(const WebPreferencesStore& store)
{
    WebPreferencesStore::removeTestRunnerOverrides();
    updatePreferences(store);
}

bool WebPage::isParentProcessAWebBrowser() const
{
#if HAVE(AUDIT_TOKEN)
    return isParentProcessAFullWebBrowser(WebProcess::singleton());
#endif
    return false;
}

void WebPage::updatePreferences(const WebPreferencesStore& store)
{
    updatePreferencesGenerated(store);
    updateSettingsGenerated(store);

    Settings& settings = m_page->settings();

#if !PLATFORM(GTK) && !PLATFORM(WIN)
    if (!settings.acceleratedCompositingEnabled()) {
        WEBPAGE_RELEASE_LOG(Layers, "updatePreferences: acceleratedCompositingEnabled setting was false. WebKit cannot function in this mode; changing setting to true");
        settings.setAcceleratedCompositingEnabled(true);
    }
#endif

    bool requiresUserGestureForMedia = store.getBoolValueForKey(WebPreferencesKey::requiresUserGestureForMediaPlaybackKey());
    settings.setVideoPlaybackRequiresUserGesture(requiresUserGestureForMedia || store.getBoolValueForKey(WebPreferencesKey::requiresUserGestureForVideoPlaybackKey()));
    settings.setAudioPlaybackRequiresUserGesture(requiresUserGestureForMedia || store.getBoolValueForKey(WebPreferencesKey::requiresUserGestureForAudioPlaybackKey()));
    settings.setUserInterfaceDirectionPolicy(static_cast<WebCore::UserInterfaceDirectionPolicy>(store.getUInt32ValueForKey(WebPreferencesKey::userInterfaceDirectionPolicyKey())));
    settings.setSystemLayoutDirection(static_cast<TextDirection>(store.getUInt32ValueForKey(WebPreferencesKey::systemLayoutDirectionKey())));
    settings.setJavaScriptRuntimeFlags(static_cast<RuntimeFlags>(store.getUInt32ValueForKey(WebPreferencesKey::javaScriptRuntimeFlagsKey())));
    settings.setStorageBlockingPolicy(static_cast<StorageBlockingPolicy>(store.getUInt32ValueForKey(WebPreferencesKey::storageBlockingPolicyKey())));
    settings.setFrameFlattening(store.getBoolValueForKey(WebPreferencesKey::frameFlatteningEnabledKey()) ? WebCore::FrameFlattening::FullyEnabled : WebCore::FrameFlattening::Disabled);
    settings.setEditableLinkBehavior(static_cast<WebCore::EditableLinkBehavior>(store.getUInt32ValueForKey(WebPreferencesKey::editableLinkBehaviorKey())));
#if ENABLE(DATA_DETECTION)
    settings.setDataDetectorTypes(static_cast<DataDetectorType>(store.getUInt32ValueForKey(WebPreferencesKey::dataDetectorTypesKey())));
#endif
    settings.setPitchCorrectionAlgorithm(static_cast<MediaPlayerEnums::PitchCorrectionAlgorithm>(store.getUInt32ValueForKey(WebPreferencesKey::pitchCorrectionAlgorithmKey())));

    DatabaseManager::singleton().setIsAvailable(store.getBoolValueForKey(WebPreferencesKey::databasesEnabledKey()));

    m_tabToLinks = store.getBoolValueForKey(WebPreferencesKey::tabsToLinksKey());

    bool isAppNapEnabled = store.getBoolValueForKey(WebPreferencesKey::pageVisibilityBasedProcessSuppressionEnabledKey());
    if (m_isAppNapEnabled != isAppNapEnabled) {
        m_isAppNapEnabled = isAppNapEnabled;
        updateThrottleState();
    }

#if PLATFORM(IOS_FAMILY)
    setForceAlwaysUserScalable(m_forceAlwaysUserScalable || store.getBoolValueForKey(WebPreferencesKey::forceAlwaysUserScalableKey()));
#endif

#if ENABLE(SERVICE_WORKER)
    if (store.getBoolValueForKey(WebPreferencesKey::serviceWorkerEntitlementDisabledForTestingKey()))
        disableServiceWorkerEntitlement();
#if ENABLE(APP_BOUND_DOMAINS)
    bool shouldAllowServiceWorkersForAppBoundViews = m_limitsNavigationsToAppBoundDomains;
#else
    bool shouldAllowServiceWorkersForAppBoundViews = false;
#endif

    if (store.getBoolValueForKey(WebPreferencesKey::serviceWorkersEnabledKey())) {
        ASSERT(parentProcessHasServiceWorkerEntitlement() || shouldAllowServiceWorkersForAppBoundViews);
        if (!parentProcessHasServiceWorkerEntitlement() && !shouldAllowServiceWorkersForAppBoundViews)
            RuntimeEnabledFeatures::sharedFeatures().setServiceWorkerEnabled(false);
    }
#endif

#if ENABLE(APP_BOUND_DOMAINS)
    m_needsInAppBrowserPrivacyQuirks = store.getBoolValueForKey(WebPreferencesKey::needsInAppBrowserPrivacyQuirksKey());
#endif

    settings.setPrivateClickMeasurementEnabled(store.getBoolValueForKey(WebPreferencesKey::privateClickMeasurementEnabledKey()));

    if (m_drawingArea)
        m_drawingArea->updatePreferences(store);

#if ENABLE(GPU_PROCESS)
    // FIXME: useGPUProcessForMediaEnabled should be a RuntimeEnabledFeature since it's global.
    static_cast<WebMediaStrategy&>(platformStrategies()->mediaStrategy()).setUseGPUProcess(m_shouldPlayMediaInGPUProcess);
    WebProcess::singleton().supplement<RemoteMediaPlayerManager>()->setUseGPUProcess(m_shouldPlayMediaInGPUProcess);
#if HAVE(AVASSETREADER)
    WebProcess::singleton().supplement<RemoteImageDecoderAVFManager>()->setUseGPUProcess(m_shouldPlayMediaInGPUProcess);
#endif
    WebProcess::singleton().setUseGPUProcessForCanvasRendering(m_shouldRenderCanvasInGPUProcess);
    bool usingGPUProcessForDOMRendering = m_shouldRenderDOMInGPUProcess && DrawingArea::supportsGPUProcessRendering(m_drawingAreaType);
    WebProcess::singleton().setUseGPUProcessForDOMRendering(usingGPUProcessForDOMRendering);
    WebProcess::singleton().setUseGPUProcessForMedia(m_shouldPlayMediaInGPUProcess);
#endif // ENABLE(GPU_PROCESS)

#if ENABLE(IPC_TESTING_API)
    m_ipcTestingAPIEnabled = store.getBoolValueForKey(WebPreferencesKey::ipcTestingAPIEnabledKey());

    WebProcess::singleton().parentProcessConnection()->setIgnoreInvalidMessageForTesting();
    if (auto* gpuProcessConnection = WebProcess::singleton().existingGPUProcessConnection())
        gpuProcessConnection->connection().setIgnoreInvalidMessageForTesting();
#endif

#if ENABLE(WEB_AUTHN) && PLATFORM(IOS)
    if (isParentProcessAWebBrowser())
        settings.setWebAuthenticationEnabled(true);
#endif

#if ENABLE(WEBM_FORMAT_READER)
    PlatformMediaSessionManager::setWebMFormatReaderEnabled(RuntimeEnabledFeatures::sharedFeatures().webMFormatReaderEnabled());
#endif

#if ENABLE(VORBIS) && PLATFORM(MAC)
    PlatformMediaSessionManager::setVorbisDecoderEnabled(RuntimeEnabledFeatures::sharedFeatures().vorbisDecoderEnabled());
#endif

#if ENABLE(OPUS) && PLATFORM(MAC)
    PlatformMediaSessionManager::setOpusDecoderEnabled(RuntimeEnabledFeatures::sharedFeatures().opusDecoderEnabled());
#endif
}

#if ENABLE(DATA_DETECTION)

void WebPage::setDataDetectionResults(NSArray *detectionResults)
{
    DataDetectionResult dataDetectionResult;
    dataDetectionResult.results = detectionResults;
    send(Messages::WebPageProxy::SetDataDetectionResult(dataDetectionResult));
}

void WebPage::removeDataDetectedLinks(CompletionHandler<void(const DataDetectionResult&)>&& completionHandler)
{
    for (auto frame = makeRefPtr(&m_page->mainFrame()); frame; frame = frame->tree().traverseNext()) {
        auto document = makeRefPtr(frame->document());
        if (!document)
            continue;

        DataDetection::removeDataDetectedLinksInDocument(*document);

        if (auto* results = frame->dataDetectionResultsIfExists()) {
            // FIXME: It seems odd that we're clearing out all data detection results here,
            // instead of only data detectors that correspond to links.
            results->setDocumentLevelResults({ });
        }
    }
    completionHandler({ });
}

void WebPage::detectDataInAllFrames(OptionSet<WebCore::DataDetectorType> dataDetectorTypes, CompletionHandler<void(const DataDetectionResult&)>&& completionHandler)
{
    DataDetectionResult mainFrameResult;
    for (auto frame = makeRefPtr(&m_page->mainFrame()); frame; frame = frame->tree().traverseNext()) {
        auto document = makeRefPtr(frame->document());
        if (!document)
            continue;
        auto results = retainPtr(DataDetection::detectContentInRange(makeRangeSelectingNodeContents(*document), dataDetectorTypes, m_dataDetectionContext.get()));
        frame->dataDetectionResults().setDocumentLevelResults(results.get());
        if (frame->isMainFrame())
            mainFrameResult.results = WTFMove(results);
    }
    completionHandler(WTFMove(mainFrameResult));
}

std::optional<std::pair<Ref<WebCore::HTMLElement>, WebCore::IntRect>> WebPage::findDataDetectionResultElementInImageOverlay(const FloatPoint& location, const HTMLElement& imageOverlayHost)
{
    Vector<Ref<HTMLElement>> dataDetectorElements;
    for (auto& child : descendantsOfType<HTMLElement>(*imageOverlayHost.shadowRoot())) {
        if (child.isImageOverlayDataDetectorResult())
            dataDetectorElements.append(child);
    }

    for (auto& element : dataDetectorElements) {
        auto elementBounds = element->boundsInRootViewSpace();
        if (elementBounds.contains(roundedIntPoint(location)))
            return {{ WTFMove(element), elementBounds }};
    }

    return std::nullopt;
}

#endif // ENABLE(DATA_DETECTION)

#if PLATFORM(COCOA)
void WebPage::willCommitLayerTree(RemoteLayerTreeTransaction& layerTransaction)
{
    FrameView* frameView = corePage()->mainFrame().view();
    if (!frameView)
        return;

    layerTransaction.setContentsSize(frameView->contentsSize());
    layerTransaction.setScrollOrigin(frameView->scrollOrigin());
    layerTransaction.setPageScaleFactor(corePage()->pageScaleFactor());
    layerTransaction.setRenderTreeSize(corePage()->renderTreeSize());
    layerTransaction.setThemeColor(corePage()->themeColor());
    layerTransaction.setPageExtendedBackgroundColor(corePage()->pageExtendedBackgroundColor());
    layerTransaction.setSampledPageTopColor(corePage()->sampledPageTopColor());

    layerTransaction.setBaseLayoutViewportSize(frameView->baseLayoutViewportSize());
    layerTransaction.setMinStableLayoutViewportOrigin(frameView->minStableLayoutViewportOrigin());
    layerTransaction.setMaxStableLayoutViewportOrigin(frameView->maxStableLayoutViewportOrigin());

#if PLATFORM(IOS_FAMILY)
    layerTransaction.setScaleWasSetByUIProcess(scaleWasSetByUIProcess());
    layerTransaction.setMinimumScaleFactor(m_viewportConfiguration.minimumScale());
    layerTransaction.setMaximumScaleFactor(m_viewportConfiguration.maximumScale());
    layerTransaction.setInitialScaleFactor(m_viewportConfiguration.initialScale());
    layerTransaction.setViewportMetaTagWidth(m_viewportConfiguration.viewportArguments().width);
    layerTransaction.setViewportMetaTagWidthWasExplicit(m_viewportConfiguration.viewportArguments().widthWasExplicit);
    layerTransaction.setViewportMetaTagCameFromImageDocument(m_viewportConfiguration.viewportArguments().type == ViewportArguments::ImageDocument);
    layerTransaction.setAvoidsUnsafeArea(m_viewportConfiguration.avoidsUnsafeArea());
    layerTransaction.setIsInStableState(m_isInStableState);
    layerTransaction.setAllowsUserScaling(allowsUserScaling());
    if (m_pendingDynamicViewportSizeUpdateID) {
        layerTransaction.setDynamicViewportSizeUpdateID(*m_pendingDynamicViewportSizeUpdateID);
        m_pendingDynamicViewportSizeUpdateID = std::nullopt;
    }
    if (m_lastTransactionPageScaleFactor != layerTransaction.pageScaleFactor()) {
        m_lastTransactionPageScaleFactor = layerTransaction.pageScaleFactor();
        m_lastTransactionIDWithScaleChange = layerTransaction.transactionID();
    }
#endif

    layerTransaction.setScrollPosition(frameView->scrollPosition());

    m_pendingThemeColorChange = false;
    m_pendingPageExtendedBackgroundColorChange = false;
    m_pendingSampledPageTopColorChange = false;

    if (hasPendingEditorStateUpdate()) {
        layerTransaction.setEditorState(editorState());
        m_pendingEditorStateUpdateStatus = PendingEditorStateUpdateStatus::NotScheduled;
    }
}

void WebPage::didFlushLayerTreeAtTime(MonotonicTime timestamp)
{
#if PLATFORM(IOS_FAMILY)
    if (m_oldestNonStableUpdateVisibleContentRectsTimestamp != MonotonicTime()) {
        Seconds elapsed = timestamp - m_oldestNonStableUpdateVisibleContentRectsTimestamp;
        m_oldestNonStableUpdateVisibleContentRectsTimestamp = MonotonicTime();

        m_estimatedLatency = m_estimatedLatency * 0.80 + elapsed * 0.20;
    }
#else
    UNUSED_PARAM(timestamp);
#endif
}
#endif

void WebPage::layoutIfNeeded()
{
    m_page->layoutIfNeeded();
}
    
void WebPage::updateRendering()
{
    m_page->updateRendering();
}

void WebPage::didUpdateRendering()
{
    if (m_didUpdateRenderingAfterCommittingLoad)
        return;

    m_didUpdateRenderingAfterCommittingLoad = true;
    send(Messages::WebPageProxy::DidUpdateRenderingAfterCommittingLoad());
}

bool WebPage::shouldTriggerRenderingUpdate(unsigned rescheduledRenderingUpdateCount) const
{
#if ENABLE(GPU_PROCESS)
    static constexpr unsigned maxRescheduledRenderingUpdateCount = FullSpeedFramesPerSecond;
    if (rescheduledRenderingUpdateCount >= maxRescheduledRenderingUpdateCount)
        return true;

    static constexpr unsigned maxDelayedRenderingUpdateCount = 2;
    if (m_remoteRenderingBackendProxy && m_remoteRenderingBackendProxy->delayedRenderingUpdateCount() > maxDelayedRenderingUpdateCount)
        return false;
#endif
    return true;
}

void WebPage::finalizeRenderingUpdate(OptionSet<FinalizeRenderingUpdateFlags> flags)
{
    m_page->finalizeRenderingUpdate(flags);
#if ENABLE(GPU_PROCESS)
    if (m_remoteRenderingBackendProxy)
        m_remoteRenderingBackendProxy->finalizeRenderingUpdate();
#endif
}

void WebPage::releaseMemory(Critical)
{
#if ENABLE(GPU_PROCESS)
    if (m_remoteRenderingBackendProxy)
        m_remoteRenderingBackendProxy->remoteResourceCacheProxy().releaseMemory();
#endif
}

WebInspector* WebPage::inspector(LazyCreationPolicy behavior)
{
    if (m_isClosed)
        return nullptr;
    if (!m_inspector && behavior == LazyCreationPolicy::CreateIfNeeded)
        m_inspector = WebInspector::create(this);
    return m_inspector.get();
}

WebInspectorUI* WebPage::inspectorUI()
{
    if (m_isClosed)
        return nullptr;
    if (!m_inspectorUI)
        m_inspectorUI = WebInspectorUI::create(*this);
    return m_inspectorUI.get();
}

RemoteWebInspectorUI* WebPage::remoteInspectorUI()
{
    if (m_isClosed)
        return nullptr;
    if (!m_remoteInspectorUI)
        m_remoteInspectorUI = RemoteWebInspectorUI::create(*this);
    return m_remoteInspectorUI.get();
}

void WebPage::inspectorFrontendCountChanged(unsigned count)
{
    send(Messages::WebPageProxy::DidChangeInspectorFrontendCount(count));
}

#if PLATFORM(IOS_FAMILY)
void WebPage::setAllowsMediaDocumentInlinePlayback(bool allows)
{
    m_page->setAllowsMediaDocumentInlinePlayback(allows);
}
#endif

void WebPage::addConsoleMessage(FrameIdentifier frameID, MessageSource messageSource, MessageLevel messageLevel, const String& message, uint64_t requestID)
{
    if (auto* frame = WebProcess::singleton().webFrame(frameID))
        frame->addConsoleMessage(messageSource, messageLevel, message, requestID);
}

void WebPage::sendCSPViolationReport(FrameIdentifier frameID, const URL& reportURL, IPC::FormDataReference&& reportData)
{
    auto report = reportData.takeData();
    if (!report)
        return;
    if (auto* frame = WebProcess::singleton().webFrame(frameID))
        PingLoader::sendViolationReport(*frame->coreFrame(), reportURL, report.releaseNonNull(), ViolationReportType::ContentSecurityPolicy);
}

void WebPage::enqueueSecurityPolicyViolationEvent(FrameIdentifier frameID, SecurityPolicyViolationEvent::Init&& eventInit)
{
    auto* frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;
    auto* coreFrame = frame->coreFrame();
    if (!coreFrame)
        return;
    if (auto* document = coreFrame->document())
        document->enqueueSecurityPolicyViolationEvent(WTFMove(eventInit));
}

NotificationPermissionRequestManager* WebPage::notificationPermissionRequestManager()
{
    if (m_notificationPermissionRequestManager)
        return m_notificationPermissionRequestManager.get();

    m_notificationPermissionRequestManager = NotificationPermissionRequestManager::create(this);
    return m_notificationPermissionRequestManager.get();
}

#if ENABLE(DRAG_SUPPORT)

#if PLATFORM(GTK)
void WebPage::performDragControllerAction(DragControllerAction action, const IntPoint& clientPosition, const IntPoint& globalPosition, OptionSet<DragOperation> draggingSourceOperationMask, SelectionData&& selectionData, OptionSet<DragApplicationFlags> flags)
{
    if (!m_page) {
        send(Messages::WebPageProxy::DidPerformDragControllerAction(std::nullopt, DragHandlingMethod::None, false, 0, { }, { }));
        return;
    }

    DragData dragData(&selectionData, clientPosition, globalPosition, draggingSourceOperationMask, flags, anyDragDestinationAction(), m_identifier);
    switch (action) {
    case DragControllerAction::Entered: {
        auto resolvedDragOperation = m_page->dragController().dragEntered(dragData);
        send(Messages::WebPageProxy::DidPerformDragControllerAction(resolvedDragOperation, m_page->dragController().dragHandlingMethod(), m_page->dragController().mouseIsOverFileInput(), m_page->dragController().numberOfItemsToBeAccepted(), { }, { }));
        return;
    }
    case DragControllerAction::Updated: {
        auto resolvedDragOperation = m_page->dragController().dragUpdated(dragData);
        send(Messages::WebPageProxy::DidPerformDragControllerAction(resolvedDragOperation, m_page->dragController().dragHandlingMethod(), m_page->dragController().mouseIsOverFileInput(), m_page->dragController().numberOfItemsToBeAccepted(), { }, { }));
        return;
    }
    case DragControllerAction::Exited:
        m_page->dragController().dragExited(dragData);
        return;

    case DragControllerAction::PerformDragOperation: {
        m_page->dragController().performDragOperation(dragData);
        return;
    }
    }
    ASSERT_NOT_REACHED();
}
#else
void WebPage::performDragControllerAction(DragControllerAction action, const WebCore::DragData& dragData, SandboxExtension::Handle&& sandboxExtensionHandle, Vector<SandboxExtension::Handle>&& sandboxExtensionsHandleArray)
{
    if (!m_page) {
        send(Messages::WebPageProxy::DidPerformDragControllerAction(std::nullopt, DragHandlingMethod::None, false, 0, { }, { }));
        return;
    }

    switch (action) {
    case DragControllerAction::Entered: {
        auto resolvedDragOperation = m_page->dragController().dragEntered(dragData);
        send(Messages::WebPageProxy::DidPerformDragControllerAction(resolvedDragOperation, m_page->dragController().dragHandlingMethod(), m_page->dragController().mouseIsOverFileInput(), m_page->dragController().numberOfItemsToBeAccepted(), m_page->dragCaretController().caretRectInRootViewCoordinates(), m_page->dragCaretController().editableElementRectInRootViewCoordinates()));
        return;
    }
    case DragControllerAction::Updated: {
        auto resolvedDragOperation = m_page->dragController().dragUpdated(dragData);
        send(Messages::WebPageProxy::DidPerformDragControllerAction(resolvedDragOperation, m_page->dragController().dragHandlingMethod(), m_page->dragController().mouseIsOverFileInput(), m_page->dragController().numberOfItemsToBeAccepted(), m_page->dragCaretController().caretRectInRootViewCoordinates(), m_page->dragCaretController().editableElementRectInRootViewCoordinates()));
        return;
    }
    case DragControllerAction::Exited:
        m_page->dragController().dragExited(dragData);
        send(Messages::WebPageProxy::DidPerformDragControllerAction(std::nullopt, DragHandlingMethod::None, false, 0, { }, { }));
        return;
        
    case DragControllerAction::PerformDragOperation: {
        ASSERT(!m_pendingDropSandboxExtension);

        m_pendingDropSandboxExtension = SandboxExtension::create(WTFMove(sandboxExtensionHandle));
        for (size_t i = 0; i < sandboxExtensionsHandleArray.size(); i++) {
            if (auto extension = SandboxExtension::create(WTFMove(sandboxExtensionsHandleArray[i])))
                m_pendingDropExtensionsForFileUpload.append(extension);
        }

        bool handled = m_page->dragController().performDragOperation(dragData);

        // If we started loading a local file, the sandbox extension tracker would have adopted this
        // pending drop sandbox extension. If not, we'll play it safe and clear it.
        m_pendingDropSandboxExtension = nullptr;

        m_pendingDropExtensionsForFileUpload.clear();
        send(Messages::WebPageProxy::DidPerformDragOperation(handled));
        return;
    }
    }
    ASSERT_NOT_REACHED();
}
#endif

void WebPage::dragEnded(WebCore::IntPoint clientPosition, WebCore::IntPoint globalPosition, OptionSet<WebCore::DragOperation> dragOperationMask)
{
    IntPoint adjustedClientPosition(clientPosition.x() + m_page->dragController().dragOffset().x(), clientPosition.y() + m_page->dragController().dragOffset().y());
    IntPoint adjustedGlobalPosition(globalPosition.x() + m_page->dragController().dragOffset().x(), globalPosition.y() + m_page->dragController().dragOffset().y());

    m_page->dragController().dragEnded();
    FrameView* view = m_page->mainFrame().view();
    if (!view)
        return;
    // FIXME: These are fake modifier keys here, but they should be real ones instead.
    PlatformMouseEvent event(adjustedClientPosition, adjustedGlobalPosition, LeftButton, PlatformEvent::MouseMoved, 0, false, false, false, false, WallTime::now(), 0, WebCore::NoTap);
    m_page->mainFrame().eventHandler().dragSourceEndedAt(event, dragOperationMask);

    send(Messages::WebPageProxy::DidEndDragging());

    m_isStartingDrag = false;
}

void WebPage::willPerformLoadDragDestinationAction()
{
    m_sandboxExtensionTracker.willPerformLoadDragDestinationAction(WTFMove(m_pendingDropSandboxExtension));
}

void WebPage::mayPerformUploadDragDestinationAction()
{
    for (size_t i = 0; i < m_pendingDropExtensionsForFileUpload.size(); i++)
        m_pendingDropExtensionsForFileUpload[i]->consumePermanently();
    m_pendingDropExtensionsForFileUpload.clear();
}

void WebPage::didStartDrag()
{
    m_isStartingDrag = false;
    m_page->mainFrame().eventHandler().didStartDrag();
}

void WebPage::dragCancelled()
{
    m_isStartingDrag = false;
    m_page->mainFrame().eventHandler().dragCancelled();
}

#endif // ENABLE(DRAG_SUPPORT)

WebUndoStep* WebPage::webUndoStep(WebUndoStepID stepID)
{
    return m_undoStepMap.get(stepID);
}

void WebPage::addWebUndoStep(WebUndoStepID stepID, Ref<WebUndoStep>&& entry)
{
    auto addResult = m_undoStepMap.set(stepID, WTFMove(entry));
    ASSERT_UNUSED(addResult, addResult.isNewEntry);
}

void WebPage::removeWebEditCommand(WebUndoStepID stepID)
{
    if (auto undoStep = m_undoStepMap.take(stepID))
        undoStep->didRemoveFromUndoManager();
}

void WebPage::unapplyEditCommand(WebUndoStepID stepID)
{
    auto* step = webUndoStep(stepID);
    if (!step)
        return;

    step->step().unapply();
}

void WebPage::reapplyEditCommand(WebUndoStepID stepID)
{
    auto* step = webUndoStep(stepID);
    if (!step)
        return;

    m_isInRedo = true;
    step->step().reapply();
    m_isInRedo = false;
}

void WebPage::didRemoveEditCommand(WebUndoStepID commandID)
{
    removeWebEditCommand(commandID);
}

void WebPage::setActivePopupMenu(WebPopupMenu* menu)
{
    m_activePopupMenu = menu;
}

#if ENABLE(INPUT_TYPE_COLOR)

void WebPage::setActiveColorChooser(WebColorChooser* colorChooser)
{
    m_activeColorChooser = colorChooser;
}

void WebPage::didEndColorPicker()
{
    if (m_activeColorChooser)
        m_activeColorChooser->didEndChooser();
}

void WebPage::didChooseColor(const WebCore::Color& color)
{
    if (m_activeColorChooser)
        m_activeColorChooser->didChooseColor(color);
}

#endif

#if ENABLE(DATALIST_ELEMENT)

void WebPage::setActiveDataListSuggestionPicker(WebDataListSuggestionPicker& dataListSuggestionPicker)
{
    m_activeDataListSuggestionPicker = makeWeakPtr(&dataListSuggestionPicker);
}

void WebPage::didSelectDataListOption(const String& selectedOption)
{
    if (m_activeDataListSuggestionPicker)
        m_activeDataListSuggestionPicker->didSelectOption(selectedOption);
}

void WebPage::didCloseSuggestions()
{
    if (auto picker = std::exchange(m_activeDataListSuggestionPicker, nullptr))
        picker->didCloseSuggestions();
}

#endif

#if ENABLE(DATE_AND_TIME_INPUT_TYPES)

void WebPage::setActiveDateTimeChooser(WebDateTimeChooser& dateTimeChooser)
{
    m_activeDateTimeChooser = makeWeakPtr(&dateTimeChooser);
}

void WebPage::didChooseDate(const String& date)
{
    if (m_activeDateTimeChooser)
        m_activeDateTimeChooser->didChooseDate(date);
}

void WebPage::didEndDateTimePicker()
{
    if (auto chooser = std::exchange(m_activeDateTimeChooser, nullptr))
        chooser->didEndChooser();
}

#endif

void WebPage::setActiveOpenPanelResultListener(Ref<WebOpenPanelResultListener>&& openPanelResultListener)
{
    m_activeOpenPanelResultListener = WTFMove(openPanelResultListener);
}

bool WebPage::findStringFromInjectedBundle(const String& target, OptionSet<FindOptions> options)
{
    return m_page->findString(target, core(options));
}

void WebPage::findStringMatchesFromInjectedBundle(const String& target, OptionSet<FindOptions> options)
{
    findController().findStringMatches(target, options, 0);
}

void WebPage::replaceStringMatchesFromInjectedBundle(const Vector<uint32_t>& matchIndices, const String& replacementText, bool selectionOnly)
{
    findController().replaceMatches(matchIndices, replacementText, selectionOnly);
}

void WebPage::findString(const String& string, OptionSet<FindOptions> options, uint32_t maxMatchCount, CompletionHandler<void(bool)>&& completionHandler)
{
    findController().findString(string, options, maxMatchCount, WTFMove(completionHandler));
}

void WebPage::findStringMatches(const String& string, OptionSet<FindOptions> options, uint32_t maxMatchCount)
{
    findController().findStringMatches(string, options, maxMatchCount);
}

void WebPage::getImageForFindMatch(uint32_t matchIndex)
{
    findController().getImageForFindMatch(matchIndex);
}

void WebPage::selectFindMatch(uint32_t matchIndex)
{
    findController().selectFindMatch(matchIndex);
}

void WebPage::indicateFindMatch(uint32_t matchIndex)
{
    findController().indicateFindMatch(matchIndex);
}

void WebPage::hideFindUI()
{
    findController().hideFindUI();
}

void WebPage::countStringMatches(const String& string, OptionSet<FindOptions> options, uint32_t maxMatchCount)
{
    findController().countStringMatches(string, options, maxMatchCount);
}

void WebPage::replaceMatches(const Vector<uint32_t>& matchIndices, const String& replacementText, bool selectionOnly, CompletionHandler<void(uint64_t)>&& completionHandler)
{
    auto numberOfReplacements = findController().replaceMatches(matchIndices, replacementText, selectionOnly);
    completionHandler(numberOfReplacements);
}

void WebPage::didChangeSelectedIndexForActivePopupMenu(int32_t newIndex)
{
    changeSelectedIndex(newIndex);
    m_activePopupMenu = nullptr;
}

void WebPage::changeSelectedIndex(int32_t index)
{
    if (!m_activePopupMenu)
        return;

    m_activePopupMenu->didChangeSelectedIndex(index);
}

#if PLATFORM(IOS_FAMILY)
void WebPage::didChooseFilesForOpenPanelWithDisplayStringAndIcon(const Vector<String>& files, const String& displayString, const IPC::DataReference& iconData, SandboxExtension::Handle&& frontboardServicesSandboxExtensionHandle, SandboxExtension::Handle&& iconServicesSandboxExtensionHandle)
{
    if (!m_activeOpenPanelResultListener)
        return;

#if HAVE(FRONTBOARD_SYSTEM_APP_SERVICES)
    auto frontboardServicesSandboxExtension = SandboxExtension::create(WTFMove(frontboardServicesSandboxExtensionHandle));
    if (frontboardServicesSandboxExtension) {
        bool consumed = frontboardServicesSandboxExtension->consume();
        ASSERT_UNUSED(consumed, consumed);
    }
    RELEASE_ASSERT(!sandbox_check(getpid(), "mach-lookup", static_cast<enum sandbox_filter_type>(SANDBOX_FILTER_GLOBAL_NAME | SANDBOX_CHECK_NO_REPORT), "com.apple.frontboard.systemappservices"));
#endif

    auto iconServicesSandboxExtension = SandboxExtension::create(WTFMove(iconServicesSandboxExtensionHandle));
    if (iconServicesSandboxExtension) {
        bool consumed = iconServicesSandboxExtension->consume();
        ASSERT_UNUSED(consumed, consumed);
    }
    RELEASE_ASSERT(!sandbox_check(getpid(), "mach-lookup", static_cast<enum sandbox_filter_type>(SANDBOX_FILTER_GLOBAL_NAME | SANDBOX_CHECK_NO_REPORT), "com.apple.iconservices"));

    RefPtr<Icon> icon;
    if (!iconData.isEmpty()) {
        RetainPtr<CFDataRef> dataRef = adoptCF(CFDataCreate(nullptr, iconData.data(), iconData.size()));
        RetainPtr<CGDataProviderRef> imageProviderRef = adoptCF(CGDataProviderCreateWithCFData(dataRef.get()));
        RetainPtr<CGImageRef> imageRef = adoptCF(CGImageCreateWithJPEGDataProvider(imageProviderRef.get(), nullptr, true, kCGRenderingIntentDefault));
        icon = Icon::createIconForImage(WTFMove(imageRef));
    }

    m_activeOpenPanelResultListener->didChooseFilesWithDisplayStringAndIcon(files, displayString, icon.get());
    m_activeOpenPanelResultListener = nullptr;

#if HAVE(FRONTBOARD_SYSTEM_APP_SERVICES)
    if (frontboardServicesSandboxExtension) {
        bool revoked = frontboardServicesSandboxExtension->revoke();
        ASSERT_UNUSED(revoked, revoked);
    }
#endif

    if (iconServicesSandboxExtension) {
        bool revoked = iconServicesSandboxExtension->revoke();
        ASSERT_UNUSED(revoked, revoked);
    }

}
#endif

void WebPage::didChooseFilesForOpenPanel(const Vector<String>& files, const Vector<String>& replacementFiles)
{
    if (!m_activeOpenPanelResultListener)
        return;

    m_activeOpenPanelResultListener->didChooseFiles(files, replacementFiles);
    m_activeOpenPanelResultListener = nullptr;
}

void WebPage::didCancelForOpenPanel()
{
    m_activeOpenPanelResultListener = nullptr;
}

#if ENABLE(SANDBOX_EXTENSIONS)
void WebPage::extendSandboxForFilesFromOpenPanel(Vector<SandboxExtension::Handle>&& handles)
{
    bool result = SandboxExtension::consumePermanently(handles);
    if (!result) {
        // We have reports of cases where this fails for some unknown reason, <rdar://problem/10156710>.
        WTFLogAlways("WebPage::extendSandboxForFileFromOpenPanel(): Could not consume a sandbox extension");
    }
}
#endif

#if !PLATFORM(IOS_FAMILY)
void WebPage::advanceToNextMisspelling(bool startBeforeSelection)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.editor().advanceToNextMisspelling(startBeforeSelection);
}
#endif

bool WebPage::hasRichlyEditableSelection() const
{
    auto& frame = m_page->focusController().focusedOrMainFrame();
    if (m_page->dragCaretController().isContentRichlyEditable())
        return true;

    return frame.selection().selection().isContentRichlyEditable();
}

void WebPage::changeSpellingToWord(const String& word)
{
    replaceSelectionWithText(&m_page->focusController().focusedOrMainFrame(), word);
}

void WebPage::unmarkAllMisspellings()
{
    for (Frame* frame = &m_page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (Document* document = frame->document())
            document->markers().removeMarkers(DocumentMarker::Spelling);
    }
}

void WebPage::unmarkAllBadGrammar()
{
    for (Frame* frame = &m_page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (Document* document = frame->document())
            document->markers().removeMarkers(DocumentMarker::Grammar);
    }
}

#if USE(APPKIT)
void WebPage::uppercaseWord()
{
    m_page->focusController().focusedOrMainFrame().editor().uppercaseWord();
}

void WebPage::lowercaseWord()
{
    m_page->focusController().focusedOrMainFrame().editor().lowercaseWord();
}

void WebPage::capitalizeWord()
{
    m_page->focusController().focusedOrMainFrame().editor().capitalizeWord();
}
#endif
    
void WebPage::setTextForActivePopupMenu(int32_t index)
{
    if (!m_activePopupMenu)
        return;

    m_activePopupMenu->setTextForIndex(index);
}

#if PLATFORM(GTK)
void WebPage::failedToShowPopupMenu()
{
    if (!m_activePopupMenu)
        return;

    m_activePopupMenu->client()->popupDidHide();
}
#endif

#if ENABLE(CONTEXT_MENUS)
void WebPage::didSelectItemFromActiveContextMenu(const WebContextMenuItemData& item)
{
    if (auto contextMenu = std::exchange(m_contextMenu, nullptr))
        contextMenu->itemSelected(item);
}
#endif

void WebPage::replaceSelectionWithText(Frame* frame, const String& text)
{
    return frame->editor().replaceSelectionWithText(text, WebCore::Editor::SelectReplacement::Yes, WebCore::Editor::SmartReplace::No);
}

#if !PLATFORM(IOS_FAMILY)
void WebPage::clearSelection()
{
    m_page->focusController().focusedOrMainFrame().selection().clear();
}
#endif

void WebPage::restoreSelectionInFocusedEditableElement()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.selection().isNone())
        return;

    if (auto document = frame.document()) {
        if (auto element = document->focusedElement())
            element->updateFocusAppearance(SelectionRestorationMode::RestoreOrSelectAll, SelectionRevealMode::DoNotReveal);
    }
}

bool WebPage::mainFrameHasCustomContentProvider() const
{
    if (Frame* frame = mainFrame()) {
        WebFrameLoaderClient* webFrameLoaderClient = toWebFrameLoaderClient(frame->loader().client());
        ASSERT(webFrameLoaderClient);
        return webFrameLoaderClient->frameHasCustomContentProvider();
    }

    return false;
}

void WebPage::addMIMETypeWithCustomContentProvider(const String& mimeType)
{
    m_mimeTypesWithCustomContentProviders.add(mimeType);
}

void WebPage::updateMainFrameScrollOffsetPinning()
{
    Frame& frame = m_page->mainFrame();
    ScrollPosition scrollPosition = frame.view()->scrollPosition();
    ScrollPosition maximumScrollPosition = frame.view()->maximumScrollPosition();
    ScrollPosition minimumScrollPosition = frame.view()->minimumScrollPosition();

    bool isPinnedToLeftSide = (scrollPosition.x() <= minimumScrollPosition.x());
    bool isPinnedToRightSide = (scrollPosition.x() >= maximumScrollPosition.x());
    bool isPinnedToTopSide = (scrollPosition.y() <= minimumScrollPosition.y());
    bool isPinnedToBottomSide = (scrollPosition.y() >= maximumScrollPosition.y());

    if (isPinnedToLeftSide != m_cachedMainFrameIsPinnedToLeftSide || isPinnedToRightSide != m_cachedMainFrameIsPinnedToRightSide || isPinnedToTopSide != m_cachedMainFrameIsPinnedToTopSide || isPinnedToBottomSide != m_cachedMainFrameIsPinnedToBottomSide) {
        send(Messages::WebPageProxy::DidChangeScrollOffsetPinningForMainFrame(isPinnedToLeftSide, isPinnedToRightSide, isPinnedToTopSide, isPinnedToBottomSide));
        
        m_cachedMainFrameIsPinnedToLeftSide = isPinnedToLeftSide;
        m_cachedMainFrameIsPinnedToRightSide = isPinnedToRightSide;
        m_cachedMainFrameIsPinnedToTopSide = isPinnedToTopSide;
        m_cachedMainFrameIsPinnedToBottomSide = isPinnedToBottomSide;
    }
}

void WebPage::mainFrameDidLayout()
{
    unsigned pageCount = m_page->pageCount();
    if (pageCount != m_cachedPageCount) {
        send(Messages::WebPageProxy::DidChangePageCount(pageCount));
        m_cachedPageCount = pageCount;
    }

#if PLATFORM(COCOA) || PLATFORM(GTK)
    if (m_viewGestureGeometryCollector)
        m_viewGestureGeometryCollector->mainFrameDidLayout();
#endif
#if PLATFORM(IOS_FAMILY)
    if (FrameView* frameView = mainFrameView()) {
        IntSize newContentSize = frameView->contentsSize();
        LOG_WITH_STREAM(VisibleRects, stream << "WebPage " << m_identifier.toUInt64() << " mainFrameDidLayout setting content size to " << newContentSize);
        if (m_viewportConfiguration.setContentsSize(newContentSize))
            viewportConfigurationChanged();
    }
    findController().redraw();
#endif
}

void WebPage::sendSetWindowFrame(const FloatRect& windowFrame)
{
#if PLATFORM(COCOA)
    m_hasCachedWindowFrame = false;
#endif
    send(Messages::WebPageProxy::SetWindowFrame(windowFrame));
}

#if PLATFORM(COCOA)
void WebPage::windowAndViewFramesChanged(const FloatRect& windowFrameInScreenCoordinates, const FloatRect& windowFrameInUnflippedScreenCoordinates, const FloatRect& viewFrameInWindowCoordinates, const FloatPoint& accessibilityViewCoordinates)
{
    m_windowFrameInScreenCoordinates = windowFrameInScreenCoordinates;
    m_windowFrameInUnflippedScreenCoordinates = windowFrameInUnflippedScreenCoordinates;
    m_viewFrameInWindowCoordinates = viewFrameInWindowCoordinates;
    m_accessibilityPosition = accessibilityViewCoordinates;

    m_hasCachedWindowFrame = !m_windowFrameInUnflippedScreenCoordinates.isEmpty();
}
#endif

void WebPage::setMainFrameIsScrollable(bool isScrollable)
{
    m_mainFrameIsScrollable = isScrollable;
    m_drawingArea->mainFrameScrollabilityChanged(isScrollable);

    if (FrameView* frameView = m_mainFrame->coreFrame()->view()) {
        frameView->setCanHaveScrollbars(isScrollable);
        frameView->setProhibitsScrolling(!isScrollable);
    }
}

bool WebPage::windowIsFocused() const
{
    return m_page->focusController().isActive();
}

bool WebPage::windowAndWebPageAreFocused() const
{
    if (!isVisible())
        return false;

    return m_page->focusController().isFocused() && m_page->focusController().isActive();
}

void WebPage::didReceiveMessage(IPC::Connection& connection, IPC::Decoder& decoder)
{
    if (decoder.messageReceiverName() == Messages::WebInspector::messageReceiverName()) {
        if (WebInspector* inspector = this->inspector())
            inspector->didReceiveMessage(connection, decoder);
        return;
    }

    if (decoder.messageReceiverName() == Messages::WebInspectorUI::messageReceiverName()) {
        if (WebInspectorUI* inspectorUI = this->inspectorUI())
            inspectorUI->didReceiveMessage(connection, decoder);
        return;
    }

    if (decoder.messageReceiverName() == Messages::RemoteWebInspectorUI::messageReceiverName()) {
        if (RemoteWebInspectorUI* remoteInspectorUI = this->remoteInspectorUI())
            remoteInspectorUI->didReceiveMessage(connection, decoder);
        return;
    }

    didReceiveWebPageMessage(connection, decoder);
}

bool WebPage::didReceiveSyncMessage(IPC::Connection& connection, IPC::Decoder& decoder, UniqueRef<IPC::Encoder>& replyEncoder)
{   
    return didReceiveSyncWebPageMessage(connection, decoder, replyEncoder);
}

#if ENABLE(ASYNC_SCROLLING)
ScrollingCoordinator* WebPage::scrollingCoordinator() const
{
    return m_page->scrollingCoordinator();
}
#endif

WebPage::SandboxExtensionTracker::~SandboxExtensionTracker()
{
    invalidate();
}

void WebPage::SandboxExtensionTracker::invalidate()
{
    m_pendingProvisionalSandboxExtension = nullptr;

    if (m_provisionalSandboxExtension) {
        m_provisionalSandboxExtension->revoke();
        m_provisionalSandboxExtension = nullptr;
    }

    if (m_committedSandboxExtension) {
        m_committedSandboxExtension->revoke();
        m_committedSandboxExtension = nullptr;
    }
}

void WebPage::SandboxExtensionTracker::willPerformLoadDragDestinationAction(RefPtr<SandboxExtension>&& pendingDropSandboxExtension)
{
    setPendingProvisionalSandboxExtension(WTFMove(pendingDropSandboxExtension));
}

void WebPage::SandboxExtensionTracker::beginLoad(WebFrame* frame, SandboxExtension::Handle&& handle)
{
    ASSERT_UNUSED(frame, frame->isMainFrame());

    setPendingProvisionalSandboxExtension(SandboxExtension::create(WTFMove(handle)));
}

void WebPage::SandboxExtensionTracker::beginReload(WebFrame* frame, SandboxExtension::Handle&& handle)
{
    ASSERT_UNUSED(frame, frame->isMainFrame());

    // Maintain existing provisional SandboxExtension in case of a reload, if the new handle is null. This is needed
    // because the UIProcess sends us a null handle if it already sent us a handle for this path in the past.
    if (auto sandboxExtension = SandboxExtension::create(WTFMove(handle)))
        setPendingProvisionalSandboxExtension(WTFMove(sandboxExtension));
}

void WebPage::SandboxExtensionTracker::setPendingProvisionalSandboxExtension(RefPtr<SandboxExtension>&& pendingProvisionalSandboxExtension)
{
    m_pendingProvisionalSandboxExtension = WTFMove(pendingProvisionalSandboxExtension);
}

bool WebPage::SandboxExtensionTracker::shouldReuseCommittedSandboxExtension(WebFrame* frame)
{
    ASSERT(frame->isMainFrame());

    FrameLoader& frameLoader = frame->coreFrame()->loader();
    FrameLoadType frameLoadType = frameLoader.loadType();

    // If the page is being reloaded, it should reuse whatever extension is committed.
    if (isReload(frameLoadType))
        return true;

    if (m_pendingProvisionalSandboxExtension)
        return false;

    DocumentLoader* documentLoader = frameLoader.documentLoader();
    DocumentLoader* provisionalDocumentLoader = frameLoader.provisionalDocumentLoader();
    if (!documentLoader || !provisionalDocumentLoader)
        return false;

    if (documentLoader->url().isLocalFile() && provisionalDocumentLoader->url().isLocalFile())
        return true;

    return false;
}

void WebPage::SandboxExtensionTracker::didStartProvisionalLoad(WebFrame* frame)
{
    if (!frame->isMainFrame())
        return;

    // We should only reuse the commited sandbox extension if it is not null. It can be
    // null if the last load was for an error page.
    if (m_committedSandboxExtension && shouldReuseCommittedSandboxExtension(frame))
        m_pendingProvisionalSandboxExtension = m_committedSandboxExtension;

    ASSERT(!m_provisionalSandboxExtension);

    m_provisionalSandboxExtension = WTFMove(m_pendingProvisionalSandboxExtension);
    if (!m_provisionalSandboxExtension)
        return;

    m_provisionalSandboxExtension->consume();
}

void WebPage::SandboxExtensionTracker::didCommitProvisionalLoad(WebFrame* frame)
{
    if (!frame->isMainFrame())
        return;

    if (m_committedSandboxExtension)
        m_committedSandboxExtension->revoke();

    m_committedSandboxExtension = WTFMove(m_provisionalSandboxExtension);

    // We can also have a non-null m_pendingProvisionalSandboxExtension if a new load is being started.
    // This extension is not cleared, because it does not pertain to the failed load, and will be needed.
}

void WebPage::SandboxExtensionTracker::didFailProvisionalLoad(WebFrame* frame)
{
    if (!frame->isMainFrame())
        return;

    if (!m_provisionalSandboxExtension)
        return;

    m_provisionalSandboxExtension->revoke();
    m_provisionalSandboxExtension = nullptr;

    // We can also have a non-null m_pendingProvisionalSandboxExtension if a new load is being started
    // (notably, if the current one fails because the new one cancels it). This extension is not cleared,
    // because it does not pertain to the failed load, and will be needed.
}

bool WebPage::hasLocalDataForURL(const URL& url)
{
    if (url.isLocalFile())
        return true;

    DocumentLoader* documentLoader = m_page->mainFrame().loader().documentLoader();
    if (documentLoader && documentLoader->subresource(url))
        return true;

    return false;
}

void WebPage::setCustomTextEncodingName(const String& encoding)
{
    m_page->mainFrame().loader().reloadWithOverrideEncoding(encoding);
}

void WebPage::didRemoveBackForwardItem(const BackForwardItemIdentifier& itemID)
{
    WebBackForwardListProxy::removeItem(itemID);
}

void WebPage::effectiveAppearanceDidChange(bool useDarkAppearance, bool useElevatedUserInterfaceLevel)
{
    corePage()->effectiveAppearanceDidChange(useDarkAppearance, useElevatedUserInterfaceLevel);
}

void WebPage::freezeLayerTreeDueToSwipeAnimation()
{
    freezeLayerTree(LayerTreeFreezeReason::SwipeAnimation);
}

void WebPage::unfreezeLayerTreeDueToSwipeAnimation()
{
    unfreezeLayerTree(LayerTreeFreezeReason::SwipeAnimation);
}

void WebPage::beginPrinting(FrameIdentifier frameID, const PrintInfo& printInfo)
{
    PrintContextAccessScope scope { *this };

    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;

    Frame* coreFrame = frame->coreFrame();
    if (!coreFrame)
        return;

#if PLATFORM(MAC)
    if (pdfDocumentForPrintingFrame(coreFrame))
        return;
#endif

    if (!m_printContext) {
        m_printContext = makeUnique<PrintContext>(coreFrame);
        m_page->dispatchBeforePrintEvent();
    }

    freezeLayerTree(LayerTreeFreezeReason::Printing);

    auto computedPageSize = m_printContext->computedPageSize(FloatSize(printInfo.availablePaperWidth, printInfo.availablePaperHeight), printInfo.margin);

    m_printContext->begin(computedPageSize.width(), computedPageSize.height());

    // PrintContext::begin() performed a synchronous layout which might have executed a
    // script that closed the WebPage, clearing m_printContext.
    // See <rdar://problem/49731211> for cases of this happening.
    if (!m_printContext) {
        unfreezeLayerTree(LayerTreeFreezeReason::Printing);
        return;
    }

    float fullPageHeight;
    m_printContext->computePageRects(FloatRect(0, 0, computedPageSize.width(), computedPageSize.height()), 0, 0, printInfo.pageSetupScaleFactor, fullPageHeight, true);

#if PLATFORM(GTK)
    if (!m_printOperation)
        m_printOperation = WebPrintOperationGtk::create(this, printInfo);
#endif
}

void WebPage::endPrinting()
{
    if (m_inActivePrintContextAccessScope) {
        m_shouldEndPrintingImmediately = true;
        return;
    }
    endPrintingImmediately();
}

void WebPage::endPrintingImmediately()
{
    RELEASE_ASSERT(!m_inActivePrintContextAccessScope);
    m_shouldEndPrintingImmediately = false;

    unfreezeLayerTree(LayerTreeFreezeReason::Printing);

    if (m_printContext) {
        m_printContext = nullptr;
        m_page->dispatchAfterPrintEvent();
    }
}

void WebPage::computePagesForPrinting(FrameIdentifier frameID, const PrintInfo& printInfo, CompletionHandler<void(const Vector<WebCore::IntRect>&, double, const WebCore::FloatBoxExtent&)>&& completionHandler)
{
    PrintContextAccessScope scope { *this };
    Vector<IntRect> resultPageRects;
    double resultTotalScaleFactorForPrinting = 1;
    auto computedPageMargin = printInfo.margin;
    computePagesForPrintingImpl(frameID, printInfo, resultPageRects, resultTotalScaleFactorForPrinting, computedPageMargin);
    completionHandler(resultPageRects, resultTotalScaleFactorForPrinting, computedPageMargin);
}

void WebPage::computePagesForPrintingImpl(FrameIdentifier frameID, const PrintInfo& printInfo, Vector<WebCore::IntRect>& resultPageRects, double& resultTotalScaleFactorForPrinting, FloatBoxExtent& computedPageMargin)
{
    ASSERT(resultPageRects.isEmpty());

    beginPrinting(frameID, printInfo);

    if (m_printContext) {
        PrintContextAccessScope scope { *this };
        resultPageRects = m_printContext->pageRects();
        computedPageMargin = m_printContext->computedPageMargin(printInfo.margin);
        auto computedPageSize = m_printContext->computedPageSize(FloatSize(printInfo.availablePaperWidth, printInfo.availablePaperHeight), printInfo.margin);
        resultTotalScaleFactorForPrinting = m_printContext->computeAutomaticScaleFactor(computedPageSize) * printInfo.pageSetupScaleFactor;
    }
#if PLATFORM(COCOA)
    else
        computePagesForPrintingPDFDocument(frameID, printInfo, resultPageRects);
#endif // PLATFORM(COCOA)

    // If we're asked to print, we should actually print at least a blank page.
    if (resultPageRects.isEmpty())
        resultPageRects.append(IntRect(0, 0, 1, 1));
}

#if PLATFORM(COCOA)
void WebPage::drawToPDF(FrameIdentifier frameID, const std::optional<FloatRect>& rect, CompletionHandler<void(const IPC::DataReference&)>&& completionHandler)
{
    auto& frameView = *m_page->mainFrame().view();
    IntSize snapshotSize;
    if (rect)
        snapshotSize = IntSize(rect->size());
    else
        snapshotSize = { frameView.contentsSize() };

    IntRect snapshotRect;
    if (rect)
        snapshotRect = { {(int)rect->x(), (int)rect->y()}, snapshotSize };
    else
        snapshotRect = { {0, 0}, snapshotSize };

    auto originalLayoutViewportOverrideRect = frameView.layoutViewportOverrideRect();
    frameView.setLayoutViewportOverrideRect(LayoutRect(snapshotRect));
    auto originalPaintBehavior = frameView.paintBehavior();
    frameView.setPaintBehavior(originalPaintBehavior | PaintBehavior::AnnotateLinks);

    auto pdfData = pdfSnapshotAtSize(snapshotRect, snapshotSize, 0);

    frameView.setLayoutViewportOverrideRect(originalLayoutViewportOverrideRect);
    frameView.setPaintBehavior(originalPaintBehavior);

    completionHandler(IPC::DataReference(CFDataGetBytePtr(pdfData.get()), CFDataGetLength(pdfData.get())));
}

void WebPage::drawRectToImage(FrameIdentifier frameID, const PrintInfo& printInfo, const IntRect& rect, const WebCore::IntSize& imageSize, CompletionHandler<void(const WebKit::ShareableBitmap::Handle&)>&& completionHandler)
{
    PrintContextAccessScope scope { *this };
    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    Frame* coreFrame = frame ? frame->coreFrame() : 0;

    RefPtr<WebImage> image;

#if USE(CG)
    if (coreFrame) {
#if PLATFORM(MAC)
        ASSERT(coreFrame->document()->printing() || pdfDocumentForPrintingFrame(coreFrame));
#else
        ASSERT(coreFrame->document()->printing());
#endif

        auto bitmap = ShareableBitmap::createShareable(imageSize, { });
        if (!bitmap) {
            ASSERT_NOT_REACHED();
            return completionHandler({ });
        }
        auto graphicsContext = bitmap->createGraphicsContext();
        if (graphicsContext) {
            float printingScale = static_cast<float>(imageSize.width()) / rect.width();
            graphicsContext->scale(printingScale);

#if PLATFORM(MAC)
            if (RetainPtr<PDFDocument> pdfDocument = pdfDocumentForPrintingFrame(coreFrame)) {
                ASSERT(!m_printContext);
                graphicsContext->scale(FloatSize(1, -1));
                graphicsContext->translate(0, -rect.height());
                drawPDFDocument(graphicsContext->platformContext(), pdfDocument.get(), printInfo, rect);
            } else
#endif
            {
                m_printContext->spoolRect(*graphicsContext, rect);
            }
        }

        image = WebImage::create(bitmap.releaseNonNull());
    }
#endif

    ShareableBitmap::Handle handle;

    if (image)
        image->bitmap().createHandle(handle, SharedMemory::Protection::ReadOnly);

    completionHandler(handle);
}

void WebPage::drawPagesToPDF(FrameIdentifier frameID, const PrintInfo& printInfo, uint32_t first, uint32_t count, CompletionHandler<void(const IPC::DataReference&)>&& callback)
{
    PrintContextAccessScope scope { *this };
    RetainPtr<CFMutableDataRef> pdfPageData;
    drawPagesToPDFImpl(frameID, printInfo, first, count, pdfPageData);
    callback({ CFDataGetBytePtr(pdfPageData.get()), static_cast<size_t>(CFDataGetLength(pdfPageData.get())) });
}

void WebPage::drawPagesToPDFImpl(FrameIdentifier frameID, const PrintInfo& printInfo, uint32_t first, uint32_t count, RetainPtr<CFMutableDataRef>& pdfPageData)
{
    WebFrame* frame = WebProcess::singleton().webFrame(frameID);
    Frame* coreFrame = frame ? frame->coreFrame() : 0;

    pdfPageData = adoptCF(CFDataCreateMutable(0, 0));

#if USE(CG)
    if (coreFrame) {

#if PLATFORM(MAC)
        ASSERT(coreFrame->document()->printing() || pdfDocumentForPrintingFrame(coreFrame));
#else
        ASSERT(coreFrame->document()->printing());
#endif

        // FIXME: Use CGDataConsumerCreate with callbacks to avoid copying the data.
        RetainPtr<CGDataConsumerRef> pdfDataConsumer = adoptCF(CGDataConsumerCreateWithCFData(pdfPageData.get()));

        CGRect mediaBox = (m_printContext && m_printContext->pageCount()) ? m_printContext->pageRect(0) : CGRectMake(0, 0, printInfo.availablePaperWidth, printInfo.availablePaperHeight);
        RetainPtr<CGContextRef> context = adoptCF(CGPDFContextCreate(pdfDataConsumer.get(), &mediaBox, 0));

#if PLATFORM(MAC)
        if (RetainPtr<PDFDocument> pdfDocument = pdfDocumentForPrintingFrame(coreFrame)) {
            ASSERT(!m_printContext);
            drawPagesToPDFFromPDFDocument(context.get(), pdfDocument.get(), printInfo, first, count);
        } else
#endif
        {
            size_t pageCount = m_printContext->pageCount();
            for (uint32_t page = first; page < first + count; ++page) {
                if (page >= pageCount)
                    break;

                RetainPtr<CFDictionaryRef> pageInfo = adoptCF(CFDictionaryCreateMutable(0, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
                CGPDFContextBeginPage(context.get(), pageInfo.get());

                GraphicsContextCG ctx(context.get());
                ctx.scale(FloatSize(1, -1));
                ctx.translate(0, -m_printContext->pageRect(page).height());
                m_printContext->spoolPage(ctx, page, m_printContext->pageRect(page).width());

                CGPDFContextEndPage(context.get());
            }
        }
        CGPDFContextClose(context.get());
    }
#endif
}

#elif PLATFORM(GTK)
void WebPage::drawPagesForPrinting(FrameIdentifier frameID, const PrintInfo& printInfo, CompletionHandler<void(const WebCore::ResourceError&)>&& completionHandler)
{
    beginPrinting(frameID, printInfo);
    if (m_printContext && m_printOperation) {
        m_printOperation->startPrint(m_printContext.get(), [this, protectedThis = makeRef(*this), completionHandler = WTFMove(completionHandler)] (const WebCore::ResourceError& error) mutable {
            m_printOperation = nullptr;
            completionHandler(error);
        });
        return;
    }
    completionHandler({ });
}
#endif

void WebPage::addResourceRequest(unsigned long identifier, const WebCore::ResourceRequest& request)
{
    if (!request.url().protocolIsInHTTPFamily())
        return;

    if (m_mainFrameProgressCompleted && !UserGestureIndicator::processingUserGesture())
        return;

    ASSERT(!m_trackedNetworkResourceRequestIdentifiers.contains(identifier));
    bool wasEmpty = m_trackedNetworkResourceRequestIdentifiers.isEmpty();
    m_trackedNetworkResourceRequestIdentifiers.add(identifier);
    if (wasEmpty)
        send(Messages::WebPageProxy::SetNetworkRequestsInProgress(true));
}

void WebPage::removeResourceRequest(unsigned long identifier)
{
    if (!m_trackedNetworkResourceRequestIdentifiers.remove(identifier))
        return;

    if (m_trackedNetworkResourceRequestIdentifiers.isEmpty())
        send(Messages::WebPageProxy::SetNetworkRequestsInProgress(false));
}

void WebPage::setMediaVolume(float volume)
{
    m_page->setMediaVolume(volume);
}

void WebPage::setMuted(MediaProducer::MutedStateFlags state, CompletionHandler<void()>&& completionHandler)
{
    m_page->setMuted(state);
    completionHandler();
}

void WebPage::setMayStartMediaWhenInWindow(bool mayStartMedia)
{
    if (mayStartMedia == m_mayStartMediaWhenInWindow)
        return;

    m_mayStartMediaWhenInWindow = mayStartMedia;
    if (m_mayStartMediaWhenInWindow && m_page->isInWindow())
        m_setCanStartMediaTimer.startOneShot(0_s);
}

void WebPage::runModal()
{
    if (m_isClosed)
        return;
    if (m_isRunningModal)
        return;

    m_isRunningModal = true;
    send(Messages::WebPageProxy::RunModal());
#if ASSERT_ENABLED
    Ref<WebPage> protector(*this);
#endif
    RunLoop::run();
    ASSERT(!m_isRunningModal);
}

bool WebPage::canHandleRequest(const WebCore::ResourceRequest& request)
{
    if (LegacySchemeRegistry::shouldLoadURLSchemeAsEmptyDocument(request.url().protocol().toStringWithoutCopying()))
        return true;

    if (request.url().protocolIsBlob())
        return true;

    return platformCanHandleRequest(request);
}

#if PLATFORM(COCOA)
void WebPage::handleAlternativeTextUIResult(const String& result)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.editor().handleAlternativeTextUIResult(result);
}
#endif

void WebPage::simulateMouseDown(int button, WebCore::IntPoint position, int clickCount, WKEventModifiers modifiers, WallTime time)
{
    static_assert(sizeof(WKEventModifiers) >= sizeof(WebEvent::Modifier), "WKEventModifiers must be greater than or equal to the size of WebEvent::Modifier");
    mouseEvent(WebMouseEvent(WebMouseEvent::MouseDown, static_cast<WebMouseEvent::Button>(button), 0, position, position, 0, 0, 0, clickCount, OptionSet<WebEvent::Modifier>::fromRaw(modifiers), time, WebCore::ForceAtClick, WebMouseEvent::NoTap), std::nullopt);
}

void WebPage::simulateMouseUp(int button, WebCore::IntPoint position, int clickCount, WKEventModifiers modifiers, WallTime time)
{
    static_assert(sizeof(WKEventModifiers) >= sizeof(WebEvent::Modifier), "WKEventModifiers must be greater than or equal to the size of WebEvent::Modifier");
    mouseEvent(WebMouseEvent(WebMouseEvent::MouseUp, static_cast<WebMouseEvent::Button>(button), 0, position, position, 0, 0, 0, clickCount, OptionSet<WebEvent::Modifier>::fromRaw(modifiers), time, WebCore::ForceAtClick, WebMouseEvent::NoTap), std::nullopt);
}

void WebPage::simulateMouseMotion(WebCore::IntPoint position, WallTime time)
{
    mouseEvent(WebMouseEvent(WebMouseEvent::MouseMove, WebMouseEvent::NoButton, 0, position, position, 0, 0, 0, 0, OptionSet<WebEvent::Modifier> { }, time, 0, WebMouseEvent::NoTap), std::nullopt);
}

void WebPage::setCompositionForTesting(const String& compositionString, uint64_t from, uint64_t length, bool suppressUnderline, const Vector<CompositionHighlight>& highlights)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().canEdit())
        return;

    Vector<CompositionUnderline> underlines;
    if (!suppressUnderline)
        underlines.append(CompositionUnderline(0, compositionString.length(), CompositionUnderlineColor::TextColor, Color(Color::black), false));

    frame.editor().setComposition(compositionString, underlines, highlights, from, from + length);
}

bool WebPage::hasCompositionForTesting()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    return frame.editor().hasComposition();
}

void WebPage::confirmCompositionForTesting(const String& compositionString)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().canEdit())
        return;

    if (compositionString.isNull())
        frame.editor().confirmComposition();
    frame.editor().confirmComposition(compositionString);
}

void WebPage::wheelEventHandlersChanged(bool hasHandlers)
{
    if (m_hasWheelEventHandlers == hasHandlers)
        return;

    m_hasWheelEventHandlers = hasHandlers;
    recomputeShortCircuitHorizontalWheelEventsState();
}

static bool hasEnabledHorizontalScrollbar(ScrollableArea* scrollableArea)
{
    if (Scrollbar* scrollbar = scrollableArea->horizontalScrollbar())
        return scrollbar->enabled();

    return false;
}

static bool pageContainsAnyHorizontalScrollbars(Frame* mainFrame)
{
    if (FrameView* frameView = mainFrame->view()) {
        if (hasEnabledHorizontalScrollbar(frameView))
            return true;
    }

    for (Frame* frame = mainFrame; frame; frame = frame->tree().traverseNext()) {
        FrameView* frameView = frame->view();
        if (!frameView)
            continue;

        const HashSet<ScrollableArea*>* scrollableAreas = frameView->scrollableAreas();
        if (!scrollableAreas)
            continue;

        for (HashSet<ScrollableArea*>::const_iterator it = scrollableAreas->begin(), end = scrollableAreas->end(); it != end; ++it) {
            ScrollableArea* scrollableArea = *it;
            if (!scrollableArea->scrollbarsCanBeActive())
                continue;

            if (hasEnabledHorizontalScrollbar(scrollableArea))
                return true;
        }
    }

    return false;
}

void WebPage::recomputeShortCircuitHorizontalWheelEventsState()
{
    bool canShortCircuitHorizontalWheelEvents = !m_hasWheelEventHandlers;

    if (canShortCircuitHorizontalWheelEvents) {
        // Check if we have any horizontal scroll bars on the page.
        if (pageContainsAnyHorizontalScrollbars(mainFrame()))
            canShortCircuitHorizontalWheelEvents = false;
    }

    if (m_canShortCircuitHorizontalWheelEvents == canShortCircuitHorizontalWheelEvents)
        return;

    m_canShortCircuitHorizontalWheelEvents = canShortCircuitHorizontalWheelEvents;
    send(Messages::WebPageProxy::SetCanShortCircuitHorizontalWheelEvents(m_canShortCircuitHorizontalWheelEvents));
}

Frame* WebPage::mainFrame() const
{
    return m_page ? &m_page->mainFrame() : nullptr;
}

FrameView* WebPage::mainFrameView() const
{
    if (Frame* frame = mainFrame())
        return frame->view();
    
    return nullptr;
}

bool WebPage::shouldUseCustomContentProviderForResponse(const ResourceResponse& response)
{
    auto& mimeType = response.mimeType();
    if (mimeType.isNull())
        return false;

    return m_mimeTypesWithCustomContentProviders.contains(mimeType);
}

#if PLATFORM(COCOA)

void WebPage::setTextAsync(const String& text)
{
    auto frame = makeRef(m_page->focusController().focusedOrMainFrame());
    if (frame->selection().selection().isContentEditable()) {
        UserTypingGestureIndicator indicator(frame.get());
        frame->selection().selectAll();
        if (text.isEmpty())
            frame->editor().deleteSelectionWithSmartDelete(false);
        else
            frame->editor().insertText(text, nullptr, TextEventInputKeyboard);
        return;
    }

    if (is<HTMLInputElement>(m_focusedElement.get())) {
        downcast<HTMLInputElement>(*m_focusedElement).setValueForUser(text);
        return;
    }

    ASSERT_NOT_REACHED();
}

void WebPage::insertTextAsync(const String& text, const EditingRange& replacementEditingRange, InsertTextOptions&& options)
{
    platformWillPerformEditingCommand();

    auto& frame = m_page->focusController().focusedOrMainFrame();

    Ref<Frame> protector(frame);

    UserGestureIndicator gestureIndicator { options.processingUserGesture ? ProcessingUserGesture : NotProcessingUserGesture, frame.document() };

    bool replacesText = false;
    if (replacementEditingRange.location != notFound) {
        if (auto replacementRange = EditingRange::toRange(frame, replacementEditingRange, options.editingRangeIsRelativeTo)) {
            SetForScope<bool> isSelectingTextWhileInsertingAsynchronously(m_isSelectingTextWhileInsertingAsynchronously, options.suppressSelectionUpdate);
            frame.selection().setSelection(VisibleSelection(*replacementRange));
            replacesText = replacementEditingRange.length;
        }
    }
    
    if (options.registerUndoGroup)
        send(Messages::WebPageProxy::RegisterInsertionUndoGrouping());

    RefPtr<Element> focusedElement = frame.document() ? frame.document()->focusedElement() : nullptr;
    if (focusedElement && options.shouldSimulateKeyboardInput)
        focusedElement->dispatchEvent(Event::create(eventNames().keydownEvent, Event::CanBubble::Yes, Event::IsCancelable::Yes));

    if (!frame.editor().hasComposition()) {
        // An insertText: might be handled by other responders in the chain if we don't handle it.
        // One example is space bar that results in scrolling down the page.
        frame.editor().insertText(text, nullptr, replacesText ? TextEventInputAutocompletion : TextEventInputKeyboard);
    } else
        frame.editor().confirmComposition(text);

    if (focusedElement && options.shouldSimulateKeyboardInput) {
        focusedElement->dispatchEvent(Event::create(eventNames().keyupEvent, Event::CanBubble::Yes, Event::IsCancelable::Yes));
        focusedElement->dispatchEvent(Event::create(eventNames().changeEvent, Event::CanBubble::Yes, Event::IsCancelable::Yes));
    }
}

void WebPage::hasMarkedText(CompletionHandler<void(bool)>&& completionHandler)
{
    completionHandler(m_page->focusController().focusedOrMainFrame().editor().hasComposition());
}

void WebPage::getMarkedRangeAsync(CompletionHandler<void(const EditingRange&)>&& completionHandler)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    completionHandler(EditingRange::fromRange(frame, frame.editor().compositionRange()));
}

void WebPage::getSelectedRangeAsync(CompletionHandler<void(const EditingRange&)>&& completionHandler)
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    completionHandler(EditingRange::fromRange(frame, frame.selection().selection().toNormalizedRange()));
}

void WebPage::characterIndexForPointAsync(const WebCore::IntPoint& point, CompletionHandler<void(uint64_t)>&& completionHandler)
{
    constexpr OptionSet<HitTestRequest::Type> hitType { HitTestRequest::Type::ReadOnly, HitTestRequest::Type::Active, HitTestRequest::Type::DisallowUserAgentShadowContent,  HitTestRequest::Type::AllowChildFrameContent };
    auto result = m_page->mainFrame().eventHandler().hitTestResultAtPoint(point, hitType);
    auto& frame = result.innerNonSharedNode() ? *result.innerNodeFrame() : m_page->focusController().focusedOrMainFrame();
    auto range = frame.rangeForPoint(result.roundedPointInInnerNodeFrame());
    auto editingRange = EditingRange::fromRange(frame, range);
    completionHandler(editingRange.location);
}

void WebPage::firstRectForCharacterRangeAsync(const EditingRange& editingRange, CompletionHandler<void(const WebCore::IntRect&, const EditingRange&)>&& completionHandler)
{
    auto& frame = m_page->focusController().focusedOrMainFrame();
    auto range = EditingRange::toRange(frame, editingRange);
    if (!range)
        return completionHandler({ }, { });

    // FIXME: Pass an EditingRange that matches the range of the first rect, rather than the entire passed-in range?
    auto rect = frame.view()->contentsToWindow(frame.editor().firstRectForRange(*range));
    completionHandler(rect, editingRange);
}

void WebPage::setCompositionAsync(const String& text, const Vector<CompositionUnderline>& underlines, const Vector<CompositionHighlight>& highlights, const EditingRange& selection, const EditingRange& replacementEditingRange)
{
    platformWillPerformEditingCommand();

    auto& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.selection().selection().isContentEditable()) {
        if (replacementEditingRange.location != notFound) {
            if (auto replacementRange = EditingRange::toRange(frame, replacementEditingRange))
                frame.selection().setSelection(VisibleSelection(*replacementRange));
        }
        frame.editor().setComposition(text, underlines, highlights, selection.location, selection.location + selection.length);
    }
}

void WebPage::confirmCompositionAsync()
{
    platformWillPerformEditingCommand();

    Frame& frame = m_page->focusController().focusedOrMainFrame();
    frame.editor().confirmComposition();
}

#endif // PLATFORM(COCOA)

#if PLATFORM(GTK) || PLATFORM(WPE)

static Frame* targetFrameForEditing(WebPage& page)
{
    Frame& targetFrame = page.corePage()->focusController().focusedOrMainFrame();

    Editor& editor = targetFrame.editor();
    if (!editor.canEdit())
        return nullptr;

    if (editor.hasComposition()) {
        // We should verify the parent node of this IME composition node are
        // editable because JavaScript may delete a parent node of the composition
        // node. In this case, WebKit crashes while deleting texts from the parent
        // node, which doesn't exist any longer.
        if (auto range = editor.compositionRange()) {
            if (!range->startContainer().isContentEditable())
                return nullptr;
        }
    }
    return &targetFrame;
}

void WebPage::cancelComposition(const String& compositionString)
{
    if (auto* targetFrame = targetFrameForEditing(*this))
        targetFrame->editor().confirmComposition(compositionString);
}

void WebPage::deleteSurrounding(int64_t offset, unsigned characterCount)
{
    auto* targetFrame = targetFrameForEditing(*this);
    if (!targetFrame)
        return;

    auto& selection = targetFrame->selection().selection();
    if (selection.isNone())
        return;

    auto selectionStart = selection.visibleStart();
    auto surroundingRange = makeSimpleRange(startOfEditableContent(selectionStart), selectionStart);
    if (!surroundingRange)
        return;

    auto& rootNode = surroundingRange->start.container->treeScope().rootNode();
    auto characterRange = WebCore::CharacterRange(WebCore::characterCount(*surroundingRange) + offset, characterCount);
    auto selectionRange = resolveCharacterRange(makeRangeSelectingNodeContents(rootNode), characterRange);

    targetFrame->editor().setIgnoreSelectionChanges(true);
    targetFrame->selection().setSelection(VisibleSelection(selectionRange));
    targetFrame->editor().deleteSelectionWithSmartDelete(false);
    targetFrame->editor().setIgnoreSelectionChanges(false);
    sendEditorStateUpdate();
}

#endif

void WebPage::didApplyStyle()
{
    sendEditorStateUpdate();
}

void WebPage::didChangeContents()
{
    sendEditorStateUpdate();
}

void WebPage::didChangeOverflowScrollPosition()
{
    didChangeSelectionOrOverflowScrollPosition();
}

void WebPage::didChangeSelection()
{
    didChangeSelectionOrOverflowScrollPosition();
}

void WebPage::didChangeSelectionOrOverflowScrollPosition()
{
    // Similarly, we don't want to propagate changes to the web process when inserting text asynchronously, since we will
    // end up with a range selection very briefly right before inserting the text.
    if (m_isSelectingTextWhileInsertingAsynchronously)
        return;

#if PLATFORM(MAC)
    bool hasPreviouslyFocusedDueToUserInteraction = m_hasEverFocusedElementDueToUserInteractionSincePageTransition;
    m_hasEverFocusedElementDueToUserInteractionSincePageTransition |= m_userIsInteracting;

    if (!hasPreviouslyFocusedDueToUserInteraction && m_hasEverFocusedElementDueToUserInteractionSincePageTransition) {
        send(Messages::WebPageProxy::SetHasHadSelectionChangesFromUserInteraction(m_hasEverFocusedElementDueToUserInteractionSincePageTransition));
    }

    // Abandon the current inline input session if selection changed for any other reason but an input method direct action.
    // FIXME: This logic should be in WebCore.
    // FIXME: Many changes that affect composition node do not go through didChangeSelection(). We need to do something when DOM manipulation affects the composition, because otherwise input method's idea about it will be different from Editor's.
    // FIXME: We can't cancel composition when selection changes to NoSelection, but we probably should.
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.editor().hasComposition() && !frame.editor().ignoreSelectionChanges() && !frame.selection().isNone()) {
        frame.editor().cancelComposition();
        discardedComposition();
        return;
    }
#endif

    scheduleFullEditorStateUpdate();
}

void WebPage::resetFocusedElementForFrame(WebFrame* frame)
{
    if (!m_focusedElement)
        return;

    if (frame->isMainFrame() || m_focusedElement->document().frame() == frame->coreFrame()) {
#if PLATFORM(IOS_FAMILY)
        send(Messages::WebPageProxy::ElementDidBlur());
#elif PLATFORM(MAC)
        send(Messages::WebPageProxy::SetEditableElementIsFocused(false));
#endif
        m_focusedElement = nullptr;
    }
}

void WebPage::elementDidRefocus(WebCore::Element& element)
{
    elementDidFocus(element);

    if (m_userIsInteracting)
        scheduleFullEditorStateUpdate();
}

bool WebPage::shouldDispatchUpdateAfterFocusingElement(const Element& element) const
{
    if (m_focusedElement == &element || m_recentlyBlurredElement == &element) {
#if PLATFORM(IOS_FAMILY)
        return !m_isShowingInputViewForFocusedElement;
#else
        return false;
#endif
    }
    return true;
}

static bool isTextFormControlOrEditableContent(const WebCore::Element& element)
{
    return is<HTMLTextFormControlElement>(element) || element.hasEditableStyle();
}

void WebPage::elementDidFocus(WebCore::Element& element)
{
    if (!shouldDispatchUpdateAfterFocusingElement(element)) {
        updateInputContextAfterBlurringAndRefocusingElementIfNeeded(element);
        m_focusedElement = &element;
        m_recentlyBlurredElement = nullptr;
        return;
    }

    if (is<HTMLSelectElement>(element) || isTextFormControlOrEditableContent(element)) {
        m_focusedElement = &element;
        m_hasPendingInputContextUpdateAfterBlurringAndRefocusingElement = false;

#if PLATFORM(IOS_FAMILY)

        auto information = focusedElementInformation();
        if (!information)
            return;

        RefPtr<API::Object> userData;

        m_formClient->willBeginInputSession(this, &element, WebFrame::fromCoreFrame(*element.document().frame()), m_userIsInteracting, userData);

        send(Messages::WebPageProxy::ElementDidFocus(information.value(), m_userIsInteracting, m_recentlyBlurredElement, m_lastActivityStateChanges, UserData(WebProcess::singleton().transformObjectsToHandles(userData.get()).get())));
#elif PLATFORM(MAC)
        // FIXME: This can be unified with the iOS code above by bringing ElementDidFocus to macOS.
        // This also doesn't take other noneditable controls into account, such as input type color.
        send(Messages::WebPageProxy::SetEditableElementIsFocused(!element.hasTagName(WebCore::HTMLNames::selectTag)));
#endif
        m_recentlyBlurredElement = nullptr;
    }
}

void WebPage::elementDidBlur(WebCore::Element& element)
{
    if (m_focusedElement == &element) {
        m_recentlyBlurredElement = WTFMove(m_focusedElement);
        callOnMainRunLoop([protectedThis = makeRefPtr(this)] {
            if (protectedThis->m_recentlyBlurredElement) {
#if PLATFORM(IOS_FAMILY)
                protectedThis->send(Messages::WebPageProxy::ElementDidBlur());
#elif PLATFORM(MAC)
                protectedThis->send(Messages::WebPageProxy::SetEditableElementIsFocused(false));
#endif
            }
            protectedThis->m_recentlyBlurredElement = nullptr;
        });
        m_hasPendingInputContextUpdateAfterBlurringAndRefocusingElement = false;
    }
}

void WebPage::focusedElementDidChangeInputMode(WebCore::Element& element, WebCore::InputMode mode)
{
    if (m_focusedElement != &element)
        return;

#if PLATFORM(IOS_FAMILY)
    ASSERT(is<HTMLElement>(element));
    ASSERT(downcast<HTMLElement>(element).canonicalInputMode() == mode);

    if (!isTextFormControlOrEditableContent(element))
        return;

    send(Messages::WebPageProxy::FocusedElementDidChangeInputMode(mode));
#else
    UNUSED_PARAM(mode);
#endif
}

void WebPage::didUpdateComposition()
{
    sendEditorStateUpdate();
}

void WebPage::didEndUserTriggeredSelectionChanges()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (!frame.editor().ignoreSelectionChanges())
        sendEditorStateUpdate();
}

void WebPage::discardedComposition()
{
    send(Messages::WebPageProxy::CompositionWasCanceled());
    sendEditorStateUpdate();
}

void WebPage::canceledComposition()
{
    send(Messages::WebPageProxy::CompositionWasCanceled());
    sendEditorStateUpdate();
}

void WebPage::setAlwaysShowsHorizontalScroller(bool alwaysShowsHorizontalScroller)
{
    if (alwaysShowsHorizontalScroller == m_alwaysShowsHorizontalScroller)
        return;

    m_alwaysShowsHorizontalScroller = alwaysShowsHorizontalScroller;
    auto view = corePage()->mainFrame().view();
    if (!alwaysShowsHorizontalScroller)
        view->setHorizontalScrollbarLock(false);
    view->setHorizontalScrollbarMode(alwaysShowsHorizontalScroller ? ScrollbarAlwaysOn : m_mainFrameIsScrollable ? ScrollbarAuto : ScrollbarAlwaysOff, alwaysShowsHorizontalScroller || !m_mainFrameIsScrollable);
}

void WebPage::setAlwaysShowsVerticalScroller(bool alwaysShowsVerticalScroller)
{
    if (alwaysShowsVerticalScroller == m_alwaysShowsVerticalScroller)
        return;

    m_alwaysShowsVerticalScroller = alwaysShowsVerticalScroller;
    auto view = corePage()->mainFrame().view();
    if (!alwaysShowsVerticalScroller)
        view->setVerticalScrollbarLock(false);
    view->setVerticalScrollbarMode(alwaysShowsVerticalScroller ? ScrollbarAlwaysOn : m_mainFrameIsScrollable ? ScrollbarAuto : ScrollbarAlwaysOff, alwaysShowsVerticalScroller || !m_mainFrameIsScrollable);
}

void WebPage::setMinimumSizeForAutoLayout(const IntSize& size)
{
    if (m_minimumSizeForAutoLayout == size)
        return;

    m_minimumSizeForAutoLayout = size;
    if (size.width() <= 0) {
        corePage()->mainFrame().view()->enableFixedWidthAutoSizeMode(false, { });
        return;
    }

    corePage()->mainFrame().view()->enableFixedWidthAutoSizeMode(true, { size.width(), std::max(size.height(), 1) });
}

void WebPage::setSizeToContentAutoSizeMaximumSize(const IntSize& size)
{
    if (m_sizeToContentAutoSizeMaximumSize == size)
        return;

    m_sizeToContentAutoSizeMaximumSize = size;
    if (size.width() <= 0 || size.height() <= 0) {
        corePage()->mainFrame().view()->enableSizeToContentAutoSizeMode(false, { });
        return;
    }

    corePage()->mainFrame().view()->enableSizeToContentAutoSizeMode(true, size);
}

void WebPage::setAutoSizingShouldExpandToViewHeight(bool shouldExpand)
{
    if (m_autoSizingShouldExpandToViewHeight == shouldExpand)
        return;

    m_autoSizingShouldExpandToViewHeight = shouldExpand;

    corePage()->mainFrame().view()->setAutoSizeFixedMinimumHeight(shouldExpand ? m_viewSize.height() : 0);
}

void WebPage::setViewportSizeForCSSViewportUnits(std::optional<WebCore::IntSize> viewportSize)
{
    if (m_viewportSizeForCSSViewportUnits == viewportSize)
        return;

    m_viewportSizeForCSSViewportUnits = viewportSize;
    if (m_viewportSizeForCSSViewportUnits)
        corePage()->mainFrame().view()->setViewportSizeForCSSViewportUnits(*m_viewportSizeForCSSViewportUnits);
}

bool WebPage::isSmartInsertDeleteEnabled()
{
    return m_page->settings().smartInsertDeleteEnabled();
}

void WebPage::setSmartInsertDeleteEnabled(bool enabled)
{
    if (m_page->settings().smartInsertDeleteEnabled() != enabled) {
        m_page->settings().setSmartInsertDeleteEnabled(enabled);
        setSelectTrailingWhitespaceEnabled(!enabled);
    }
}

bool WebPage::isSelectTrailingWhitespaceEnabled() const
{
    return m_page->settings().selectTrailingWhitespaceEnabled();
}

void WebPage::setSelectTrailingWhitespaceEnabled(bool enabled)
{
    if (m_page->settings().selectTrailingWhitespaceEnabled() != enabled) {
        m_page->settings().setSelectTrailingWhitespaceEnabled(enabled);
        setSmartInsertDeleteEnabled(!enabled);
    }
}

bool WebPage::canShowMIMEType(const String& mimeType) const
{
    if (MIMETypeRegistry::canShowMIMEType(mimeType))
        return true;

    if (!mimeType.isNull() && m_mimeTypesWithCustomContentProviders.contains(mimeType))
        return true;

    return false;
}

void WebPage::addTextCheckingRequest(TextCheckerRequestID requestID, Ref<TextCheckingRequest>&& request)
{
    m_pendingTextCheckingRequestMap.add(requestID, WTFMove(request));
}

void WebPage::didFinishCheckingText(TextCheckerRequestID requestID, const Vector<TextCheckingResult>& result)
{
    RefPtr<TextCheckingRequest> request = m_pendingTextCheckingRequestMap.take(requestID);
    if (!request)
        return;

    request->didSucceed(result);
}

void WebPage::didCancelCheckingText(TextCheckerRequestID requestID)
{
    RefPtr<TextCheckingRequest> request = m_pendingTextCheckingRequestMap.take(requestID);
    if (!request)
        return;

    request->didCancel();
}

void WebPage::willReplaceMultipartContent(const WebFrame& frame)
{
#if PLATFORM(IOS_FAMILY)
    if (!frame.isMainFrame())
        return;

    m_previousExposedContentRect = m_drawingArea->exposedContentRect();
#endif
}

void WebPage::didReplaceMultipartContent(const WebFrame& frame)
{
#if PLATFORM(IOS_FAMILY)
    if (!frame.isMainFrame())
        return;

    // Restore the previous exposed content rect so that it remains fixed when replacing content
    // from multipart/x-mixed-replace streams.
    m_drawingArea->setExposedContentRect(m_previousExposedContentRect);
#endif
}

void WebPage::didCommitLoad(WebFrame* frame)
{
#if PLATFORM(IOS_FAMILY)
    auto firstTransactionIDAfterDidCommitLoad = downcast<RemoteLayerTreeDrawingArea>(*m_drawingArea).nextTransactionID();
    frame->setFirstLayerTreeTransactionIDAfterDidCommitLoad(firstTransactionIDAfterDidCommitLoad);
    cancelPotentialTapInFrame(*frame);
#endif
    resetFocusedElementForFrame(frame);

    if (!frame->isMainFrame())
        return;

    if (m_drawingArea)
        m_drawingArea->sendEnterAcceleratedCompositingModeIfNeeded();

    ASSERT(!frame->coreFrame()->loader().stateMachine().creatingInitialEmptyDocument());
    unfreezeLayerTree(LayerTreeFreezeReason::ProcessSwap);

#if ENABLE(IMAGE_ANALYSIS)
    for (auto& [element, completionHandlers] : m_elementsPendingTextRecognition) {
        for (auto& completionHandler : completionHandlers)
            completionHandler({ });
    }
    m_elementsPendingTextRecognition.clear();
#endif

#if ENABLE(RESOURCE_LOAD_STATISTICS)
    clearLoadedSubresourceDomains();
#endif

    // If previous URL is invalid, then it's not a real page that's being navigated away from.
    // Most likely, this is actually the first load to be committed in this page.
    if (frame->coreFrame()->loader().previousURL().isValid())
        reportUsedFeatures();

    // Only restore the scale factor for standard frame loads (of the main frame).
    if (frame->coreFrame()->loader().loadType() == FrameLoadType::Standard) {
        Page* page = frame->coreFrame()->page();

        if (page && page->pageScaleFactor() != 1)
            scalePage(1, IntPoint());
    }

    m_didUpdateRenderingAfterCommittingLoad = false;

#if PLATFORM(IOS_FAMILY)
    m_hasReceivedVisibleContentRectsAfterDidCommitLoad = false;
    m_hasRestoredExposedContentRectAfterDidCommitLoad = false;
    m_lastTransactionIDWithScaleChange = firstTransactionIDAfterDidCommitLoad;
    m_scaleWasSetByUIProcess = false;
    m_userHasChangedPageScaleFactor = false;
    m_estimatedLatency = Seconds(1.0 / 60);
    m_shouldRevealCurrentSelectionAfterInsertion = true;
    m_lastLayerTreeTransactionIdAndPageScaleBeforeScalingPage = std::nullopt;

#if ENABLE(IOS_TOUCH_EVENTS)
    EventDispatcher::TouchEventQueue queuedEvents;
    WebProcess::singleton().eventDispatcher().takeQueuedTouchEventsForPage(*this, queuedEvents);
    cancelAsynchronousTouchEvents(WTFMove(queuedEvents));
#endif
#endif // PLATFORM(IOS_FAMILY)

#if ENABLE(META_VIEWPORT)
    resetViewportDefaultConfiguration(frame);
    const Frame* coreFrame = frame->coreFrame();

    bool viewportChanged = false;

    LOG_WITH_STREAM(VisibleRects, stream << "WebPage " << m_identifier.toUInt64() << " didCommitLoad setting content size to " << coreFrame->view()->contentsSize());
    if (m_viewportConfiguration.setContentsSize(coreFrame->view()->contentsSize()))
        viewportChanged = true;

    if (m_viewportConfiguration.setViewportArguments(coreFrame->document()->viewportArguments()))
        viewportChanged = true;

    if (m_viewportConfiguration.setIsKnownToLayOutWiderThanViewport(false))
        viewportChanged = true;

    if (viewportChanged)
        viewportConfigurationChanged();
#endif // ENABLE(META_VIEWPORT)

#if ENABLE(TEXT_AUTOSIZING)
    m_textAutoSizingAdjustmentTimer.stop();
#endif

#if USE(OS_STATE)
    m_loadCommitTime = WallTime::now();
#endif

    themeColorChanged();

    WebProcess::singleton().updateActivePages(m_processDisplayName);

    updateMainFrameScrollOffsetPinning();

    updateMockAccessibilityElementAfterCommittingLoad();
}

void WebPage::didFinishDocumentLoad(WebFrame& frame)
{
    if (!frame.isMainFrame())
        return;

#if ENABLE(VIEWPORT_RESIZING)
    shrinkToFitContent(ZoomToInitialScale::Yes);
#endif
}

void WebPage::didFinishLoad(WebFrame& frame)
{
    if (!frame.isMainFrame())
        return;

    WebProcess::singleton().sendPrewarmInformation(frame.url());

#if ENABLE(VIEWPORT_RESIZING)
    shrinkToFitContent(ZoomToInitialScale::Yes);
#endif
}

void WebPage::testProcessIncomingSyncMessagesWhenWaitingForSyncReply(Messages::WebPage::TestProcessIncomingSyncMessagesWhenWaitingForSyncReply::DelayedReply&& reply)
{
    RELEASE_ASSERT(IPC::UnboundedSynchronousIPCScope::hasOngoingUnboundedSyncIPC());
    reply(true);
}

std::optional<SimpleRange> WebPage::currentSelectionAsRange()
{
    auto* frame = frameWithSelection(m_page.get());
    if (!frame)
        return std::nullopt;

    return frame->selection().selection().toNormalizedRange();
}

void WebPage::reportUsedFeatures()
{
    Vector<String> namedFeatures;
    m_loaderClient->featuresUsedInPage(*this, namedFeatures);
}

void WebPage::sendEditorStateUpdate()
{
    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.editor().ignoreSelectionChanges() || !frame.document() || !frame.document()->hasLivingRenderTree())
        return;

    m_pendingEditorStateUpdateStatus = PendingEditorStateUpdateStatus::NotScheduled;

    // If we immediately dispatch an EditorState update to the UI process, layout may not be up to date yet.
    // If that is the case, just send what we have (i.e. don't include post-layout data) and wait until the
    // next layer tree commit to compute and send the complete EditorState over.
    auto state = editorState();
    send(Messages::WebPageProxy::EditorStateChanged(state));

    if (state.isMissingPostLayoutData)
        scheduleFullEditorStateUpdate();
}

void WebPage::scheduleFullEditorStateUpdate()
{
    if (hasPendingEditorStateUpdate()) {
        if (m_isChangingSelectionForAccessibility)
            m_pendingEditorStateUpdateStatus = PendingEditorStateUpdateStatus::ScheduledDuringAccessibilitySelectionChange;
        return;
    }

    if (m_isChangingSelectionForAccessibility)
        m_pendingEditorStateUpdateStatus = PendingEditorStateUpdateStatus::ScheduledDuringAccessibilitySelectionChange;
    else
        m_pendingEditorStateUpdateStatus = PendingEditorStateUpdateStatus::Scheduled;

    // FIXME: Scheduling a compositing layer flush here can be more expensive than necessary.
    // Instead, we should just compute and send post-layout editor state during the next frame.
    m_drawingArea->triggerRenderingUpdate();
}

void WebPage::flushPendingThemeColorChange()
{
    if (!m_pendingThemeColorChange)
        return;

    m_pendingThemeColorChange = false;

    send(Messages::WebPageProxy::ThemeColorChanged(m_page->themeColor()));
}

void WebPage::flushPendingPageExtendedBackgroundColorChange()
{
    if (!m_pendingPageExtendedBackgroundColorChange)
        return;

    m_pendingPageExtendedBackgroundColorChange = false;

    send(Messages::WebPageProxy::PageExtendedBackgroundColorDidChange(m_page->pageExtendedBackgroundColor()));
}

void WebPage::flushPendingSampledPageTopColorChange()
{
    if (!m_pendingSampledPageTopColorChange)
        return;

    m_pendingSampledPageTopColorChange = false;

    send(Messages::WebPageProxy::SampledPageTopColorChanged(m_page->sampledPageTopColor()));
}

void WebPage::flushPendingEditorStateUpdate()
{
    if (!hasPendingEditorStateUpdate())
        return;

    Frame& frame = m_page->focusController().focusedOrMainFrame();
    if (frame.editor().ignoreSelectionChanges())
        return;

    sendEditorStateUpdate();
}

void WebPage::updateWebsitePolicies(WebsitePoliciesData&& websitePolicies)
{
    if (!m_page)
        return;

    auto* documentLoader = m_page->mainFrame().loader().documentLoader();
    if (!documentLoader)
        return;

    m_allowsContentJavaScriptFromMostRecentNavigation = websitePolicies.allowsContentJavaScript;
    WebsitePoliciesData::applyToDocumentLoader(WTFMove(websitePolicies), *documentLoader);
    
#if ENABLE(VIDEO)
    m_page->updateMediaElementRateChangeRestrictions();
#endif
}

void WebPage::notifyUserScripts()
{
    if (!m_page)
        return;

    m_page->notifyToInjectUserScripts();
}

unsigned WebPage::extendIncrementalRenderingSuppression()
{
    unsigned token = m_maximumRenderingSuppressionToken + 1;
    while (!HashSet<unsigned>::isValidValue(token) || m_activeRenderingSuppressionTokens.contains(token))
        token++;

    m_activeRenderingSuppressionTokens.add(token);
    m_page->mainFrame().view()->setVisualUpdatesAllowedByClient(false);

    m_maximumRenderingSuppressionToken = token;

    return token;
}

void WebPage::stopExtendingIncrementalRenderingSuppression(unsigned token)
{
    if (!m_activeRenderingSuppressionTokens.remove(token))
        return;

    m_page->mainFrame().view()->setVisualUpdatesAllowedByClient(!shouldExtendIncrementalRenderingSuppression());
}
    
void WebPage::setScrollPinningBehavior(uint32_t pinning)
{
    m_scrollPinningBehavior = static_cast<ScrollPinningBehavior>(pinning);
    m_page->mainFrame().view()->setScrollPinningBehavior(m_scrollPinningBehavior);
}

void WebPage::setScrollbarOverlayStyle(std::optional<uint32_t> scrollbarStyle)
{
    if (scrollbarStyle)
        m_scrollbarOverlayStyle = static_cast<ScrollbarOverlayStyle>(scrollbarStyle.value());
    else
        m_scrollbarOverlayStyle = std::optional<ScrollbarOverlayStyle>();
    m_page->mainFrame().view()->recalculateScrollbarOverlayStyle();
}

Ref<DocumentLoader> WebPage::createDocumentLoader(Frame& frame, const ResourceRequest& request, const SubstituteData& substituteData)
{
    Ref<WebDocumentLoader> documentLoader = WebDocumentLoader::create(request, substituteData);

    documentLoader->setLastNavigationWasAppInitiated(m_lastNavigationWasAppInitiated);

    if (frame.isMainFrame()) {
        if (m_pendingNavigationID) {
            documentLoader->setNavigationID(m_pendingNavigationID);
            m_pendingNavigationID = 0;
        }

        if (m_pendingWebsitePolicies) {
            m_allowsContentJavaScriptFromMostRecentNavigation = m_pendingWebsitePolicies->allowsContentJavaScript;
            WebsitePoliciesData::applyToDocumentLoader(WTFMove(*m_pendingWebsitePolicies), documentLoader);
            m_pendingWebsitePolicies = std::nullopt;
        }
    }

    return documentLoader;
}

void WebPage::updateCachedDocumentLoader(WebDocumentLoader& documentLoader, Frame& frame)
{
    if (m_pendingNavigationID && frame.isMainFrame()) {
        documentLoader.setNavigationID(m_pendingNavigationID);
        m_pendingNavigationID = 0;
    }
}

void WebPage::getBytecodeProfile(CompletionHandler<void(const String&)>&& callback)
{
    if (LIKELY(!commonVM().m_perBytecodeProfiler))
        return callback({ });

    String result = commonVM().m_perBytecodeProfiler->toJSON();
    ASSERT(result.length());
    callback(result);
}

void WebPage::getSamplingProfilerOutput(CompletionHandler<void(const String&)>&& callback)
{
#if ENABLE(SAMPLING_PROFILER)
    SamplingProfiler* samplingProfiler = commonVM().samplingProfiler();
    if (!samplingProfiler)
        return callback({ });

    StringPrintStream result;
    samplingProfiler->reportTopFunctions(result);
    samplingProfiler->reportTopBytecodes(result);
    callback(result.toString());
#else
    callback({ });
#endif
}

void WebPage::didChangeScrollOffsetForFrame(Frame* frame)
{
    if (!frame->isMainFrame())
        return;

    // If this is called when tearing down a FrameView, the WebCore::Frame's
    // current FrameView will be null.
    if (!frame->view())
        return;

    updateMainFrameScrollOffsetPinning();
}

void WebPage::postMessage(const String& messageName, API::Object* messageBody)
{
    send(Messages::WebPageProxy::HandleMessage(messageName, UserData(WebProcess::singleton().transformObjectsToHandles(messageBody))));
}

void WebPage::postMessageIgnoringFullySynchronousMode(const String& messageName, API::Object* messageBody)
{
    send(Messages::WebPageProxy::HandleMessage(messageName, UserData(WebProcess::singleton().transformObjectsToHandles(messageBody))), IPC::SendOption::IgnoreFullySynchronousMode);
}

void WebPage::postSynchronousMessageForTesting(const String& messageName, API::Object* messageBody, RefPtr<API::Object>& returnData)
{
    UserData returnUserData;

    auto& webProcess = WebProcess::singleton();
    if (!sendSync(Messages::WebPageProxy::HandleSynchronousMessage(messageName, UserData(webProcess.transformObjectsToHandles(messageBody))), Messages::WebPageProxy::HandleSynchronousMessage::Reply(returnUserData), Seconds::infinity(), IPC::SendSyncOption::UseFullySynchronousModeForTesting))
        returnData = nullptr;
    else
        returnData = webProcess.transformHandlesToObjects(returnUserData.object());
}

void WebPage::clearWheelEventTestMonitor()
{
    if (!m_page)
        return;

    m_page->clearWheelEventTestMonitor();
}

void WebPage::setShouldScaleViewToFitDocument(bool shouldScaleViewToFitDocument)
{
    if (!m_drawingArea)
        return;

    m_drawingArea->setShouldScaleViewToFitDocument(shouldScaleViewToFitDocument);
}

void WebPage::imageOrMediaDocumentSizeChanged(const IntSize& newSize)
{
    send(Messages::WebPageProxy::ImageOrMediaDocumentSizeChanged(newSize));
}

void WebPage::addUserScript(String&& source, InjectedBundleScriptWorld& world, WebCore::UserContentInjectedFrames injectedFrames, WebCore::UserScriptInjectionTime injectionTime)
{
    WebCore::UserScript userScript { WTFMove(source), URL(aboutBlankURL()), Vector<String>(), Vector<String>(), injectionTime, injectedFrames, WebCore::WaitForNotificationBeforeInjecting::No };

    m_userContentController->addUserScript(world, WTFMove(userScript));
}

void WebPage::addUserStyleSheet(const String& source, WebCore::UserContentInjectedFrames injectedFrames)
{
    WebCore::UserStyleSheet userStyleSheet {source, aboutBlankURL(), Vector<String>(), Vector<String>(), injectedFrames, UserStyleUserLevel };

    m_userContentController->addUserStyleSheet(InjectedBundleScriptWorld::normalWorld(), WTFMove(userStyleSheet));
}

void WebPage::removeAllUserContent()
{
    m_userContentController->removeAllUserContent();
}

void WebPage::updateIntrinsicContentSizeIfNeeded(const WebCore::IntSize& size)
{
    m_pendingIntrinsicContentSize = std::nullopt;
    if (!minimumSizeForAutoLayout().width() && !sizeToContentAutoSizeMaximumSize().width() && !sizeToContentAutoSizeMaximumSize().height())
        return;
    ASSERT(mainFrameView());
    ASSERT(mainFrameView()->isFixedWidthAutoSizeEnabled() || mainFrameView()->isSizeToContentAutoSizeEnabled());
    ASSERT(!mainFrameView()->needsLayout());
    if (m_lastSentIntrinsicContentSize == size)
        return;
    m_lastSentIntrinsicContentSize = size;
    send(Messages::WebPageProxy::DidChangeIntrinsicContentSize(size));
}

void WebPage::flushPendingIntrinsicContentSizeUpdate()
{
    if (auto pendingSize = std::exchange(m_pendingIntrinsicContentSize, std::nullopt))
        updateIntrinsicContentSizeIfNeeded(*pendingSize);
}

void WebPage::scheduleIntrinsicContentSizeUpdate(const IntSize& size)
{
    if (!minimumSizeForAutoLayout().width() && !sizeToContentAutoSizeMaximumSize().width() && !sizeToContentAutoSizeMaximumSize().height())
        return;
    ASSERT(mainFrameView());
    ASSERT(mainFrameView()->isFixedWidthAutoSizeEnabled() || mainFrameView()->isSizeToContentAutoSizeEnabled());
    ASSERT(!mainFrameView()->needsLayout());
    m_pendingIntrinsicContentSize = size;
}

void WebPage::dispatchDidReachLayoutMilestone(OptionSet<WebCore::LayoutMilestone> milestones)
{
    RefPtr<API::Object> userData;
    injectedBundleLoaderClient().didReachLayoutMilestone(*this, milestones, userData);

    // Clients should not set userData for this message, and it won't be passed through.
    ASSERT(!userData);

    // The drawing area might want to defer dispatch of didLayout to the UI process.
    if (m_drawingArea) {
        static auto paintMilestones = OptionSet<WebCore::LayoutMilestone> { DidHitRelevantRepaintedObjectsAreaThreshold, DidFirstFlushForHeaderLayer, DidFirstPaintAfterSuppressedIncrementalRendering, DidRenderSignificantAmountOfText, DidFirstMeaningfulPaint };   
        auto drawingAreaRelatedMilestones = milestones & paintMilestones;
        if (drawingAreaRelatedMilestones && m_drawingArea->addMilestonesToDispatch(drawingAreaRelatedMilestones))
            milestones.remove(drawingAreaRelatedMilestones);
    }
    if (milestones.contains(DidFirstLayout) && mainFrameView()) {
        // Ensure we never send DidFirstLayout milestone without updating the intrinsic size.
        updateIntrinsicContentSizeIfNeeded(mainFrameView()->autoSizingIntrinsicContentSize());
    }

    send(Messages::WebPageProxy::DidReachLayoutMilestone(milestones));
}

void WebPage::didRestoreScrollPosition()
{
    send(Messages::WebPageProxy::DidRestoreScrollPosition());
}

void WebPage::setUserInterfaceLayoutDirection(uint32_t direction)
{
    m_userInterfaceLayoutDirection = static_cast<WebCore::UserInterfaceLayoutDirection>(direction);
    m_page->setUserInterfaceLayoutDirection(m_userInterfaceLayoutDirection);
}

#if ENABLE(POINTER_LOCK)
void WebPage::didAcquirePointerLock()
{
    corePage()->pointerLockController().didAcquirePointerLock();
}

void WebPage::didNotAcquirePointerLock()
{
    corePage()->pointerLockController().didNotAcquirePointerLock();
}

void WebPage::didLosePointerLock()
{
    corePage()->pointerLockController().didLosePointerLock();
}
#endif

void WebPage::didGetLoadDecisionForIcon(bool decision, CallbackID loadIdentifier, CompletionHandler<void(const IPC::SharedBufferDataReference&)>&& completionHandler)
{
    auto* documentLoader = corePage()->mainFrame().loader().documentLoader();
    if (!documentLoader)
        return completionHandler({ });

    documentLoader->didGetLoadDecisionForIcon(decision, loadIdentifier.toInteger(), [completionHandler = WTFMove(completionHandler)] (WebCore::SharedBuffer* iconData) mutable {
        completionHandler({ iconData });
    });
}

void WebPage::setUseIconLoadingClient(bool useIconLoadingClient)
{
    static_cast<WebFrameLoaderClient&>(corePage()->mainFrame().loader().client()).setUseIconLoadingClient(useIconLoadingClient);
}

WebURLSchemeHandlerProxy* WebPage::urlSchemeHandlerForScheme(const String& scheme)
{
    return m_schemeToURLSchemeHandlerProxyMap.get(scheme);
}

void WebPage::stopAllURLSchemeTasks()
{
    HashSet<WebURLSchemeHandlerProxy*> handlers;
    for (auto& handler : m_schemeToURLSchemeHandlerProxyMap.values())
        handlers.add(handler.get());

    for (auto* handler : handlers)
        handler->stopAllTasks();
}

void WebPage::registerURLSchemeHandler(uint64_t handlerIdentifier, const String& scheme)
{
    WEBPAGE_RELEASE_LOG(Process, "registerURLSchemeHandler: Registered handler %" PRIu64 " for the '%s' scheme", handlerIdentifier, scheme.utf8().data());
    WebCore::LegacySchemeRegistry::registerURLSchemeAsHandledBySchemeHandler(scheme);
    WebCore::LegacySchemeRegistry::registerURLSchemeAsCORSEnabled(scheme);
    auto schemeResult = m_schemeToURLSchemeHandlerProxyMap.add(scheme, WebURLSchemeHandlerProxy::create(*this, handlerIdentifier));
    m_identifierToURLSchemeHandlerProxyMap.add(handlerIdentifier, schemeResult.iterator->value.get());
}

void WebPage::urlSchemeTaskWillPerformRedirection(uint64_t handlerIdentifier, uint64_t taskIdentifier, ResourceResponse&& response, ResourceRequest&& request, CompletionHandler<void(WebCore::ResourceRequest&&)>&& completionHandler)
{
    auto* handler = m_identifierToURLSchemeHandlerProxyMap.get(handlerIdentifier);
    ASSERT(handler);

    auto actualNewRequest = request;
    handler->taskDidPerformRedirection(taskIdentifier, WTFMove(response), WTFMove(request), WTFMove(completionHandler));
}

void WebPage::urlSchemeTaskDidPerformRedirection(uint64_t handlerIdentifier, uint64_t taskIdentifier, ResourceResponse&& response, ResourceRequest&& request)
{
    auto* handler = m_identifierToURLSchemeHandlerProxyMap.get(handlerIdentifier);
    ASSERT(handler);

    handler->taskDidPerformRedirection(taskIdentifier, WTFMove(response), WTFMove(request), [] (ResourceRequest&&) {});
}
    
void WebPage::urlSchemeTaskDidReceiveResponse(uint64_t handlerIdentifier, uint64_t taskIdentifier, const ResourceResponse& response)
{
    auto* handler = m_identifierToURLSchemeHandlerProxyMap.get(handlerIdentifier);
    ASSERT(handler);

    handler->taskDidReceiveResponse(taskIdentifier, response);
}

void WebPage::urlSchemeTaskDidReceiveData(uint64_t handlerIdentifier, uint64_t taskIdentifier, const IPC::DataReference& data)
{
    auto* handler = m_identifierToURLSchemeHandlerProxyMap.get(handlerIdentifier);
    ASSERT(handler);

    handler->taskDidReceiveData(taskIdentifier, data.size(), data.data());
}

void WebPage::urlSchemeTaskDidComplete(uint64_t handlerIdentifier, uint64_t taskIdentifier, const ResourceError& error)
{
    auto* handler = m_identifierToURLSchemeHandlerProxyMap.get(handlerIdentifier);
    ASSERT(handler);

    handler->taskDidComplete(taskIdentifier, error);
}

void WebPage::setIsSuspended(bool suspended)
{
    if (m_isSuspended == suspended)
        return;

    m_isSuspended = suspended;

    if (!suspended)
        return;

    // Unfrozen on drawing area reset.
    freezeLayerTree(LayerTreeFreezeReason::PageSuspended);

    // Only the committed WebPage gets application visibility notifications from the UIProcess, so make sure
    // we don't hold a BackgroundApplication freeze reason when transitioning from committed to suspended.
    unfreezeLayerTree(LayerTreeFreezeReason::BackgroundApplication);

    WebProcess::singleton().sendPrewarmInformation(mainWebFrame().url());

    suspendForProcessSwap();
}

void WebPage::frameBecameRemote(FrameIdentifier frameID, GlobalFrameIdentifier&& remoteFrameIdentifier, GlobalWindowIdentifier&& remoteWindowIdentifier)
{
    RefPtr<WebFrame> frame = WebProcess::singleton().webFrame(frameID);
    if (!frame)
        return;

    if (frame->page() != this)
        return;

    auto* coreFrame = frame->coreFrame();
    auto* previousWindow = coreFrame->window();
    if (!previousWindow)
        return;

    auto remoteFrame = RemoteFrame::create(WTFMove(remoteFrameIdentifier));
    auto remoteWindow = RemoteDOMWindow::create(remoteFrame.copyRef(), WTFMove(remoteWindowIdentifier));

    remoteFrame->setOpener(frame->coreFrame()->loader().opener());

    auto jsWindowProxies = frame->coreFrame()->windowProxy().releaseJSWindowProxies();
    remoteFrame->windowProxy().setJSWindowProxies(WTFMove(jsWindowProxies));
    remoteFrame->windowProxy().setDOMWindow(remoteWindow.ptr());

    coreFrame->setView(nullptr);
    coreFrame->willDetachPage();
    coreFrame->detachFromPage();

    if (frame->isMainFrame())
        close();
}

#if ENABLE(RESOURCE_LOAD_STATISTICS)
void WebPage::hasStorageAccess(RegistrableDomain&& subFrameDomain, RegistrableDomain&& topFrameDomain, WebFrame& frame, CompletionHandler<void(bool)>&& completionHandler)
{
    if (hasPageLevelStorageAccess(topFrameDomain, subFrameDomain)) {
        completionHandler(true);
        return;
    }

    WebProcess::singleton().ensureNetworkProcessConnection().connection().sendWithAsyncReply(Messages::NetworkConnectionToWebProcess::HasStorageAccess(WTFMove(subFrameDomain), WTFMove(topFrameDomain), frame.frameID(), m_identifier), WTFMove(completionHandler));
}

void WebPage::requestStorageAccess(RegistrableDomain&& subFrameDomain, RegistrableDomain&& topFrameDomain, WebFrame& frame, StorageAccessScope scope, CompletionHandler<void(WebCore::RequestStorageAccessResult)>&& completionHandler)
{
    WebProcess::singleton().ensureNetworkProcessConnection().connection().sendWithAsyncReply(Messages::NetworkConnectionToWebProcess::RequestStorageAccess(WTFMove(subFrameDomain), WTFMove(topFrameDomain), frame.frameID(), m_identifier, m_webPageProxyIdentifier, scope), [this, protectedThis = makeRef(*this), completionHandler = WTFMove(completionHandler), frame = makeRef(frame), pageID = m_identifier, frameID = frame.frameID()](RequestStorageAccessResult result) mutable {
        if (result.wasGranted == StorageAccessWasGranted::Yes) {
            switch (result.scope) {
            case StorageAccessScope::PerFrame:
                frame->frameLoaderClient()->setHasFrameSpecificStorageAccess({ frameID, pageID });
                break;
            case StorageAccessScope::PerPage:
                addDomainWithPageLevelStorageAccess(result.topFrameDomain, result.subFrameDomain);
                break;
            }
        }
        completionHandler(result);
    });
}

void WebPage::addDomainWithPageLevelStorageAccess(const RegistrableDomain& topLevelDomain, const RegistrableDomain& resourceDomain)
{
    m_domainsWithPageLevelStorageAccess.add(topLevelDomain, resourceDomain);
}

bool WebPage::hasPageLevelStorageAccess(const RegistrableDomain& topLevelDomain, const RegistrableDomain& resourceDomain) const
{
    auto it = m_domainsWithPageLevelStorageAccess.find(topLevelDomain);
    return it != m_domainsWithPageLevelStorageAccess.end() && it->value == resourceDomain;
}

void WebPage::clearPageLevelStorageAccess()
{
    m_domainsWithPageLevelStorageAccess.clear();
}

void WebPage::wasLoadedWithDataTransferFromPrevalentResource()
{
    auto* frame = mainFrame();
    if (!frame || !frame->document())
        return;

    frame->document()->wasLoadedWithDataTransferFromPrevalentResource();
}

void WebPage::didLoadFromRegistrableDomain(RegistrableDomain&& targetDomain)
{
    if (targetDomain != RegistrableDomain(mainWebFrame().url()))
        m_loadedSubresourceDomains.add(targetDomain);
}

void WebPage::getLoadedSubresourceDomains(CompletionHandler<void(Vector<RegistrableDomain>)>&& completionHandler)
{
    completionHandler(copyToVector(m_loadedSubresourceDomains));
}

void WebPage::clearLoadedSubresourceDomains()
{
    m_loadedSubresourceDomains.clear();
}

#endif

void WebPage::showShareSheet(ShareDataWithParsedURL& shareData, WTF::CompletionHandler<void(bool)>&& callback)
{
    sendWithAsyncReply(Messages::WebPageProxy::ShowShareSheet(WTFMove(shareData)), WTFMove(callback));
}

void WebPage::showContactPicker(const WebCore::ContactsRequestData& requestData, CompletionHandler<void(std::optional<Vector<WebCore::ContactInfo>>&&)>&& callback)
{
    sendWithAsyncReply(Messages::WebPageProxy::ShowContactPicker(requestData), WTFMove(callback));
}

WebCore::DOMPasteAccessResponse WebPage::requestDOMPasteAccess(const String& originIdentifier)
{
    auto response = WebCore::DOMPasteAccessResponse::DeniedForGesture;
#if PLATFORM(IOS_FAMILY)
    // FIXME: Computing and sending an autocorrection context is a workaround for the fact that autocorrection context
    // requests on iOS are currently synchronous in the web process. This allows us to immediately fulfill pending
    // autocorrection context requests in the UI process on iOS before handling the DOM paste request. This workaround
    // should be removed once <rdar://problem/16207002> is resolved.
    send(Messages::WebPageProxy::HandleAutocorrectionContext(autocorrectionContext()));
#endif
    sendSyncWithDelayedReply(Messages::WebPageProxy::RequestDOMPasteAccess(rectForElementAtInteractionLocation(), originIdentifier), Messages::WebPageProxy::RequestDOMPasteAccess::Reply(response));
    return response;
}

#if USE(SYSTEM_PREVIEW)
void WebPage::systemPreviewActionTriggered(WebCore::SystemPreviewInfo previewInfo, const String& message)
{
    auto* document = Document::allDocumentsMap().get(previewInfo.element.documentIdentifier);
    if (!document)
        return;

    auto pageID = document->pageID();
    if (!pageID || previewInfo.element.webPageIdentifier != pageID.value())
        return;

    document->dispatchSystemPreviewActionEvent(previewInfo, message);
}
#endif

#if ENABLE(ATTACHMENT_ELEMENT)

void WebPage::insertAttachment(const String& identifier, std::optional<uint64_t>&& fileSize, const String& fileName, const String& contentType, CompletionHandler<void()>&& callback)
{
    auto& frame = m_page->focusController().focusedOrMainFrame();
    frame.editor().insertAttachment(identifier, WTFMove(fileSize), fileName, contentType);
    callback();
}

void WebPage::updateAttachmentAttributes(const String& identifier, std::optional<uint64_t>&& fileSize, const String& contentType, const String& fileName, const IPC::DataReference& enclosingImageData, CompletionHandler<void()>&& callback)
{
    if (auto attachment = attachmentElementWithIdentifier(identifier)) {
        attachment->document().updateLayout();
        attachment->updateAttributes(WTFMove(fileSize), contentType, fileName);
        attachment->updateEnclosingImageWithData(contentType, SharedBuffer::create(enclosingImageData.data(), enclosingImageData.size()));
    }
    callback();
}

void WebPage::updateAttachmentIcon(const String& identifier, const ShareableBitmap::Handle& qlThumbnailHandle)
{
    if (auto attachment = attachmentElementWithIdentifier(identifier)) {
        if (RefPtr<ShareableBitmap> thumbnail = !qlThumbnailHandle.isNull() ? ShareableBitmap::create(qlThumbnailHandle) : nullptr)
            attachment->updateThumbnail(thumbnail->createImage());
    }
}

RefPtr<HTMLAttachmentElement> WebPage::attachmentElementWithIdentifier(const String& identifier) const
{
    // FIXME: Handle attachment elements in subframes too as well.
    auto* frame = mainFrame();
    if (!frame || !frame->document())
        return nullptr;

    return frame->document()->attachmentForIdentifier(identifier);
}

#endif // ENABLE(ATTACHMENT_ELEMENT)

#if ENABLE(APPLICATION_MANIFEST)
void WebPage::getApplicationManifest(CompletionHandler<void(const std::optional<WebCore::ApplicationManifest>&)>&& completionHandler)
{
    Document* mainFrameDocument = m_mainFrame->coreFrame()->document();
    DocumentLoader* loader = mainFrameDocument ? mainFrameDocument->loader() : nullptr;
    if (!loader)
        return completionHandler(std::nullopt);

    auto coreCallbackID = loader->loadApplicationManifest();
    if (!coreCallbackID)
        return completionHandler(std::nullopt);

    m_applicationManifestFetchCallbackMap.add(coreCallbackID, WTFMove(completionHandler));
}

void WebPage::didFinishLoadingApplicationManifest(uint64_t coreCallbackID, const std::optional<WebCore::ApplicationManifest>& manifest)
{
    if (auto callback = m_applicationManifestFetchCallbackMap.take(coreCallbackID))
        callback(manifest);
}
#endif // ENABLE(APPLICATION_MANIFEST)

void WebPage::updateCurrentModifierState(OptionSet<PlatformEvent::Modifier> modifiers)
{
    PlatformKeyboardEvent::setCurrentModifierState(modifiers);
}

#if !PLATFORM(IOS_FAMILY)

WebCore::IntRect WebPage::rectForElementAtInteractionLocation() const
{
    return { };
}

void WebPage::updateInputContextAfterBlurringAndRefocusingElementIfNeeded(Element&)
{
}

#endif // !PLATFORM(IOS_FAMILY)

void WebPage::setCanShowPlaceholder(const WebCore::ElementContext& elementContext, bool canShowPlaceholder)
{
    RefPtr<Element> element = elementForContext(elementContext);
    if (is<HTMLTextFormControlElement>(element))
        downcast<HTMLTextFormControlElement>(*element).setCanShowPlaceholder(canShowPlaceholder);
}

RefPtr<Element> WebPage::elementForContext(const ElementContext& elementContext) const
{
    if (elementContext.webPageIdentifier != m_identifier)
        return nullptr;

    auto* document = Document::allDocumentsMap().get(elementContext.documentIdentifier);
    if (!document)
        return nullptr;

    if (document->page() != m_page.get())
        return nullptr;

    return document->searchForElementByIdentifier(elementContext.elementIdentifier);
}

std::optional<WebCore::ElementContext> WebPage::contextForElement(WebCore::Element& element) const
{
    auto& document = element.document();
    if (!m_page || document.page() != m_page.get())
        return std::nullopt;

    auto frame = document.frame();
    if (!frame)
        return std::nullopt;

    return WebCore::ElementContext { element.boundingBoxInRootViewCoordinates(), m_identifier, document.identifier(), document.identifierForElement(element) };
}

void WebPage::startTextManipulations(Vector<WebCore::TextManipulationController::ExclusionRule>&& exclusionRules, CompletionHandler<void()>&& completionHandler)
{
    if (!m_page)
        return;

    auto mainDocument = makeRefPtr(m_page->mainFrame().document());
    if (!mainDocument)
        return;

    mainDocument->textManipulationController().startObservingParagraphs([webPage = makeWeakPtr(*this)] (Document& document, const Vector<WebCore::TextManipulationController::ManipulationItem>& items) {
        auto* frame = document.frame();
        if (!webPage || !frame || webPage->mainFrame() != frame)
            return;

        auto* webFrame = WebFrame::fromCoreFrame(*frame);
        if (!webFrame)
            return;

        webPage->send(Messages::WebPageProxy::DidFindTextManipulationItems(items));
    }, WTFMove(exclusionRules));
    // For now, we assume startObservingParagraphs find all paragraphs synchronously at once.
    completionHandler();
}

void completeTextManipulation(const Vector<WebCore::TextManipulationController::ManipulationItem>,
    CompletionHandler<void(const HashSet<WebCore::TextManipulationController::ItemIdentifier, WebCore::TextManipulationController::ManipulationFailure>&)>&&);

void WebPage::completeTextManipulation(const Vector<WebCore::TextManipulationController::ManipulationItem>& items,
    CompletionHandler<void(bool allFailed, const Vector<WebCore::TextManipulationController::ManipulationFailure>&)>&& completionHandler)
{
    if (!m_page) {
        completionHandler(true, { });
        return;
    }

    auto mainDocument = makeRefPtr(m_page->mainFrame().document());
    if (!mainDocument) {
        completionHandler(true, { });
        return;
    }

    auto* controller = mainDocument->textManipulationControllerIfExists();
    if (!controller) {
        completionHandler(true, { });
        return;
    }

    completionHandler(false, controller->completeManipulation(items));
}

PAL::SessionID WebPage::sessionID() const
{
    return WebProcess::singleton().sessionID();
}

bool WebPage::usesEphemeralSession() const
{
    return sessionID().isEphemeral();
}

void WebPage::configureLoggingChannel(const String& channelName, WTFLogChannelState state, WTFLogLevel level)
{
#if ENABLE(GPU_PROCESS)
    if (auto* gpuProcessConnection = WebProcess::singleton().existingGPUProcessConnection())
        gpuProcessConnection->configureLoggingChannel(channelName, state, level);
#endif

    send(Messages::WebPageProxy::ConfigureLoggingChannel(channelName, state, level));
}

#if !PLATFORM(COCOA)
void WebPage::getPDFFirstPageSize(WebCore::FrameIdentifier, CompletionHandler<void(WebCore::FloatSize)>&& completionHandler)
{
    completionHandler({ });
}

void WebPage::getProcessDisplayName(CompletionHandler<void(String&&)>&& completionHandler)
{
    completionHandler({ });
}

void WebPage::updateMockAccessibilityElementAfterCommittingLoad()
{
}
#endif

#if !PLATFORM(IOS_FAMILY) || !ENABLE(DRAG_SUPPORT)

void WebPage::didFinishLoadingImageForElement(WebCore::HTMLImageElement&)
{
}

#endif

#if ENABLE(TEXT_AUTOSIZING)
void WebPage::textAutoSizingAdjustmentTimerFired()
{
    m_page->recomputeTextAutoSizingInAllFrames();
}

void WebPage::textAutosizingUsesIdempotentModeChanged()
{
    if (!m_page->settings().textAutosizingUsesIdempotentMode())
        m_textAutoSizingAdjustmentTimer.stop();
}
#endif // ENABLE(TEXT_AUTOSIZING)

void WebPage::setOverriddenMediaType(const String& mediaType)
{
    if (mediaType == m_overriddenMediaType)
        return;

    m_overriddenMediaType = mediaType;
    m_page->setNeedsRecalcStyleInAllFrames();
}

void WebPage::updateCORSDisablingPatterns(Vector<String>&& patterns)
{
    if (!m_page)
        return;

    m_corsDisablingPatterns = WTFMove(patterns);
    synchronizeCORSDisablingPatternsWithNetworkProcess();
    m_page->setCORSDisablingPatterns(parseAndAllowAccessToCORSDisablingPatterns(m_corsDisablingPatterns));
}

void WebPage::synchronizeCORSDisablingPatternsWithNetworkProcess()
{
    // FIXME: We should probably have this mechanism done between UIProcess and NetworkProcess directly.
    WebProcess::singleton().ensureNetworkProcessConnection().connection().send(Messages::NetworkConnectionToWebProcess::SetCORSDisablingPatterns(m_identifier, m_corsDisablingPatterns), 0);
}

void WebPage::isPlayingMediaDidChange(WebCore::MediaProducer::MediaStateFlags state)
{
    send(Messages::WebPageProxy::IsPlayingMediaDidChange(state));
}

#if ENABLE(IMAGE_ANALYSIS)

void WebPage::requestTextRecognition(WebCore::Element& element, CompletionHandler<void(RefPtr<WebCore::Element>&&)>&& completion)
{
    if (!is<HTMLElement>(element)) {
        if (completion)
            completion({ });
        return;
    }

    auto htmlElement = makeRef(downcast<HTMLElement>(element));
    if (corePage()->hasCachedTextRecognitionResult(htmlElement.get())) {
        if (completion) {
            RefPtr<Element> imageOverlayHost;
            if (htmlElement->hasImageOverlay())
                imageOverlayHost = makeRefPtr(element);
            completion(WTFMove(imageOverlayHost));
        }
        return;
    }

    auto matchIndex = m_elementsPendingTextRecognition.findMatching([&] (auto& elementAndCompletionHandlers) {
        return elementAndCompletionHandlers.first == &element;
    });

    if (matchIndex != notFound) {
        if (completion)
            m_elementsPendingTextRecognition[matchIndex].second.append(WTFMove(completion));
        return;
    }

    auto* renderer = element.renderer();
    if (!is<RenderImage>(renderer)) {
        if (completion)
            completion({ });
        return;
    }

    auto& renderImage = downcast<RenderImage>(*renderer);
    auto bitmap = createShareableBitmap(renderImage, { std::nullopt, AllowAnimatedImages::No, UseSnapshotForTransparentImages::Yes });
    if (!bitmap) {
        if (completion)
            completion({ });
        return;
    }

    ShareableBitmap::Handle bitmapHandle;
    bitmap->createHandle(bitmapHandle);
    if (bitmapHandle.isNull()) {
        if (completion)
            completion({ });
        return;
    }

    Vector<CompletionHandler<void(RefPtr<Element>&&)>> completionHandlers;
    if (completion)
        completionHandlers.append(WTFMove(completion));
    m_elementsPendingTextRecognition.append({ makeWeakPtr(element), WTFMove(completionHandlers) });

    auto cachedImage = renderImage.cachedImage();
    auto imageURL = cachedImage ? element.document().completeURL(cachedImage->url().string()) : URL { };
    sendWithAsyncReply(Messages::WebPageProxy::RequestTextRecognition(WTFMove(imageURL), WTFMove(bitmapHandle)), [webPage = makeWeakPtr(*this), weakElement = makeWeakPtr(element)] (auto&& result) {
        auto protectedPage = makeRefPtr(webPage.get());
        if (!protectedPage)
            return;

        protectedPage->m_elementsPendingTextRecognition.removeAllMatching([&] (auto& elementAndCompletionHandlers) {
            auto& [element, completionHandlers] = elementAndCompletionHandlers;
            if (element)
                return false;

            for (auto& completionHandler : completionHandlers)
                completionHandler({ });
            return true;
        });

        auto protectedElement = makeRefPtr(weakElement.get());
        if (!protectedElement)
            return;

        auto& htmlElement = downcast<HTMLElement>(*protectedElement);
        htmlElement.updateWithTextRecognitionResult(result);

        auto matchIndex = protectedPage->m_elementsPendingTextRecognition.findMatching([&] (auto& elementAndCompletionHandlers) {
            return elementAndCompletionHandlers.first == &htmlElement;
        });

        if (matchIndex == notFound)
            return;

        auto imageOverlayHost = htmlElement.hasImageOverlay() ? makeRefPtr(htmlElement) : nullptr;
        for (auto& completionHandler : protectedPage->m_elementsPendingTextRecognition[matchIndex].second)
            completionHandler(imageOverlayHost.copyRef());

        protectedPage->m_elementsPendingTextRecognition.remove(matchIndex);
    });
}

void WebPage::updateWithTextRecognitionResult(const TextRecognitionResult& result, const ElementContext& context, const FloatPoint& location, CompletionHandler<void(TextRecognitionUpdateResult)>&& completionHandler)
{
    auto elementToUpdate = elementForContext(context);
    if (!is<HTMLElement>(elementToUpdate)) {
        completionHandler(TextRecognitionUpdateResult::NoText);
        return;
    }

    downcast<HTMLElement>(*elementToUpdate).updateWithTextRecognitionResult(result);
    auto hitTestResult = corePage()->mainFrame().eventHandler().hitTestResultAtPoint(roundedIntPoint(location), {
        HitTestRequest::Type::ReadOnly,
        HitTestRequest::Type::Active,
        HitTestRequest::Type::AllowVisibleChildFrameContentOnly,
    });

    auto nodeAtLocation = makeRefPtr(hitTestResult.innerNonSharedNode());
    auto updateResult = ([&] {
        if (!nodeAtLocation || nodeAtLocation->shadowHost() != elementToUpdate || !HTMLElement::isInsideImageOverlay(*nodeAtLocation))
            return TextRecognitionUpdateResult::NoText;

#if ENABLE(DATA_DETECTION)
        if (findDataDetectionResultElementInImageOverlay(location, downcast<HTMLElement>(*elementToUpdate)))
            return TextRecognitionUpdateResult::DataDetector;
#endif

        if (HTMLElement::isImageOverlayText(*nodeAtLocation))
            return TextRecognitionUpdateResult::Text;

        return TextRecognitionUpdateResult::NoText;
    })();

    completionHandler(updateResult);
}

#endif // ENABLE(IMAGE_ANALYSIS)

#if ENABLE(MEDIA_CONTROLS_CONTEXT_MENUS) && USE(UICONTEXTMENU)
void WebPage::showMediaControlsContextMenu(FloatRect&& targetFrame, Vector<MediaControlsContextMenuItem>&& items, CompletionHandler<void(MediaControlsContextMenuItem::ID)>&& completionHandler)
{
    sendWithAsyncReply(Messages::WebPageProxy::ShowMediaControlsContextMenu(WTFMove(targetFrame), WTFMove(items)), completionHandler);
}
#endif // ENABLE(MEDIA_CONTROLS_CONTEXT_MENUS) && USE(UICONTEXTMENU)

#if !PLATFORM(IOS_FAMILY)

void WebPage::animationDidFinishForElement(const WebCore::Element&)
{
}

#endif

#if ENABLE(APP_BOUND_DOMAINS)
void WebPage::setIsNavigatingToAppBoundDomain(std::optional<NavigatingToAppBoundDomain> isNavigatingToAppBoundDomain, WebFrame* frame)
{
    frame->setIsNavigatingToAppBoundDomain(isNavigatingToAppBoundDomain);
    
    m_navigationHasOccured = true;
}

void WebPage::notifyPageOfAppBoundBehavior()
{
    if (!m_navigationHasOccured && !m_limitsNavigationsToAppBoundDomains)
        send(Messages::WebPageProxy::SetHasExecutedAppBoundBehaviorBeforeNavigation());
}
#endif

#if ENABLE(GPU_PROCESS)
RemoteRenderingBackendProxy& WebPage::ensureRemoteRenderingBackendProxy()
{
    if (!m_remoteRenderingBackendProxy)
        m_remoteRenderingBackendProxy = RemoteRenderingBackendProxy::create(*this);
    return *m_remoteRenderingBackendProxy;
}
#endif

Vector<RefPtr<SandboxExtension>> WebPage::consumeSandboxExtensions(Vector<SandboxExtension::Handle>&& sandboxExtensions)
{
    Vector<RefPtr<SandboxExtension>> dragSandboxExtensions;
    for (auto& sandboxExtension : sandboxExtensions) {
        auto extension = SandboxExtension::create(WTFMove(sandboxExtension));
        if (!extension)
            continue;
        bool ok = extension->consume();
        ASSERT_UNUSED(ok, ok);
        dragSandboxExtensions.append(WTFMove(extension));
    }
    return dragSandboxExtensions;
}

void WebPage::revokeSandboxExtensions(Vector<RefPtr<SandboxExtension>>& sandboxExtensions)
{
    for (auto& sandboxExtension : sandboxExtensions)
        sandboxExtension->revoke();
    sandboxExtensions.clear();
}

#if ENABLE(APP_HIGHLIGHTS)
bool WebPage::createAppHighlightInSelectedRange(WebCore::CreateNewGroupForHighlight createNewGroup, WebCore::HighlightRequestOriginatedInApp requestOriginatedInApp)
{
    SetForScope<WebCore::CreateNewGroupForHighlight> highlightIsNewGroupScope { m_highlightIsNewGroup, createNewGroup };
    SetForScope<WebCore::HighlightRequestOriginatedInApp> highlightRequestOriginScope { m_highlightRequestOriginatedInApp, requestOriginatedInApp };

    auto document = makeRefPtr(m_page->focusController().focusedOrMainFrame().document());

    auto frame = makeRefPtr(document->frame());
    if (!frame)
        return false;

    auto selectionRange = frame->selection().selection().toNormalizedRange();
    if (!selectionRange)
        return false;

    document->appHighlightRegister().addAppHighlight(StaticRange::create(selectionRange.value()));
    document->appHighlightStorage().storeAppHighlight(StaticRange::create(selectionRange.value()));

    return true;
}

void WebPage::restoreAppHighlightsAndScrollToIndex(const Vector<SharedMemory::IPCHandle>&& memoryHandles, const std::optional<unsigned> index)
{
    auto document = makeRefPtr(m_page->focusController().focusedOrMainFrame().document());

    unsigned i = 0;
    for (const auto& ipcHandle : memoryHandles) {
        auto sharedMemory = SharedMemory::map(ipcHandle.handle, SharedMemory::Protection::ReadOnly);
        if (!sharedMemory)
            continue;

        document->appHighlightStorage().restoreAndScrollToAppHighlight(SharedBuffer::create(static_cast<const uint8_t*>(sharedMemory->data()), ipcHandle.dataSize), i == index ? ScrollToHighlight::Yes : ScrollToHighlight::No);
        i++;
    }
}

void WebPage::setAppHighlightsVisibility(WebCore::HighlightVisibility appHighlightVisibility)
{
    m_appHighlightsVisible = appHighlightVisibility;
    for (RefPtr<Frame> frame = m_mainFrame->coreFrame(); frame; frame = frame->tree().traverseNextRendered()) {
        if (auto document = makeRefPtr(frame->document()))
            document->appHighlightRegister().setHighlightVisibility(appHighlightVisibility);
    }
}

#endif

#if ENABLE(MEDIA_SESSION_COORDINATOR)
void WebPage::createMediaSessionCoordinator(const String& identifier, CompletionHandler<void(bool)>&& completionHandler)
{
    auto* document = m_mainFrame->coreFrame()->document();
    if (!document || !document->domWindow()) {
        completionHandler(false);
        return;
    }

    m_page->setMediaSessionCoordinator(RemoteMediaSessionCoordinator::create(*this, identifier));
    completionHandler(true);
}

void WebPage::invalidateMediaSessionCoordinator()
{
    m_page->invalidateMediaSessionCoordinator();
}
#endif

#if !PLATFORM(COCOA)
void WebPage::consumeNetworkExtensionSandboxExtensions(const Vector<SandboxExtension::Handle>&)
{
}
#endif

void WebPage::lastNavigationWasAppInitiated(CompletionHandler<void(bool)>&& completionHandler)
{
    completionHandler(mainFrame()->document()->loader()->lastNavigationWasAppInitiated());
}

void WebPage::scrollToRect(const WebCore::FloatRect& targetRect, const WebCore::FloatPoint& origin)
{
    mainFrameView()->setScrollPosition(IntPoint(targetRect.minXMinYCorner()));
}

} // namespace WebKit

#undef WEBPAGE_RELEASE_LOG
#undef WEBPAGE_RELEASE_LOG_ERROR
