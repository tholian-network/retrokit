include(WebKitCommon)

set_property(DIRECTORY . PROPERTY FOLDER "WebKit")

set(WebKit_PRIVATE_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
)

set(WebKit_INCLUDE_DIRECTORIES
    "${WebCore_PRIVATE_FRAMEWORK_HEADERS_DIR}"
    "${WebKit_FRAMEWORK_HEADERS_DIR}"
    "${WebKit_DERIVED_SOURCES_DIR}"
    "${WebKit_DERIVED_SOURCES_DIR}/include"
    "${WEBKIT_DIR}"
    "${WEBKIT_DIR}/GPUProcess"
    "${WEBKIT_DIR}/GPUProcess/graphics"
    "${WEBKIT_DIR}/GPUProcess/media"
    "${WEBKIT_DIR}/NetworkProcess"
    "${WEBKIT_DIR}/NetworkProcess/Classifier"
    "${WEBKIT_DIR}/NetworkProcess/Cookies"
    "${WEBKIT_DIR}/NetworkProcess/Downloads"
    "${WEBKIT_DIR}/NetworkProcess/FileAPI"
    "${WEBKIT_DIR}/NetworkProcess/IndexedDB"
    "${WEBKIT_DIR}/NetworkProcess/PrivateClickMeasurement"
    "${WEBKIT_DIR}/NetworkProcess/ServiceWorker"
    "${WEBKIT_DIR}/NetworkProcess/WebStorage"
    "${WEBKIT_DIR}/NetworkProcess/cache"
    "${WEBKIT_DIR}/NetworkProcess/watchos"
    "${WEBKIT_DIR}/Platform"
    "${WEBKIT_DIR}/Platform/IPC"
    "${WEBKIT_DIR}/Shared"
    "${WEBKIT_DIR}/Shared/API"
    "${WEBKIT_DIR}/Shared/API/c"
    "${WEBKIT_DIR}/Shared/Authentication"
    "${WEBKIT_DIR}/Shared/CoreIPCSupport"
    "${WEBKIT_DIR}/Shared/Databases"
    "${WEBKIT_DIR}/Shared/Databases/IndexedDB"
    "${WEBKIT_DIR}/Shared/FileAPI"
    "${WEBKIT_DIR}/Shared/RemoteLayerTree"
    "${WEBKIT_DIR}/Shared/WebsiteData"
    "${WEBKIT_DIR}/UIProcess"
    "${WEBKIT_DIR}/UIProcess/API"
    "${WEBKIT_DIR}/UIProcess/API/C"
    "${WEBKIT_DIR}/UIProcess/API/cpp"
    "${WEBKIT_DIR}/UIProcess/Authentication"
    "${WEBKIT_DIR}/UIProcess/Downloads"
    "${WEBKIT_DIR}/UIProcess/GPU"
    "${WEBKIT_DIR}/UIProcess/Inspector"
    "${WEBKIT_DIR}/UIProcess/Inspector/Agents"
    "${WEBKIT_DIR}/UIProcess/Launcher"
    "${WEBKIT_DIR}/UIProcess/Media"
    "${WEBKIT_DIR}/UIProcess/Network"
    "${WEBKIT_DIR}/UIProcess/Notifications"
    "${WEBKIT_DIR}/UIProcess/RemoteLayerTree"
    "${WEBKIT_DIR}/UIProcess/UserContent"
    "${WEBKIT_DIR}/UIProcess/WebAuthentication"
    "${WEBKIT_DIR}/UIProcess/WebAuthentication/Mock"
    "${WEBKIT_DIR}/UIProcess/WebsiteData"
    "${WEBKIT_DIR}/WebAuthnProcess"
    "${WEBKIT_DIR}/WebProcess"
    "${WEBKIT_DIR}/WebProcess/ApplicationCache"
    "${WEBKIT_DIR}/WebProcess/Cache"
    "${WEBKIT_DIR}/WebProcess/Databases"
    "${WEBKIT_DIR}/WebProcess/Databases/IndexedDB"
    "${WEBKIT_DIR}/WebProcess/FileAPI"
    "${WEBKIT_DIR}/WebProcess/FullScreen"
    "${WEBKIT_DIR}/WebProcess/GPU"
    "${WEBKIT_DIR}/WebProcess/GPU/graphics"
    "${WEBKIT_DIR}/WebProcess/GPU/media"
    "${WEBKIT_DIR}/WebProcess/IconDatabase"
    "${WEBKIT_DIR}/WebProcess/InjectedBundle"
    "${WEBKIT_DIR}/WebProcess/InjectedBundle/API"
    "${WEBKIT_DIR}/WebProcess/InjectedBundle/API/c"
    "${WEBKIT_DIR}/WebProcess/InjectedBundle/DOM"
    "${WEBKIT_DIR}/WebProcess/Inspector"
    "${WEBKIT_DIR}/WebProcess/Launching"
    "${WEBKIT_DIR}/WebProcess/MediaCache"
    "${WEBKIT_DIR}/WebProcess/Network"
    "${WEBKIT_DIR}/WebProcess/Notifications"
    "${WEBKIT_DIR}/WebProcess/OriginData"
    "${WEBKIT_DIR}/WebProcess/ResourceCache"
    "${WEBKIT_DIR}/WebProcess/Storage"
    "${WEBKIT_DIR}/WebProcess/UserContent"
    "${WEBKIT_DIR}/WebProcess/WebCoreSupport"
    "${WEBKIT_DIR}/WebProcess/WebPage"
    "${WEBKIT_DIR}/WebProcess/WebPage/RemoteLayerTree"
    "${WEBKIT_DIR}/WebProcess/WebStorage"
    "${CMAKE_SOURCE_DIR}/Source"
    "${CMAKE_SOURCE_DIR}/WebKitLibraries"
)

