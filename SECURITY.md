
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


# Removed Features

## Target Platforms

The platforms are basically uninteresting for high security environments, or are
legacy platforms that are no longer maintained.

- [x] Playstation Platform Support (`__SCE__`, `PLATFORM_PLAYSTATION`, `BPLATFORM_PLAYSTATION`, `PlayStation`, `playstation`, `sony`)
- [ ] Apple Watch Platform Support (`TARGET_OS_WATCH`, `BOS_WATCHOS`, `BPLATFORM_WATCHOS`)
- [ ] Apple TV Platform Support (`TARGET_OS_TV`, `BOS_APPLETV`, `BPLATFORM_APPLETV`)
- [ ] *Undecided* WebKit Platform for Embedded (`WPE`, `USE_WPE_RENDERER`)
- [ ] *Undecided* Developer Mode (`DEVELOPER_MODE`)


## Legacy Hacks

These legacy APIs that are no longer of relevance to a modern Web Browser.

- [x] Hardcoded Quirks and Hacks (`bbc.co.uk`, `microsoft.com`, `playstation.com`, `sony.com`, `sonyentertainment.com`)
- [ ] Netscape Plugin API (`NETSCAPE_PLUGIN_API`)


## Library Dependencies

These libraries are external dependencies which are unnecessary for a Web Browser
to function. If a dependency has been known to be exploited in the past or abused
to track a user's identity, it is listed here.

Dependencies that allow hardware fingerprinting or network fingerprinting are
removed as well.

- [x] Geoclue Library Integration (`geoclue`) which leaks your geolocation to websites.
- [ ] Libhyphen Library Integration (`libhyphen`, `USE_LIBHYPHEN`) - do we love spell-ing so much?
- [ ] Libsecret Library Integration (`libsecret`, `USE_LIBSECRET`) which automatically leaks stored credentials only by opening URLs with the same hostname.
- [ ] Libnotify Library Integration (`libnotify`, `USE_LIBNOTIFY`) which is just annoying.
- [ ] SystemD Library Integration (`libsystemd`, `USE_SYSTEMD`) which a Browser should never have a depdendency to.
- [ ] ARKIT Integration (`ARKIT`)
- [x] PASSKIT Integration (`PASSKIT`)


## ECMAScript APIs

These Web APIs are abused by fingerprinting libraries in order to identify hardware
specifications and/or the physical position of a computer. This is an accumulative
list of Web APIs that are removed.

- [x] Apple Pay API (`APPLE_PAY`, `ApplePay*`)
- [x] Bluetooth API
- [x] Gamepad API
- [x] Geolocation API (`ENABLE_GEOLOCATION`, `Geolocation*`, `Geoposition*`)
- [x] Payment Request API (`PAYMENT_REQUEST`, `Merchant*`, `Payer*`, `Payment*`, `Coupon*`)
- [x] DeviceOrientation API (`DEVICE_ORIENTATION`, `ORIENTATION_EVENTS`, `DeviceOrientation*`, `get_orientation`, `orientationChanged`, `onorientationchange`, `OrientationNotifier`, `DOM_WINDOW_PROP_ORIENTATION`, `orientation`)
- [x] DeviceMotion API (`DEVICE_ORIENTATION`, `DeviceMotion*`)
- [x] Accelerometer Sensor API
- [x] Gravity Sensor API
- [x] Gyroscope Sensor API
- [x] Magnetometer Sensor API
- [x] FTP Directory Support (`FTPDIR`, `FTPDirectory*`, `unknownFileSizeText`)
- [x] WebXR API (`WEBXR`, `OPENXR`, `WebXR*`, `XRSpatialTracking`, `xrCompatible`)
- [ ] Web API Statistics (`WEB_API_STATISTICS`)
- [ ] Encrypted Media API (`ENCRYPTED_MEDIA`, `LEGACY_ENCRYPTED_MEDIA`)
- [ ] Picture in Picture API (`PICTURE_IN_PICTURE_API`)
- [ ] Web RTC API (`WEB_RTC`, `RTC*`)
- [ ] Web NFC API
- [ ] Web Performance API
- [ ] Web Storage API
- [ ] Indexed Database API
- [ ] Canvas API (`OFFSCREEN_CANVAS`, `Canvas*`, `DOMConvertWebGL`, `JSANGLEInstancedArray`, `CanvasActivityRecord`)
- [ ] WebGL API (`WEBGL`, `WebGL*`, `JSANGLEInstancedArray`, `GraphicsContextGL*`, `HighPerformanceGraphicsUsageSampler`)
- [ ] GPU Process for WebGL (`GPU_PROCESS`)
- [ ] WebGL via Metal (`WEBGL_COMPATIBLE_METAL`)
- [ ] WebGPU API
- [ ] Media Capture API
- [ ] Screen Capture API (`getDisplayMedia`)
- [ ] Media Stream API (`getUserMedia`)
- [ ] Page Visibility API
- [ ] Pointer Lock API (`POINTER_LOCK`)
- [ ] Web Notifications API
- [ ] Window Alert Method (`window.alert()`)
- [ ] Window History Interface (`window.history`)
- [ ] Window Popups (`window.open()`)
- [ ] WASM / WebASM Support
- [ ] *Undecided* Web Midi API
- [ ] *Undecided* Web Workers API
- [ ] *Undecided* Service Worker API


## CSS Features

- [ ] CSS Filters
- [ ] CSS Pointer Events
- [ ] Enforce CSS Web Fonts Caching
- [ ] Device Pixel Ratio
- [ ] Disallow `overflow:hidden`

