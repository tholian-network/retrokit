# -----------------------------------------------------------------------------
# Add module directories
# -----------------------------------------------------------------------------
if (NOT USE_SYSTEM_MALLOC)
    add_subdirectory(bmalloc)
endif ()

add_subdirectory(WTF)

if (USE_CAPSTONE)
    add_subdirectory(ThirdParty/capstone)
endif ()

add_subdirectory(JavaScriptCore)

if (ENABLE_API_TESTS)
    add_subdirectory(ThirdParty/gtest)
endif ()

if (USE_XDGMIME)
    add_subdirectory(ThirdParty/xdgmime)
endif ()

if (ENABLE_WEBINSPECTORUI)
    add_subdirectory(WebInspectorUI)
endif ()

if (ENABLE_WEBCORE)
    add_subdirectory(WebCore)
endif ()

if (ENABLE_WEBKIT_LEGACY)
    add_subdirectory(WebKitLegacy)
endif ()

if (ENABLE_WEBKIT)
    add_subdirectory(WebKit)
endif ()

WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()