list(APPEND WebKit_UNIFIED_SOURCE_LIST_FILES
    "Sources.txt"
)

set(WebKit_MESSAGES_IN_FILES
    GPUProcess/GPUConnectionToWebProcess
    GPUProcess/GPUProcess

    GPUProcess/graphics/RemoteRenderingBackend

    GPUProcess/media/RemoteAudioDestinationManager
    GPUProcess/media/RemoteAudioSessionProxy
    GPUProcess/media/RemoteMediaEngineConfigurationFactoryProxy
    GPUProcess/media/RemoteMediaPlayerManagerProxy
    GPUProcess/media/RemoteMediaPlayerProxy
    GPUProcess/media/RemoteMediaResourceManager
    GPUProcess/media/RemoteMediaSourceProxy
    GPUProcess/media/RemoteRemoteCommandListenerProxy
    GPUProcess/media/RemoteSourceBufferProxy

    NetworkProcess/Cookies/WebCookieManager

    NetworkProcess/IndexedDB/WebIDBServer

    NetworkProcess/NetworkBroadcastChannelRegistry
    NetworkProcess/NetworkConnectionToWebProcess
    NetworkProcess/NetworkContentRuleListManager
    NetworkProcess/NetworkProcess
    NetworkProcess/NetworkResourceLoader
    NetworkProcess/NetworkSocketChannel
    NetworkProcess/NetworkSocketStream

    NetworkProcess/ServiceWorker/ServiceWorkerFetchTask
    NetworkProcess/ServiceWorker/WebSWServerConnection
    NetworkProcess/ServiceWorker/WebSWServerToContextConnection

    NetworkProcess/WebStorage/StorageManagerSet

    NetworkProcess/cache/CacheStorageEngineConnection

    Shared/AuxiliaryProcess
    Shared/WebConnection

    Shared/Authentication/AuthenticationManager

    UIProcess/DrawingAreaProxy

    UIProcess/Media/RemoteMediaSessionCoordinatorProxy

    UIProcess/VisitedLinkStore
    UIProcess/WebCookieManagerProxy
    UIProcess/WebPageProxy
    UIProcess/WebPasteboardProxy
    UIProcess/WebProcessPool
    UIProcess/WebProcessProxy

    UIProcess/Downloads/DownloadProxy

    UIProcess/GPU/GPUProcessProxy

    UIProcess/Inspector/RemoteWebInspectorUIProxy
    UIProcess/Inspector/WebInspectorUIProxy

    UIProcess/Network/NetworkProcessProxy

    UIProcess/UserContent/WebUserContentControllerProxy

    WebProcess/WebProcess

    WebProcess/Databases/IndexedDB/WebIDBConnectionToServer

    WebProcess/GPU/GPUProcessConnection

    WebProcess/GPU/graphics/RemoteRenderingBackendProxy

    WebProcess/GPU/media/RemoteAudioHardwareListener
    WebProcess/GPU/media/MediaPlayerPrivateRemote
    WebProcess/GPU/media/MediaSourcePrivateRemote
    WebProcess/GPU/media/RemoteAudioSession
    WebProcess/GPU/media/RemoteAudioSourceProviderManager
    WebProcess/GPU/media/RemoteRemoteCommandListener
    WebProcess/GPU/media/SourceBufferPrivateRemote

    WebProcess/Inspector/RemoteWebInspectorUI
    WebProcess/Inspector/WebInspector
    WebProcess/Inspector/WebInspectorInterruptDispatcher
    WebProcess/Inspector/WebInspectorUI

    WebProcess/MediaSession/RemoteMediaSessionCoordinator

    WebProcess/Network/NetworkProcessConnection
    WebProcess/Network/WebResourceLoader
    WebProcess/Network/WebSocketChannel
    WebProcess/Network/WebSocketStream

    WebProcess/Notifications/WebNotificationManager

    WebProcess/Storage/WebSWClientConnection
    WebProcess/Storage/WebSWContextManagerConnection

    WebProcess/UserContent/WebUserContentController

    WebProcess/WebCoreSupport/WebBroadcastChannelRegistry

    WebProcess/WebPage/DrawingArea
    WebProcess/WebPage/EventDispatcher
    WebProcess/WebPage/VisitedLinkTableController
    WebProcess/WebPage/WebPage

    WebProcess/WebStorage/StorageAreaMap

)

