
# Security Improvements

These APIs have been removed from the RetroKit fork, as they require either a
proprietary service that is known to track you; or because they've been used in
the past by libraries like fingerprint.js to track/identify you as a unique user.

As fingerprinting libraries usually try to identify hardware specifications and
correlate them (e.g. display resolution, network bandwidth, amount of screens or
GPU shader support), all Web APIs that expose the local hardware or network setup
are removed as well.

Some APIs are removed because they break User Experience in an annoying way
for the end-user, and are regularly abused in order to block user interaction
or in order to make the current website's content interaction useless.

This document also functions as a "roadmap" as it contains the list of APIs that
are going to be removed in the future as well. If a checklist item has been marked
as completed, it is no longer contained in the codebase; and/or doesn't represent
an Attack Surface for malicious parties anymore.

## Polyfills

As some APIs are expected to be there in a Web Browser, they will probably be
polyfilled to represent the "answers" a fingerprinting library expects (e.g.
Windows 10 as an environment running Microsoft Edge or Google Chrome).

At some point these APIs will be integrated via a Web Extension that polyfills
those expected APIs, providing a flexible turn-on/off style feature that allows
users to avoid fingerprinting by faking fingerprinting details with commonly
used hardware specifications.

(As of now, this Web Extension does not exist, so RetroKit itself will likely
be classified either as an outdated WebKit/Safari Browser or as a Web Scraper)

# Removed Features

## Target Platforms

The platforms are basically uninteresting for high security environments, or are
legacy platforms that are no longer maintained.

- [x] Remove AIX Support (`_AIX`)
- [x] Remove Apple Watch Platform Support (`TARGET_OS_WATCH`, `BOS_WATCHOS`, `BPLATFORM_WATCHOS`, `WATCHOS`, `__WATCH_OS_VERSION`, `PEPPER_UI_CORE`, `QUICKBOARD_CONTROLLER`, `PUIC_BUTTON_TYPE_PILL`, `SYMPTOMS_FRAMEWORK`, `WatchOS*`)
- [ ] Remove Apple TV Platform Support (`TARGET_OS_TV`, `BOS_APPLETV`, `BPLATFORM_APPLETV`, `TVOS`, `__TV_OS_VERSION`)
- [x] Remove BeOS Support (`BEOS`, `__BEOS__`)
- [ ] Remove Fuchsia OS Support (`FUCHSIA`)
- [x] Remove GNU/Hurd Platform Support (`HURD`, `__GNU__`)
- [x] Remove HP-UX Platform Support (`HPUX`)
- [ ] Remove iOS Platform Support (`IOS_FAMILY`, `IOS_FAMILY_SIMULATOR`)
- [x] Remove Playstation Platform Support (`__SCE__`, `PLATFORM_PLAYSTATION`, `BPLATFORM_PLAYSTATION`, `PlayStation`, `playstation`, `sony`)
- [x] Remove SGI IRIX Platform Support (`IRIX`)
- [x] Remove Solaris Platform Support (`sun`, `__sun`)
- [x] Remove Tru64 OSF/1 Platform Support (`__osf`)
- [x] Remove Touch Bar Support (`TOUCH_BAR`, `TouchBarMenuData`, `TouchBarMenuItemData`, `didInsertMenuElement`, `didRemoveMenuElement`, `didInsertMenuItemElement`, `didRemoveMenuItemElement`, `touchBarMenu*`)
- [ ] Remove User Message Handlers Support (`USER_MESSAGE_HANDLERS`)
- [x] Remove WebDriver Support (`WEBDRIVER`, `WEBDRIVER_KEYBOARD_INTERACTIONS`, `WEBDRIVER_MOUSE_INTERACTIONS`, `WEBDRIVER_TOUCH_INTERACTIONS`, `WEBDRIVER_WHEEL_INTERACTIONS`)
- [x] Remove Wireless Playback Target Support (`WIRELESS_PLAYBACK_TARGET`, `PlaybackTarget`, `RemotePlayback`)
- [ ] *Undecided* Remove WebKit Platform for Embedded (`WPE`, `USE_WPE_RENDERER`)
- [ ] *Undecided* Remove Remote Inspector Support (`REMOTE_INSPECTOR`)
- [ ] *Undecided* Remove Developer Mode (`DEVELOPER_MODE`)
- [ ] *Undecided* Refactor Repository Structure to contain all library dependencies in one place


