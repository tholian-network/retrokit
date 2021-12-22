
# Retro - Minimalistic WebKit Fork that aims to reduce its Attack Surface

This WebKit2GTK Fork aims to reduce the Attack Surface of a Webview, and
therefore tries to reduce its features that are abused by Fingerprinting
libraries to a minimum.


# Planned Removals

- [x] Apple Pay API (`APPLE_PAY`, `ApplePay*`)
- [x] Bluetooth API
- [x] Gamepad API
- [x] Geolocation API (`ENABLE_GEOLOCATION`, `Geolocation*`, `Geoposition*`)
- [x] Geoclue Library (`geoclue`)
- [x] Payment Request API (`PAYMENT_REQUEST`, `Merchant*`, `Payer*`, `Payment*`, `Coupon*`)
- [x] PASSKIT Integration (`PASSKIT`)
- [ ] Camera API
- [ ] Microphone API
- [ ] Gyroscope API
- [ ] Accelerometer API
- [ ] Magnetometer API
- [ ] Pointer Lock API
- [ ] Display Capture API
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