set(WebKit_FRAMEWORKS
    JavaScriptCore
    PAL
    WTF
    WebCore
)
if (NOT USE_SYSTEM_MALLOC)
    list(APPEND WebKit_FRAMEWORKS bmalloc)
endif ()

set(WebProcess_LIBRARIES WebKit)
set(NetworkProcess_LIBRARIES WebKit)
set(GPUProcess_LIBRARIES WebKit)

# For platforms that want to use system-provided OpenGL (ES) / EGL headers,
# these include directories, libraries or definitions need to be
# added before the ANGLE directories.
if (USE_ANGLE_EGL)
    # Prepend to make sure the ANGLE headers are found before system headers
    list(PREPEND WebKit_PRIVATE_LIBRARIES
        ANGLE::GLES
        ANGLE::EGL
    )
else ()
    if (USE_OPENGL)
        list(APPEND WebKit_SYSTEM_INCLUDE_DIRECTORIES
            ${OPENGL_INCLUDE_DIRS}
        )
        list(APPEND WebKit_PRIVATE_LIBRARIES
            ${OPENGL_LIBRARIES}
        )
        add_definitions(${OPENGL_DEFINITIONS})
    elseif (USE_OPENGL_ES)
        list(APPEND WebKit_PRIVATE_LIBRARIES OpenGL::GLES)
    endif ()

    if (USE_EGL)
        list(APPEND WebKit_SYSTEM_INCLUDE_DIRECTORIES
            ${EGL_INCLUDE_DIRS}
        )
        list(APPEND WebKit_PRIVATE_LIBRARIES
            ${EGL_LIBRARIES}
        )
        add_definitions(${EGL_DEFINITIONS})
    endif ()
endif ()

if (ATOMICS_REQUIRE_LIBATOMIC)
    list(APPEND WebKit_PRIVATE_LIBRARIES atomic)
endif ()

if (UNIX)
    check_function_exists(shm_open SHM_OPEN_EXISTS)
    if (NOT SHM_OPEN_EXISTS)
        set(CMAKE_REQUIRED_LIBRARIES rt)
        check_function_exists(shm_open SHM_OPEN_REQUIRES_LIBRT)
        if (SHM_OPEN_REQUIRES_LIBRT)
            list(APPEND WebKit_PRIVATE_LIBRARIES rt)
        endif ()
        unset(CMAKE_REQUIRED_LIBRARIES)
    endif ()
endif ()

macro(ADD_WEBKIT_PREFIX_HEADER _target)
    if (WebKit_USE_PREFIX_HEADER)
        get_target_property(OLD_COMPILE_FLAGS ${_target} COMPILE_FLAGS)
        if (${OLD_COMPILE_FLAGS} STREQUAL "OLD_COMPILE_FLAGS-NOTFOUND")
            set(OLD_COMPILE_FLAGS "")
        endif ()
        set_target_properties(${_target} PROPERTIES COMPILE_FLAGS "${OLD_COMPILE_FLAGS} -include ${WEBKIT_DIR}/WebKit2Prefix.h")
    endif ()
endmacro()

set(WebKit_AUTOMATION_PROTOCOL_GENERATOR_EXTRA_FLAGS)

set(WebKit_INTERFACE_LIBRARIES WebKit)
set(WebKit_INTERFACE_INCLUDE_DIRECTORIES
    ${WebKit_FRAMEWORK_HEADERS_DIR}
    ${WebKit_PRIVATE_FRAMEWORK_HEADERS_DIR}
)
set(WebKit_INTERFACE_DEPENDENCIES
    NetworkProcess
    WebProcess
)