## Legacy Hacks

These legacy APIs that are no longer of relevance to a modern Web View.

- [x] Fix `None` already being defined in `X11`
- [ ] Enforce modern HarfBuzz version, and remove legacy `OPENTYPE_MATH`
- [x] Remove `MEMMEM` hack in `StringExtras.h`
- [ ] Remove `STRNSTR` hack in `StringExtras.h`
- [ ] Remove `CoreFoundation`, and instead use internal data types (`USE_CF`, `CFSTR()`)
- [x] Remove Modern Media Controls Hacks and Shadow DOM (`MODERN_MEDIA_CONTROLS`)
- [x] Remove Sleep Disablers (`SleepDisabler`, `SleepDisablerClient`, `sleepDisablers`, `shouldDisableSleep`, `isDisablingSleep`, `mediaPlayerShouldDisableSleep`, `elementIsBlockingDisplaySleep`)
- [ ] Remove System Sleep Listener (`SystemSleepListener`)
- [x] Remove Legacy Custom Protocol Manager Support (`LEGACY_CUSTOM_PROTOCOL_MANAGER`, `LegacyCustomProtocol`, `LegacyCustomProtocolManager`, `registerSchemeForLegacyCustomProtocol`, `unregisterSchemeForLegacyCustomProtocol`, `LegacySchemeRegistry`, `CustomProtocolManagerClient`)
- [x] Remove Legacy Encrypted Media API (`LEGACY_ENCRYPTED_MEDIA`)
- [ ] Remove `WebFrameLoaderClient::createPlugin` and `WebFrameLoaderClient::redirectDataToPlugin`, `pluginSupportsExtension`
- [ ] Remove Sandbox Extensions (`SANDBOX_EXTENSIONS`, `SandboxExtension`)
- [ ] Remove Webkit InjectedBundle API (`InjectedBundle`, `WKBundle*`)
- [x] Remove WebKit WebAutomation API (`WebAutomation`, `WebKitAutomation`, `AutomationClient`, `AutomationSession`)
- [ ] Remove WebKit WebExtension API (`WebKitWebExtension`, `webkit_web_extension*`)
- [ ] Remove `AllowsInlineMediaPlayback` Hacks (`AllowsInlineMediaPlayback`, `MEDIA_PLAYBACK_ALLOWS_INLINE`) and default playback behaviour to no-autoplay.
- [ ] Remove `bmalloc` and prefer system allocator (`USE_SYSTEM_MALLOC`)
- [ ] *Undecided* Remove WebKit Content Extensions API (`CONTENT_EXTENSIONS`)
- [ ] *Undecided* Remove Arbitrary User Gestures (`ArbitraryUserGesture`)
- [ ] *Undecided* Remove Release Log (`RELEASE_LOG_DISABLED`)


## Legacy Plugins

Web Browser Plugins are using the old and outdated `navigator.plugins` API,
and are the #1 reason for a Web Browser to be exploited (Thanks, Adobe and
Skype for this lesson!). These APIs serve only as an unnecessary attack
surface, that's why they're removed.

- [x] Remove Legacy Netscape Plugin API (`NETSCAPE_PLUGIN_API`, `NPAPI`, `NPIdentifier`, `NPMethod`, `NPObject`, `NPRemote`, `NPRuntimeUtilities`, `NPRuntimeObjectMap`, `NPVariant`, `NetscapeBrowserFuncs`, `NetscapePlugin`, `PlugInData`, `PluginData.h`, `PluginProcess`, `WKContextSetAdditionalPluginsDirectory`, `WKContextRefreshPlugIns`, `canPluginHandleResponse`, `addSupportedPlugin`, `clearSupportedPlugin`)
- [x] Remove PDFKit Plugin Integration (`PDFKIT_PLUGIN`, `UI_PROCESS_PDF_HUD`, `PDFHUD`)
- [x] Remove IncrementalPDF Integration (`INCREMENTAL_PDF_APIS`, `IncrementalPDFLoading`)
- [x] Remove QuickTime Plugin Replacement (`QuickTimePlugin`)
- [x] Remove YouTube Flash Plugin Replacement (`YouTubePlugin`, `YouTubeEmbedShadowElement`)
- [x] Remove Java Plugin Support (`enable_java`, `JavaEnabled`)
- [x] Remove WebKit Plugin Support (`WebKitPlugin`, `WebKitMimeInfo`, `PluginsEnabled`)
- [x] Remove Plugin DOM API (`DOMMimeType`, `DOMMimeTypeArray`, `DOMPlugin`, `DOMPluginArray`)
- [x] Remove Plugin Injected Script and Stylesheet (`plugInExtraScript`, `plugInExtraStyleSheet`, `PlugInStartLabelTitle`, `PlugInStartLabelSubtitle`, `plugInsJavascript`, `ensurePlugInsInjectedScript`)
- [x] Modify Plugin Content Security Policies (`pluginContentIsAllowed`, `allowPluginContent`, `allowPluginType`)
- [x] Remove Plugin Document Wrappers (`PluginDocument`, `PluginLoadClientPolicy`, `PluginModuleLoadPolicy`)
- [x] Remove Plugin Renderer Integration (`RenderEmbeddedObject`)
- [x] Remove Translation UI Services Support (`TRANSLATION_UI_SERVICES`)


