
# Retro - Minimalistic WebKit Fork that aims to reduce its Attack Surface

This WebKit2GTK Fork aims to reduce the Attack Surface of a Webview, and
therefore tries to reduce its features that are abused by Fingerprinting
libraries to a minimum.

Think of this as a safe and secure Webview alternative which aims to still
allow ECMAScript to run, but with the idea that no malicious code can identify
your device or fingerprint the system it is running on.

As the APIs and their bindings/interfaces/native adapters are completely
removed from the codebase, there's no way they can be exploited or bypassed.


# Removed APIs

These APIs have been removed, as they require either a proprietary service
that is known to track you; or because they've been used in the past by libraries
like fingerprint.js to track/identify you as a unique user.

As fingerprinting libraries usually tries to identify hardware specifications,
all Web APIs that expose the local hardware or network setup are removed as well.

Some APIs are removed because they break User Experience in an annoying way
for the end-user, and are regularly abused in order to block user interaction
or in order to make the current website content interaction useless.

# Removed Target Platforms

- [x] Playstation Platform Support (`__SCE__`, `PLATFORM_PLAYSTATION`, `BPLATFORM_PLAYSTATION`, `PlayStation`, `playstation`, `sony`)
- [ ] Apple Watch Platform Support (`TARGET_OS_WATCH`, `BOS_WATCHOS`, `BPLATFORM_WATCHOS`)
- [ ] Apple TV Platform Support (`TARGET_OS_TV`, `BOS_APPLETV`, `BPLATFORM_APPLETV`)
- [ ] ARKIT Integration (`ARKIT`)
- [x] PASSKIT Integration (`PASSKIT`)

# Removed ECMAScript APIs

- [x] Hardcoded Quirks and Hacks (`bbc.co.uk`, `microsoft.com`, `playstation.com`, `sony.com`, `sonyentertainment.com`)
- [x] Apple Pay API (`APPLE_PAY`, `ApplePay*`)
- [x] Bluetooth API
- [x] Gamepad API
- [x] Geolocation API (`ENABLE_GEOLOCATION`, `Geolocation*`, `Geoposition*`)
- [x] Geoclue Library (`geoclue`)
- [x] Payment Request API (`PAYMENT_REQUEST`, `Merchant*`, `Payer*`, `Payment*`, `Coupon*`)
- [x] DeviceOrientation API (`DEVICE_ORIENTATION`, `ORIENTATION_EVENTS`, `DeviceOrientation*`, `get_orientation`, `orientationChanged`, `onorientationchange`, `OrientationNotifier`, `DOM_WINDOW_PROP_ORIENTATION`, `orientation`)
- [x] DeviceMotion API (`DEVICE_ORIENTATION`, `DeviceMotion*`)
- [x] Accelerometer Sensor API
- [x] Gravity Sensor API
- [x] Gyroscope Sensor API
- [x] Magnetometer Sensor API
- [x] FTP Directory Support (`FTPDIR`, `FTPDirectory*`, `unknownFileSizeText`)
- [x] WebXR API (`WEBXR`, `OPENXR`, `WebXR*`, `XRSpatialTracking`, `xrCompatible`)
- [ ] Encrypted Media API (`ENCRYPTED_MEDIA`, `LEGACY_ENCRYPTED_MEDIA`)
- [ ] Picture in Picture API (`PICTURE_IN_PICTURE_API`)
- [ ] Web NFC API
- [ ] Web Performance API
- [ ] Web RTC API
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
- [ ] Netscape Plugin Support
- [ ] Page Visibility API
- [ ] Pointer Lock API
- [ ] Web Notifications API
- [ ] Window Alert Method (`window.alert()`)
- [ ] Window History Interface (`window.history`)
- [ ] Window Popups (`window.open()`)
- [ ] WASM / WebASM Support

# Removed CSS Features

- [ ] CSS Filters
- [ ] CSS Pointer Events
- [ ] Enforce CSS Web Fonts Caching
- [ ] Device Pixel Ratio
- [ ] Disallow `overflow:hidden`

# Undecided

These features are undecided whether they are useful or not, and
while they aren't known to be used as a tracking mechanism right
now they could be potentially be abused for fingerprinting in
the future.

- [ ] Web Midi API
- [ ] Web Workers API
- [ ] Service Worker API


# Planned Removal of Compiler Flags

- `DEVELOPER_MODE`
- `WPE`, `USE_WPE_RENDERER`
- `USE_SYSTEMD` - WTF, a Browser needs systemd integration!?
- `USE_LIBSECRET` - GNOME Keyring
- `USE_LIBNOTIFY` - GNOME Notifications
- `USE_LIBHYPHEN`


# Building on Linux

```bash
sudo pacman -S ruby base-devel \
	gstreamer gstreamer-base gstreamer-full gstreamer-codecparsers \
	libwebp openjpeg2 \
	woff2 lcms harfbuzz harfbuzz-icu \
	libwpe wpebackend-fdo;
```

```bash
cmake -DPORT=GTK -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_MINIBROWSER=ON -GNinja;
ninja;
```