WEBKIT_EXECUTABLE_DECLARE(WebProcess)
WEBKIT_EXECUTABLE_DECLARE(NetworkProcess)

if (ENABLE_GPU_PROCESS)
    WEBKIT_EXECUTABLE_DECLARE(GPUProcess)
    list(APPEND WebKit_INTERFACE_DEPENDENCIES GPUProcess)
endif ()

WEBKIT_FRAMEWORK_DECLARE(WebKit)
WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

if (WebKit_PUBLIC_FRAMEWORK_HEADERS)
    WEBKIT_COPY_FILES(WebKit_CopyHeaders
        DESTINATION ${WebKit_FRAMEWORK_HEADERS_DIR}/WebKit
        FILES ${WebKit_PUBLIC_FRAMEWORK_HEADERS}
        FLATTENED
    )
    list(APPEND WebKit_DEPENDENCIES WebKit_CopyHeaders)
endif ()

if (WebKit_PRIVATE_FRAMEWORK_HEADERS)
    WEBKIT_COPY_FILES(WebKit_CopyPrivateHeaders
        DESTINATION ${WebKit_PRIVATE_FRAMEWORK_HEADERS_DIR}/WebKit
        FILES ${WebKit_PRIVATE_FRAMEWORK_HEADERS}
        FLATTENED
    )
    list(APPEND WebKit_INTERFACE_DEPENDENCIES WebKit_CopyPrivateHeaders)
endif ()

# Helper macro which wraps the generate-message-receiver.py script
#   _output_source is a list name which will contain generated sources.(eg. WebKit_SOURCES)
#   _inputs are messages.in files to generate.
macro(GENERATE_MESSAGE_SOURCES _output_source _inputs)
    unset(_input_files)
    unset(_outputs)
    foreach (_file IN ITEMS ${_inputs})
        get_filename_component(_name ${_file} NAME_WE)
        list(APPEND _input_files ${WEBKIT_DIR}/${_file}.messages.in)
        list(APPEND _outputs
            ${WebKit_DERIVED_SOURCES_DIR}/${_name}MessageReceiver.cpp
            ${WebKit_DERIVED_SOURCES_DIR}/${_name}Messages.h
            ${WebKit_DERIVED_SOURCES_DIR}/${_name}MessagesReplies.h
        )
        list(APPEND ${_output_source} ${WebKit_DERIVED_SOURCES_DIR}/${_name}MessageReceiver.cpp)
    endforeach ()
    list(APPEND ${_output_source} ${WebKit_DERIVED_SOURCES_DIR}/MessageNames.cpp)

    add_custom_command(
        OUTPUT
            ${WebKit_DERIVED_SOURCES_DIR}/MessageArgumentDescriptions.cpp
            ${WebKit_DERIVED_SOURCES_DIR}/MessageNames.cpp
            ${WebKit_DERIVED_SOURCES_DIR}/MessageNames.h
            ${_outputs}
        MAIN_DEPENDENCY ${WEBKIT_DIR}/Scripts/generate-message-receiver.py
        DEPENDS
            ${WEBKIT_DIR}/Scripts/webkit/__init__.py
            ${WEBKIT_DIR}/Scripts/webkit/messages.py
            ${WEBKIT_DIR}/Scripts/webkit/model.py
            ${WEBKIT_DIR}/Scripts/webkit/parser.py
            ${_input_files}
        COMMAND ${PYTHON_EXECUTABLE} ${WEBKIT_DIR}/Scripts/generate-message-receiver.py ${WEBKIT_DIR} ${_inputs}
        WORKING_DIRECTORY ${WebKit_DERIVED_SOURCES_DIR}
        VERBATIM
    )
endmacro()
GENERATE_MESSAGE_SOURCES(WebKit_DERIVED_SOURCES "${WebKit_MESSAGES_IN_FILES}")

set(WebKit_WEB_PREFERENCES_TEMPLATES
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPageUpdatePreferences.cpp.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesDefinitions.h.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesExperimentalFeatures.cpp.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesGetterSetters.cpp.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesInternalDebugFeatures.cpp.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesKeys.cpp.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesKeys.h.erb
    ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesStoreDefaultsMap.cpp.erb
)

set(WebKit_WEB_PREFERENCES
    ${WTF_SCRIPTS_DIR}/Preferences/WebPreferences.yaml
    ${WTF_SCRIPTS_DIR}/Preferences/WebPreferencesDebug.yaml
    ${WTF_SCRIPTS_DIR}/Preferences/WebPreferencesExperimental.yaml
    ${WTF_SCRIPTS_DIR}/Preferences/WebPreferencesInternal.yaml
)

