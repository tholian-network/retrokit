
# RetroKit - Minimal WebKit Fork with reduced Attack Surface

This WebKit2GTK fork reduces the Attack Surface of a WebView, and therefore
reduces the set of Web APIs which are abused by fingerprinting libraries.

The purpose of this fork is to create a secure webview alternative that
can be embedded for high security (and cyber defense) environments which
assume a hostile network infrastructure that is controlled by an opposing
or malicious party.

Think of this as a modern WebKit fork that tries to keep up-to-date in
regards to HTML5/CSS4, but removes all features that can be (and were used)
for fingerprinting and tracking. As all the APIs and their bindings, interfaces,
and native adapters are removed completely from the codebase, there's no way
these can be bypassed or exploited.

As this is the WebView-side of a Web Browser's infrastructure, it is used
by the [Tholian Stealth](https://github.com/tholian-network/stealth) Browser,
which provides an extra layer of Security via a peer-to-peer TLS network and
statistical content verification infrastructure.


## Security Improvements

For an overview of the differences to upstream WebKit, take a look at the
[SECURITY.md](./SECURITY.md) file which documents the current progress of
removals and changed behaviours of Web APIs.


# Building

## ArchLinux

```bash
sudo pacman -S ruby base-devel \
	gstreamer gstreamer-base gstreamer-full gstreamer-codecparsers \
	libwebp openjpeg2 \
	woff2 lcms harfbuzz harfbuzz-icu \
	libwpe wpebackend-fdo;

# Remove all build artifacts
bash clean.sh;

# Create a Debug build
cmake -DPORT=GTK -DCMAKE_BUILD_TYPE=Debug -DENABLE_MINIBROWSER=ON -GNinja;

# Alternatively create a Release build
# cmake -DPORT=GTK -DCMAKE_BUILD_TYPE=Release -DENABLE_MINIBROWSER=ON -DCMAKE_INSTALL_SYSCONFDIR=/etc -DCMAKE_INSTALL_LOCALSTATEDIR=/var -DCMAKE_INSTALL_PREFIX=/usr -GNinja;

ninja;
```

The complete list of available options is described in the [Source/cmake/WebKitFeatures.cmake](/Source/cmake/WebKitFeatures.cmake) file.


# Debugging

A `Debug` build has support for the `WEBKIT_DEBUG` environment variable, which can activate
different logging channels (which in return are defined in [Logging.h](/Source/WebCore/platform/Logging.h)).

The log levels are in this order: `error`, `warning`, `info`, `debug`.

The shorthand `WEBKIT_DEBUG="all"` activates all Logging Channels, though it will spam
your TTY real quick. So it's hardly useful.

```bash
# Create a Debug build
cmake -DPORT=GTK -DCMAKE_BUILD_TYPE=Debug -DENABLE_MINIBROWSER=ON -GNinja;
ninja;

# Debug Network and Images Channel
export WEBKIT_DEBUG="Network=debug,Images=debug";
./bin/Minibrowser;
```


# License

The License of this project is identical to upstream, which means that it is
released under a mixture of BSD-2-Clause and LGPLv2.1.

- [JavaScriptCore](/Source/WebCore) is licensed under LGPLv2.1
- [WebCore](/Source/WebCore) is licensed under LGPLv2.1
- [WebKit](/Source/WebKit) is licensed under BSD-2-Clause