## Legacy Quirks

These legacy Quirks are really dirty Hacks that are domain-specific and site-specific
and are also no longer of relevance to a modern Web View.

- [x] Remove `needsSiteSpecificQuirks()`
- [x] Remove `shouldAutoplayForArbitraryUserGesture()` Quirk (`facebook.com`, `twitter.com`)
- [x] Remove `shouldAutoplayWebAudioForArbitraryUserGesture()` Quirk (`bing.com`, `zoom.us`)
- [x] Remove `hasBrokenEncryptedMediaAPISupportQuirk()` Quirk (`hulu.com`, `starz.com`, `youtube.com`)
- [x] Remove `shouldIgnoreInvalidSignal()` Quirk
- [x] Remove `needsFormControlToBeMouseFocusable()` Quirk (`ceac.state.gov`)
- [x] Remove `needsAutoplayPlayPauseEvents()` Quirk
- [x] Remove `needsSeekingSupportDisabled()` Quirk (`netflix.com`)
- [x] Remove `needsPerDocumentAutoplayBehavior()` Quirk (`netflix.com`)
- [x] Remove `shouldDisableContentChangeObserverTouchEventAdjustment()` Quirk (`youtube.com`)
- [x] Remove `shouldTooltipPreventFromProceedingWithClick()` Quirk (`covid.cdc.gov`)
- [x] Remove `shouldHideSearchFieldResultsButton()` Quirk (`google.com`)
- [x] Remove `needsMillisecondResolutionForHighResTimeStamp()` Quirk (`icourse163.org`)
- [x] Remove `shouldStripQuotationMarkInFontFaceSetFamily()` Quirk (`docs.google.com`)
- [x] Remove `isTouchBarUpdateSupressedForHiddenContentEditable()` Quirk (`docs.google.com`)
- [x] Remove `isNeverRichlyEditableForTouchBar()` Quirk (`icloud.com`, `onedrive.live.com`, `trix-editor.org`, `twitter.com`)
- [x] Remove `needsYouTubeMouseOutQuirk()` Quirk (`youtube.com`)
- [x] Remove `shouldAvoidUsingIOS13ForGmail()` Quirk (`mail.google.com`)
- [x] Remove `shouldSuppressAutocorrectionAndAutocaptializationInHiddenEditableAreas()` Quirk (`docs.google.com`)
- [x] Remove `shouldDispatchSyntheticMouseEventsWhenModifyingSelection()` Quirk (`medium.com`, `weebly.com`)
- [x] Remove `shouldDispatchSimulatedMouseEvents()` Quirk (`airtable.com`, `flipkart.com`, `figma.com`, `iqiyi.com`, `msn.com`, `mybinder.com, `nba.com`, `naver.com`, `soundcloud.com`, `trailers.apple.com`, `trello.com`
- [x] Remove `shouldDispatchedSimulatedMouseEventsAssumeDefaultPrevented()` Quirk (`soundcloud.com`)
- [x] Remove `simulatedMouseEventTypeForTarget()` Quirk (`airtable.com`, `desmos.com`)
- [x] Remove `shouldMakeTouchEventNonCancelableForTarget()` Quirk (`youtube.com`)
- [x] Remove `shouldPreventPointerMediaQueryFromEvaluatingToCoarse()` Quirk (`shutterstock.com`)
- [x] Remove `shouldPreventDispatchOfTouchEvent()` Quirk (`sites.google.com`)
- [x] Remove `shouldSynthesizeTouchEvents()` Quirk (`mail.yahoo.com`)
- [x] Remove `shouldAvoidResizingWhenInputViewBoundsChange()` Quirk (`google.com`, `live.com`, `sharepoint.com`, `twitter.com`)
- [x] Remove `shouldDisablePointerEventsQuirk()` Quirk (`mailchimp.com`)
- [x] Remove `needsDeferKeyDownAndKeyPressTimersUntilNextEditingCommand()` Quirk (`docs.google.com`)
- [x] Remove `needsInputModeNoneImplicitly()` Quirk (`calendar.google.com`)
- [x] Remove `needsGMailOverflowScrollQuirk()` Quirk (`mail.google.com`)
- [x] Remove `needsYouTubeOverflowScrollQuirk()` Quirk (`youtube.com`)
- [x] Remove `needsFullscreenDisplayNoneQuirk()` Quirk (`gizmodo.com`)
- [x] Remove `needsWeChatScrollingQuirk()` Quirk (`wechat.com`, WeChat iOS App)
- [x] Remove `shouldSilenceWindowResizeEvents()` Quirk (`nytimes.com`, `twitter.com`)
- [x] Remove `shouldSilenceMediaQueryListChangeEvents()` Quirk (`twitter.com`)
- [x] Remove `shouldAvoidScrollingWhenFocusedContentIsVisible()` Quirk (`zillow.com`)
- [x] Remove `shouldUseLegacySelectPopoverDismissalBehaviorInDataActivation()` Quirk (`att.com`)
- [x] Remove `shouldIgnoreAriaForFastPathContentObservationCheck()` Quirk (`ralphlauren.com`)
- [x] Remove `shouldOpenAsAboutBlank()` Quirk (`docs.google.com`, `srcdoc://`)
- [x] Remove `needsPreloadAutoQuirk()` Quirk (`vimeo.com`)
- [x] Remove `shouldBypassBackForwardCache()` Quirk (`docs.google.com`, `vimeo.com`)
- [x] Remove `shouldBypassAsyncScriptDeferring()` Quirk (`bungalow.com`)
- [x] Remove `shouldMakeEventListenerPassive()` Quirk (`SmoothScroll.js`)
- [x] Remove `shouldEnableLegacyGetUserMediaQuirk()` Quirk (`baidu.com`, `warbyparker.com`)
- [x] Remove `shouldDisableElementFullscreenQuirk()` Quirk (`nfl.com`)
- [x] Remove `needsCanPlayAfterSeekedQuirk()` Quirk (`hulu.com`)
- [x] Remove `shouldLayOutAtMinimumWindowWidthWhenIgnoringScalingConstraints()` Quirk (`wikipedia.org`)
- [x] Remove `shouldIgnoreContentObservationForSyntheticClick()` Quirk (`shutterstock.com`)
- [x] Remove `shouldAvoidPastingImagesAsWebContent()` Quirk (`mail.yahoo.com`)
- [x] Remove `isMicrosoftTeamsRedirectURL()` Quirk (`teams.microsoft.com`)
- [x] Remove `isStorageAccessQuirkDomainAndElement` Quirk (`outlook.live.com`, `playstation.com`)
- [x] Remove `isBBCDomain`, `BBCRadioPlayerURLString()` and `staticRadioPlayerURLString()` Quirk (`bbc.co.uk`)
- [x] Remove `triggerOptionalStorageAccessQuirk()` Quirk (`avclub.com`, `gizmodo.com`, `deadspin.com`, `jalopnik.com`, `jezebel.com`, `kotaku.com`, `lifehacker.com`, `theroot.com`, `thetakeout.com`, `theonion.com`, `theinventory.com`)
- [x] Remove `needsVP9FullRangeFlagQuirk()` Quirk (`youtube.com`)
- [x] Remove `needsHDRPixelDepthQuirk()` Quirk (`youtube.com`)
- [x] Remove `needsAkamaiMediaPlayerQuirk()` Quirk
- [x] Remove `needsBlackFullscreenBackgroundQuirk()` Quirk (`mlb.com`)
- [x] Remove `requiresUserGestureToPauseInPictureInPicture()` Quirk (`facebook.com`, `reddit.com`, `twitter.com`)
- [x] Remove `requiresUserGestureToLoadInPictureInPicture()` Quirk (`twitter.com`)
- [x] Remove `blocksReturnToFullscreenFromPictureInPictureQuirk()` Quirk (`vimeo.com`)
- [x] Remove `shouldDisableEndFullscreenEventWhenEnteringPictureInPictureFromFullscreenQuirk()` Quirk (`trailers.apple.com`, `espn.com`)
- [x] Remove `needsToForceUserSelectAndUserDragWhenInstallingImageOverlay()` Quirk (`youtube.com`)
- [x] Remove `hasStorageAccessForAllLoginDomains()` Quirk
- [x] Remove `requestStorageAccessAndHandleClick()` Quirk
- [x] Remove `shouldOmitHTMLDocumentSupportedPropertyNames()` Quirk
- [x] Remove `shouldBypassUserGestureRequirementForWebAuthn` Quirk (`dropbox.com`, `facebook.com`, `google.com`, `microsoft.com`, `twitter.com`)


## Legacy User-Agent Quirks

These legacy Quirks are really dirty Hacks that are domain-specific and site-specific
and are also no longer of relevance to a modern Web View.

- [x] Remove `quirksForURL()` Quirk
- [x] Remove `stringForQuirk()` Quirk
- [x] Remove `urlRequiresChromeBrowser()` Quirk
- [x] Remove `urlRequiresFirefoxBrowser()` Quirk
- [x] Remove `urlRequiresMacintoshPlatform()` Quirk
- [x] Remove `urlRequiresUnbrandedUserAgent()` Quirk


## Library Dependencies

These libraries are external dependencies which are unnecessary for a Web Browser
to function. If a dependency has been known to be exploited in the past or abused
to track a user's identity, it is listed here.

Dependencies that allow hardware fingerprinting or network fingerprinting are
removed as well.

- [x] Update Libsoup Integration to version `2.99.99` and later
- [x] Remove `SOUP2`
- [ ] Switch to cURL (Remove `USE_SOUP` and insert `USE_CURL` from Windows Platform)
- [x] Remove Geoclue Integration (`geoclue`) which leaks your geolocation to websites
- [ ] Remove libhyphen Integration (`LIBHYPTHEN`, `libhyphen`)
- [x] Remove libsecret Integration (`LIBSECRET`, `libsecret`, `secret.h`)
- [x] Remove libnotify Integration (`LIBNOTIFY`, `libnotify`, `notify.h`)
- [x] Remove libwebrtc Integration (`LIBWEBRTC`, `libWebRTC`, `libwebrtc`)
- [x] Remove SystemD Integration (`libsystemd`, `USE_SYSTEMD`, `JOURNALD`)
- [x] Remove ARKIT Integration (`ARKIT_INLINE_PREVIEW`)
- [x] Remove Quick Look Integration (`QUICK_LOOK`, `ARKIT_QUICK_LOOK_PREVIEW`)
- [x] Remove PASSKIT Integration (`PASSKIT`)


## ECMAScript APIs

These Web APIs are abused by fingerprinting libraries in order to identify hardware
specifications and/or the physical position of a computer. This is an accumulative
list of Web APIs that are removed.

- [x] Remove Apple Pay API (`APPLE_PAY`, `ApplePay`)
- [x] Remove Bluetooth API
- [x] Remove FTP Directory Support (`FTPDIR`, `FTPDirectory`, `unknownFileSizeText`)
- [x] Remove Gamepad API
- [x] Remove Geolocation API (`ENABLE_GEOLOCATION`, `Geolocation`, `Geoposition`)
- [x] Remove Payment Request API (`PAYMENT_REQUEST`, `Merchant`, `Payer`, `Payment`, `Coupon`)
- [x] Remove DeviceOrientation API (`DEVICE_ORIENTATION`, `ORIENTATION_EVENTS`, `DeviceOrientation`, `get_orientation`, `orientationChanged`, `onorientationchange`, `OrientationNotifier`, `DOM_WINDOW_PROP_ORIENTATION`, `orientation`)
- [x] Remove DeviceMotion API (`DEVICE_ORIENTATION`, `DeviceMotion`)
- [x] Remove WebXR API (`WEBXR`, `OPENXR`, `WebXR`, `XRSpatialTracking`, `xrCompatible`)
- [x] Remove Web API Statistics (`WEB_API_STATISTICS`)
- [x] Remove Fullscreen API (`FULLSCREEN`, `TOGGLE_VIDEO_FULLSCREEN`, `FullScreenClient`, `FullScreenManagerProxy`, `EnterFullScreen`, `ExitFullScreen`, `IsFullScreen`, `WebFullScreenManager`, `webkitfullscreenchange`, `webkitfullscreenerror`)
- [x] Remove Fullscreen Events (`allowfullscreen`, `webkitbeginfullscreen`, `webkitendfullscreen`, `webkitfullscreenchange`, `webkitfullscreenerror`)
- [x] Remove 3D Model Element API (`MODEL_ELEMENT`, `HTMLModelElement`, `ModelElement`)
- [x] Remove Picture in Picture API (`PICTURE_IN_PICTURE_API`, `operationMatchesPictureinPicturePseudoClass`, `EnterPictureInPictureEvent`, `picture-in-picture`, `PictureInPictureElement`)
- [x] Remove Video Presentation Mode (`VIDEO_PRESENTATION_MODE`, `ControlledElementID`, `EnhancedFullscreen`, `VideoPresentation`, `prepareForVideoFullscreen`, `webkitPresentationModeChanged`)
- [x] Remove iFrame `allowfullscreen` Property
- [x] Remove Encrypted Media API (`ENCRYPTED_MEDIA`, `LEGACY_ENCRYPTED_MEDIA`)
- [x] Remove Encrypted Media Thunder API (`THUNDER`)
- [x] Remove Media Capture API (`MEDIA_CAPTURE`, `MediaCapture`)
- [x] Remove Media Recorder API (`MediaRecorder`)
- [x] Remove Media Statistics API (`MEDIA_STATISTICS`, `webkitAudioDecodedByteCount`, `webkitVideoDecodedByteCount`, `webkitDecodedFrameCount`, `webkitDroppedFrameCount`)
- [x] Remove Media Stream API (`MEDIA_STREAM`, `getUserMedia`, `MediaDevices`, `MediaStream`, `MediaStreamTrack`, `UserMediaPermissionRequest`)
- [x] Remove Media Usage API (`MEDIA_USAGE`, `MediaUsageManager`)
- [x] Remove Speech Recognition API (`SPEECHRECOGNIZER`, `SpeechRecognizer`, `SpeechRecognition`, `SpeechSynthesizer`)
- [x] Remove Speech Synthesis API (`SPEECH_SYNTHESIS`, `SpeechSynthesis`)
- [x] Remove WebGL API (`WEBGL`, `WebGL`, `GraphicsContextGL`)
- [x] Remove WebGL2 API (`WEBGL2`, `WebGLTransformFeedback`, `WebGLVertexArrayObject`)
- [x] Remove WebGL via Metal API (`WEBGL_COMPATIBLE_METAL`)
- [x] Remove Web RTC API (`WEB_RTC`, `RTC`, `PeerConnectionState`, `PeerMediaDescription`, `RTCBundle`, `RTCDataChannel`, `RTCPeer`, `RTCIce`, `createRTCProvider`)
- [x] Remove Web RTC H264 Codec (`WebRTCH264LowLatencyCoderEnabled`, `WebRTCH264HardwareEncoderEnabled`, `WebRTCH264SimulcastEnabled`)
- [x] Remove Web RTC H265 Codec (`WebRTCH256CodecEnabled`)
- [x] Remove Web RTC DTMF Codec (`WebRTCDTMFEnabled`)
- [x] Remove Web RTC Insertable Streams (`WebRTCInsertableStreamsEnabled`)
- [x] Remove Web RTC Platform Codecs (`WebRTCPlatformCodecsInGPUProcessEnabled`)
- [x] Remove Web RTC Platform Sockets (`WebRTCPlatformTCPSocketsEnabled`, `WebRTCPlatformUDPSocketsEnabled`)
- [x] Remove Web RTC Sockets Proxying (`WebRTCSocketsProxyingEnabled`)
- [x] Remove Web RTC VP9 Codec Profiles (`WebRTCVP9Profile0CodecEnabled`, `WebRTCVP9Profile2CodecEnabled`)
- [x] Remove Web RTC MDNS ICE Candidates Support (`WebRTCMDNSICECandidatesEnabled`, `PeerConnectionEnabled`, `ICECandidateFiltering`)
- [ ] Remove Contact Picker API (`ContactPickerAPI`, `ContactInfo`, `ContactProperty`, `ContactsManager`, `ContactsRequestData`, `ContactsSelectOption`, `navigator.contacts`)
- [ ] Remove Media Session API (`MEDIA_SESSION`, `MediaSessionManager`)
- [ ] Remove Media Capabilities API (`MediaCapabilities`)
- [ ] Remove Offsceen Canvas in Workers (`OFFSCREEN_CANVAS_IN_WORKERS`)
- [ ] Remove Network Shareable Resource API (`SHAREABLE_RESOURCE`)
- [ ] Remove Web Assembly API (`WEBASSEMBLY`)
- [ ] Remove Web NFC API
- [ ] Remove Web Performance API
- [ ] Remove Web Storage API
- [ ] Remove Indexed Database API (`IDBFactory`, `IDBOpenDBRequest`, `IDBDatabase`, `IDBTransaction`, `IDBRequest`, `IDBObjectStore`, `IDBIndex`, `IDBCursor`, `IDBKeyRange`, `IDBLocaleAwareKeyRange`, `IDBVersionChangeEvent`, `IDBDatabaseException`, `IDBTransactionSync`, `IDBObjectStoreSync`, `IDBIndexSync`, `IDBFactorySync`, `IDBEnvironmentSync`, `IDBDatabaseSync`, `IDBCursorSync`)
- [ ] Remove Canvas API (`OFFSCREEN_CANVAS`, `Canvas`, `DOMConvertWebGL`, `JSANGLEInstancedArray`, `CanvasActivityRecord`)
- [ ] Remove WebGPU API
- [ ] Remove Page Visibility API
- [ ] Remove Pointer Lock API (`POINTER_LOCK`)
- [ ] Remove Web Notifications API
- [ ] Remove Window Alert Method (`window.alert()`)
- [ ] Remove Window History Interface (`window.history`)
- [ ] Remove Window Popups (`window.open()`)
- [ ] Remove Intersection Observer API (`INTERSECTION_OBSERVER`, `IntersectionObserver`)
- [ ] Remove Performance Observer API (`PerformanceObserver`)
- [ ] Remove Resize Observer API (`RESIZE_OBSERVER`, `ResizeObserver`)
- [ ] *Undecided* Remove Custom Protocol Manager Support (`CustomProtocol`, `CustomProtocolManager`)
- [ ] *Undecided* Remove Web Midi API
- [ ] *Undecided* Remove Web Workers API
- [ ] *Undecided* Remove Service Worker API (`SERVICE_WORKER`)
- [ ] *Undecided* Remove Content Change Observer (`CONTENT_CHANGE_OBSERVER`)
- [ ] *Undecided* Remove App Bound Domains Support (`APP_BOUND_DOMAINS`)
- [ ] Remove WebSQL Database API (`WebSQL`)


## Web Workers and Worklets

- [ ] Remove Worklet API (`Worklet`, `WorkletGlobalScope`, `WorkletPendingTask`)
- [ ] Remove AudioWorklet API
- [ ] Remove PaintWorklet API


## HTML Features

- [ ] Remove `preconnect` Support (`SERVER_PRECONNECT`)
- [ ] Remove `preload` Support
- [ ] Remove `prerender` Support
- [x] Remove `<plug-in>` (`PlugInElement`, `HTMLPlugInElement`, `JSPluginElement`, `jsObjectForPluginElement`)
- [x] Remove `<applet>` Element (`appletTag`, `AppletElement`, `HTMLAppletElement`)
- [x] Remove `<embed>` Element (`embedTag`, `EmbedElement`, `HTMLEmbedElement`, `EmbeddedObject`, `RenderEmbeddedObject`)
- [x] Remove `<object>` Element (`objectTag`, `ObjectElement`, `HTMLObjectElement`)
- [ ] Remove `<foreignObject>` SVG Element (`foreignObjectTag`)


## CSS Features

- [x] Remove `:-webkit-animating-full-screen-transition`, `:-webkit-full-screen-...`
- [ ] Remove CSS Filters
- [ ] Remove CSS Pointer Events
- [ ] Enforce CSS Web Fonts Caching
- [ ] Remove Device Pixel Ratio
- [ ] Disallow `overflow:hidden`