set_source_files_properties(${WebKit_WEB_PREFERENCES} PROPERTIES GENERATED TRUE)

add_custom_command(
    OUTPUT ${WebKit_DERIVED_SOURCES_DIR}/WebPageUpdatePreferences.cpp ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesDefinitions.h ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesExperimentalFeatures.cpp ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesGetterSetters.cpp ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesInternalDebugFeatures.cpp ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesKeys.cpp ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesKeys.h ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesStoreDefaultsMap.cpp
    DEPENDS ${WebKit_WEB_PREFERENCES_TEMPLATES} ${WebKit_WEB_PREFERENCES} WTF_CopyPreferences
    COMMAND ${RUBY_EXECUTABLE} ${WTF_SCRIPTS_DIR}/GeneratePreferences.rb --frontend WebKit --base ${WTF_SCRIPTS_DIR}/Preferences/WebPreferences.yaml --debug ${WTF_SCRIPTS_DIR}/Preferences/WebPreferencesDebug.yaml --experimental ${WTF_SCRIPTS_DIR}/Preferences/WebPreferencesExperimental.yaml --internal ${WTF_SCRIPTS_DIR}/Preferences/WebPreferencesInternal.yaml --outputDir "${WebKit_DERIVED_SOURCES_DIR}" --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPageUpdatePreferences.cpp.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesDefinitions.h.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesExperimentalFeatures.cpp.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesGetterSetters.cpp.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesInternalDebugFeatures.cpp.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesKeys.cpp.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesKeys.h.erb --template ${WEBKIT_DIR}/Scripts/PreferencesTemplates/WebPreferencesStoreDefaultsMap.cpp.erb
    VERBATIM)

list(APPEND WebKit_HEADERS
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesDefinitions.h
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesKeys.h
)

list(APPEND WebKit_SOURCES
    ${WebKit_DERIVED_SOURCES_DIR}/WebPageUpdatePreferences.cpp
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesExperimentalFeatures.cpp
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesGetterSetters.cpp
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesInternalDebugFeatures.cpp
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesKeys.cpp
    ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesStoreDefaultsMap.cpp
)

list(APPEND WebKit_SOURCES ${WebKit_DERIVED_SOURCES})

WEBKIT_COMPUTE_SOURCES(WebKit)
WEBKIT_FRAMEWORK(WebKit)
WEBKIT_FRAMEWORK_TARGET(WebKit)
ADD_WEBKIT_PREFIX_HEADER(WebKit)

list(APPEND WebProcess_PRIVATE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:WebKit,INCLUDE_DIRECTORIES>)
WEBKIT_EXECUTABLE(WebProcess)
ADD_WEBKIT_PREFIX_HEADER(WebProcess)

list(APPEND NetworkProcess_PRIVATE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:WebKit,INCLUDE_DIRECTORIES>)
WEBKIT_EXECUTABLE(NetworkProcess)
ADD_WEBKIT_PREFIX_HEADER(NetworkProcess)

if (ENABLE_GPU_PROCESS)
    list(APPEND GPUProcess_PRIVATE_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:WebKit,INCLUDE_DIRECTORIES>)
    WEBKIT_EXECUTABLE(GPUProcess)
    ADD_WEBKIT_PREFIX_HEADER(GPUProcess)
endif ()

if (COMPILER_IS_GCC_OR_CLANG)
    WEBKIT_ADD_TARGET_CXX_FLAGS(WebKit -Wno-unused-parameter)
    WEBKIT_ADD_TARGET_CXX_FLAGS(WebProcess -Wno-unused-parameter)
    WEBKIT_ADD_TARGET_CXX_FLAGS(NetworkProcess -Wno-unused-parameter)
    if (ENABLE_GPU_PROCESS)
        WEBKIT_ADD_TARGET_CXX_FLAGS(GPUProcess -Wno-unused-parameter)
    endif ()
endif ()

if ("${PORT}" STREQUAL "Mac")
    WEBKIT_DEFINE_XPC_SERVICES()
else ()
    WEBKIT_POPULATE_LIBRARY_VERSION(WEBKIT)

    set_target_properties(WebKit PROPERTIES VERSION ${WEBKIT_VERSION} SOVERSION ${WEBKIT_VERSION_MAJOR})

    install(TARGETS WebKit WebProcess NetworkProcess
        LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
        RUNTIME DESTINATION "${LIBEXEC_INSTALL_DIR}"
    )
endif ()
