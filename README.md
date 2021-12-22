
# Retro - Minimalistic WebKit Fork that aims to reduce its Attack Surface

This WebKit2GTK Fork aims to reduce the Attack Surface of a Webview, and
therefore tries to reduce its features that are abused by Fingerprinting
libraries to a minimum.

Think of this as a safe and secure Webview alternative which aims to still
allow ECMAScript to run, but with the idea that no malicious code can identify
your device or fingerprint the system it is running on.

As the APIs and their bindings/interfaces/native adapters are completely
removed from the codebase, there's no way they can be exploited or bypassed.


# Removed Web/ECMAScript APIs

- [x] Apple Pay API (`APPLE_PAY`, `ApplePay*`)
- [x] Bluetooth API
- [x] Gamepad API
- [x] Geolocation API (`ENABLE_GEOLOCATION`, `Geolocation*`, `Geoposition*`)
- [x] Geoclue Library (`geoclue`)
- [x] Payment Request API (`PAYMENT_REQUEST`, `Merchant*`, `Payer*`, `Payment*`, `Coupon*`)
- [x] PASSKIT Integration (`PASSKIT`)
- [x] DeviceOrientation API (`DEVICE_ORIENTATION`, `ORIENTATION_EVENTS`, `DeviceOrientation*`, `get_orientation`, `orientationChanged`, `onorientationchange`, `OrientationNotifier`, `DOM_WINDOW_PROP_ORIENTATION`, `orientation`)
- [x] DeviceMotion API (`DEVICE_ORIENTATION`, `DeviceMotion*`)
- [x] Accelerometer Sensor API
- [x] Gravity Sensor API
- [x] Gyroscope Sensor API
- [x] Magnetometer Sensor API

# Removed CSS APIs

(TBD)

# Planned Removals

- [ ] Device Pixel Ratio API
- [ ] Page Visibility API
- [ ] Pointer Events API
- [ ] OrientationSensor API
- [ ] Camera API
- [ ] Microphone API
- [ ] Pointer Lock API
- [ ] Display Capture API
- [ ] Media Capture API
- [ ] Web Authn API
- [ ] WebASM API
- [ ] WebGL API
- [ ] Web GPU API
- [ ] Web Midi API
- [ ] Web NFC API
- [ ] Web Notifications API
- [ ] Web VR API
- [ ] Web XR API
- [ ] Canvas API
- [ ] WebRTC API
- [ ] Web Fonts
- [ ] History API
- [ ] LocalStorage/SessionStorage API
- [ ] Web SQLite Database API
- [ ] Web Notifications API
- [ ] Web Performance API
- [ ] Web Workers API
- [ ] Service Worker API
- [ ] Window Popup API
- [ ] Window Alert API
- [ ] Speaker Selection API

- [ ] CSS Pointer Events API
- [ ] CSS Filters API (`FILTERS_LEVEL_2`)

- [ ] FTP Directory Support (`FTPDIR`)

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
