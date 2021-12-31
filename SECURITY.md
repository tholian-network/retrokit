
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

- [x] Remove Playstation Platform Support (`__SCE__`, `PLATFORM_PLAYSTATION`, `BPLATFORM_PLAYSTATION`, `PlayStation`, `playstation`, `sony`)
- [ ] Remove Apple Watch Platform Support (`TARGET_OS_WATCH`, `BOS_WATCHOS`, `BPLATFORM_WATCHOS`)
- [ ] Remove Apple TV Platform Support (`TARGET_OS_TV`, `BOS_APPLETV`, `BPLATFORM_APPLETV`)
- [ ] Remove Solaris Platform Support
- [ ] Remove User Message Handlers Support (`USER_MESSAGE_HANDLERS`)
- [ ] Remove Wireless Playback Target Support (`WIRELESS_PLAYBACK_TARGET`)
- [ ] *Undecided* Remove WebDriver Support (`WEBDRIVER`, `WEBDRIVER_KEYBOARD_INTERACTIONS`, `WEBDRIVER_MOUSE_INTERACTIONS`, `WEBDRIVER_TOUCH_INTERACTIONS`, `WEBDRIVER_WHEEL_INTERACTIONS`)
- [ ] *Undecided* Remove WebKit Platform for Embedded (`WPE`, `USE_WPE_RENDERER`)
- [ ] *Undecided* Remove Remote Inspector Support (`REMOTE_INSPECTOR`)
- [ ] *Undecided* Remove Developer Mode (`DEVELOPER_MODE`)


## Legacy Hacks

These legacy APIs that are no longer of relevance to a modern Web Browser.

- [x] Fix `None` already being defined in `X11`
- [x] Remove `MEMMEM` hack in `StringExtras.h`
- [ ] Remove `STRNSTR` hack in `StringExtras.h`
- [x] Remove hardcoded Quirks for Akamai Media Player
- [x] Remove hardcoded Quirks for `bbc.co.uk` Media Player
- [x] Remove hardcoded Quirks for `espn.com`
- [x] Remove hardcoded Quirks for `facebook.com`
- [x] Remove hardcoded Quirks for `gizmodo.com`
- [x] Remove hardcoded Quirks for `kinja.com`
- [x] Remove hardcoded Quirks for `microsoft.com` and Microsoft Teams redirect links
- [x] Remove hardcoded Quirks for `mlb.com`
- [x] Remove hardcoded Quirks for `netflix.com`
- [x] Remove hardcoded Quirks for `nfl.com`
- [ ] Remove hardcoded Quirks for `twitter.com`
- [x] Remove hardcoded Quirks for `playstation.com`, `sony.com` and `sonyentertainment.com`
- [x] Remove hardcoded Quirks for `vimeo.com`
- [x] Remove hardcoded Quirks for `youtube.com`
- [x] Remove Modern Media Controls Hacks and Shadow DOM (`MODERN_MEDIA_CONTROLS`)
- [x] Remove Sleep Disablers (`SleepDisabler`, `SleepDisablerClient`, `sleepDisablers`, `shouldDisableSleep`, `isDisablingSleep`, `mediaPlayerShouldDisableSleep`, `elementIsBlockingDisplaySleep`)
- [ ] Remove System Sleep Listener (`SystemSleepListener`)
- [x] Remove Legacy Custom Protocol Manager Support (`LEGACY_CUSTOM_PROTOCOL_MANAGER`, `LegacyCustomProtocol`, `LegacyCustomProtocolManager`, `registerSchemeForLegacyCustomProtocol`, `unregisterSchemeForLegacyCustomProtocol`, `LegacySchemeRegistry`, `CustomProtocolManagerClient`)
- [ ] Remove Legacy Encrypted Media API (`LEGACY_ENCRYPTED_MEDIA`)
- [ ] Remove Legacy Netscape Plugin API (`NETSCAPE_PLUGIN_API`)
- [ ] Remove `AllowsInlineMediaPlayback` Hacks (`AllowsInlineMediaPlayback`, `MEDIA_PLAYBACK_ALLOWS_INLINE`) and default playback behaviour to no-autoplay.
- [ ] Remove `QuickTimePluginReplacement`
- [ ] Remove `YouTubePluginReplacement`
- [ ] Remove `bmalloc` and prefer system allocator (`USE_SYSTEM_MALLOC`)


## Library Dependencies

These libraries are external dependencies which are unnecessary for a Web Browser
to function. If a dependency has been known to be exploited in the past or abused
to track a user's identity, it is listed here.

Dependencies that allow hardware fingerprinting or network fingerprinting are
removed as well.

- [x] Remove Geoclue Library Integration (`geoclue`) which leaks your geolocation to websites.
- [ ] Remove Libhyphen Library Integration (`libhyphen`, `USE_LIBHYPHEN`) - do we love spell-ing so much?
- [ ] Remove Libsecret Library Integration (`libsecret`, `USE_LIBSECRET`) which automatically leaks stored credentials only by opening URLs with the same hostname.
- [ ] Remove Libnotify Library Integration (`libnotify`, `USE_LIBNOTIFY`) which is just annoying.
- [x] Remove SystemD Library Integration (`libsystemd`, `USE_SYSTEMD`, `JOURNALD`) which is just unnecessary.
- [ ] Remove ARKIT Integration (`ARKIT`)
- [x] Remove PASSKIT Integration (`PASSKIT`)


## ECMAScript APIs

These Web APIs are abused by fingerprinting libraries in order to identify hardware
specifications and/or the physical position of a computer. This is an accumulative
list of Web APIs that are removed.

- [x] Remove Apple Pay API (`APPLE_PAY`, `ApplePay*`)
- [x] Remove Bluetooth API
- [x] Remove FTP Directory Support (`FTPDIR`, `FTPDirectory*`, `unknownFileSizeText`)
- [x] Remove Gamepad API
- [x] Remove Geolocation API (`ENABLE_GEOLOCATION`, `Geolocation*`, `Geoposition*`)
- [x] Remove Payment Request API (`PAYMENT_REQUEST`, `Merchant*`, `Payer*`, `Payment*`, `Coupon*`)
- [x] Remove DeviceOrientation API (`DEVICE_ORIENTATION`, `ORIENTATION_EVENTS`, `DeviceOrientation*`, `get_orientation`, `orientationChanged`, `onorientationchange`, `OrientationNotifier`, `DOM_WINDOW_PROP_ORIENTATION`, `orientation`)
- [x] Remove DeviceMotion API (`DEVICE_ORIENTATION`, `DeviceMotion*`)
- [x] Remove WebXR API (`WEBXR`, `OPENXR`, `WebXR*`, `XRSpatialTracking`, `xrCompatible`)
- [x] Remove Web API Statistics (`WEB_API_STATISTICS`)
- [x] Remove Fullscreen API (`FULLSCREEN`, `TOGGLE_VIDEO_FULLSCREEN`, `FullScreenClient`, `FullScreenManagerProxy`, `*EnterFullScreen`, `*ExitFullScreen`, `*IsFullScreen`, `WebFullScreenManager`, `webkitfullscreenchange`, `webkitfullscreenerror`)
- [x] Remove Fullscreen Events (`allowfullscreen`, `webkitbeginfullscreen`, `webkitendfullscreen`, `webkitfullscreenchange`, `webkitfullscreenerror`)
- [x] Remove Picture in Picture API (`PICTURE_IN_PICTURE_API`, `operationMatchesPictureinPicturePseudoClass`, `EnterPictureInPictureEvent`, `picture-in-picture`, `PictureInPictureElement`)
- [x] Remove Video Presentation Mode (`VIDEO_PRESENTATION_MODE`, `ControlledElementID`, `EnhancedFullscreen`, `VideoPresentation`, `prepareForVideoFullscreen`, `webkitPresentationModeChanged`)
- [x] Remove iFrame `allowfullscreen` Property
- [ ] Remove Network Shareable Resource API (`SHAREABLE_RESOURCE`)
- [ ] Remove 3D Model Element API (`MODEL_ELEMENT`)
- [ ] Remove Encrypted Media API (`ENCRYPTED_MEDIA`, `LEGACY_ENCRYPTED_MEDIA`)
- [ ] Remove Encrypted Media Thunder API (`THUNDER`)
- [ ] Remove Web RTC API (`WEB_RTC`, `RTC*`)
- [ ] Remove Web NFC API
- [ ] Remove Web Performance API
- [ ] Remove Web Storage API
- [ ] Remove Indexed Database API
- [ ] Remove Canvas API (`OFFSCREEN_CANVAS`, `Canvas*`, `DOMConvertWebGL`, `JSANGLEInstancedArray`, `CanvasActivityRecord`)
- [ ] Remove WebGL API (`WEBGL`, `WebGL*`, `JSANGLEInstancedArray`, `GraphicsContextGL*`, `HighPerformanceGraphicsUsageSampler`)
- [ ] Remove WebGL2 API (`WEBGL2`)
- [ ] Remove GPU Process for WebGL (`GPU_PROCESS`)
- [ ] Remove WebGL via Metal (`WEBGL_COMPATIBLE_METAL`)
- [ ] Remove WebGPU API
- [ ] Remove Media Capture API
- [ ] Remove Screen Capture API (`getDisplayMedia`)
- [ ] Remove Media Stream API (`getUserMedia`)
- [ ] Remove Page Visibility API
- [ ] Remove Pointer Lock API (`POINTER_LOCK`)
- [ ] Remove Web Notifications API
- [ ] Remove Window Alert Method (`window.alert()`)
- [ ] Remove Window History Interface (`window.history`)
- [ ] Remove Window Popups (`window.open()`)
- [ ] Remove Window Resize Observer Support (`RESIZE_OBSERVER`)
- [ ] Remove WASM / WebASM Support
- [ ] *Undecided* Remove Custom Protocol Manager Support (`CustomProtocol`, `CustomProtocolManager`)
- [ ] *Undecided* Remove Web Midi API
- [ ] *Undecided* Remove Web Workers API
- [ ] *Undecided* Remove Service Worker API


## HTML Features

- [ ] Remove `preconnect` Support (`SERVER_PRECONNECT`)
- [ ] Remove `preload` Support
- [ ] Remove `prerender` Support


## CSS Features

- [x] Remove `:-webkit-animating-full-screen-transition`, `:-webkit-full-screen*`
- [ ] Remove CSS Filters
- [ ] Remove CSS Pointer Events
- [ ] Enforce CSS Web Fonts Caching
- [ ] Remove Device Pixel Ratio
- [ ] Disallow `overflow:hidden`

