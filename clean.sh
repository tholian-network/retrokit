#!/bin/bash

rm -rf ./bin                2> /dev/null;
rm -rf ./lib                2> /dev/null;
rm -rf ./ANGLE              2> /dev/null;
rm -rf ./JavaScriptCore     2> /dev/null;
rm -rf ./JavaScriptCoreGLib 2> /dev/null;
rm -rf ./MiniBrowser        2> /dev/null;
rm -rf ./PAL                2> /dev/null;
rm -rf ./WTF                2> /dev/null;
rm -rf ./WebCore            2> /dev/null;
rm -rf ./WebDriver          2> /dev/null;
rm -rf ./WebInspectorUI     2> /dev/null;
rm -rf ./WebKit2Gtk         2> /dev/null;
rm -rf ./bmalloc            2> /dev/null;

rm -rf ./Source/JavaScriptCore/disassembler/udis86/__pycache__       2> /dev/null;
rm -rf ./Source/JavaScriptCore/inspector/scripts/codegen/__pycache__ 2> /dev/null;
rm -rf ./Source/JavaScriptCore/wasm/__pycache__                      2> /dev/null;
rm -rf ./Source/JavaScriptCore/yarr/__pycache__                      2> /dev/null;
rm ./Source/WebCore/idl_files_WebCoreBindings.tmp                    2> /dev/null;
rm ./Source/WebCore/idl_files_WebCoreTestSupportBindings.tmp         2> /dev/null;
rm ./Source/WebCore/pp_idl_files_WebCoreBindings.tmp                 2> /dev/null;
rm ./Source/WebCore/pp_idl_files_WebCoreTestSupportBindings.tmp      2> /dev/null;
rm ./inspector-resources.stamp                                       2> /dev/null;

rm -rf ./DerivedSources                          2> /dev/null;
rm ./Source/JavaScriptCore/javascriptcoregtk*.pc 2> /dev/null;
rm ./Source/WebCore/platform/gtk/po/POTFILES     2> /dev/null;
rm ./Source/WebCore/platform/gtk/po/*.gmo        2> /dev/null;
rm ./Source/WebCore/platform/gtk/po/*.pot        2> /dev/null;
rm ./Source/WebKit/webkit2gtk*.pc                2> /dev/null;
rm ./gtkdoc-jsc-glib.cfg                         2> /dev/null;
rm ./gtkdoc-webkit2gtk.cfg                       2> /dev/null;
rm ./gtkdoc-webkitdom.cfg                        2> /dev/null;

rm ./Tools/MiniBrowser/gtk/cmake_install.cmake           2> /dev/null;
rm ./Tools/cmake_install.cmake                           2> /dev/null;
rm ./Source/ThirdParty/ANGLE/cmake_install.cmake         2> /dev/null;
rm ./Source/ThirdParty/ANGLE/include/cmake_install.cmake 2> /dev/null;
rm ./Source/ThirdParty/xdgmime/cmake_install.cmake       2> /dev/null;
rm ./Source/JavaScriptCore/cmake_install.cmake           2> /dev/null;
rm ./Source/JavaScriptCore/shell/cmake_install.cmake     2> /dev/null;
rm ./Source/WebCore/cmake_install.cmake                  2> /dev/null;
rm ./Source/WebCore/PAL/pal/cmake_install.cmake          2> /dev/null;
rm ./Source/WebCore/platform/gtk/po/cmake_install.cmake  2> /dev/null;
rm ./Source/WebDriver/cmake_install.cmake                2> /dev/null;
rm ./Source/WebKit/cmake_install.cmake                   2> /dev/null;
rm ./Source/WTF/cmake_install.cmake                      2> /dev/null;
rm ./Source/WTF/wtf/cmake_install.cmake                  2> /dev/null;
rm ./Source/WebInspectorUI/cmake_install.cmake           2> /dev/null;
rm ./Source/bmalloc/cmake_install.cmake                  2> /dev/null;
rm ./Source/cmake_install.cmake                          2> /dev/null;
rm ./cmake_install.cmake                                 2> /dev/null;

rm -rf ./CMakeFiles                               2> /dev/null;
rm -rf ./Source/bmalloc/CMakeFiles                2> /dev/null;
rm -rf ./Source/JavaScriptCore/CMakeFiles         2> /dev/null;
rm -rf ./Source/JavaScriptCore/shell/CMakeFiles   2> /dev/null;
rm -rf ./Source/ThirdParty/ANGLE/CMakeFiles       2> /dev/null;
rm -rf ./Source/ThirdParty/xdgmime/CMakeFiles     2> /dev/null;
rm -rf ./Source/WebCore/CMakeFiles                2> /dev/null;
rm -rf ./Source/WebCore/PAL/pal/CMakeFiles        2> /dev/null;
rm -rf ./Source/WebDriver/CMakeFiles              2> /dev/null;
rm -rf ./Source/WebKit/CMakeFiles                 2> /dev/null;
rm -rf ./Source/WebKit/Scripts/webkit/__pycache__ 2> /dev/null;
rm -rf ./Source/WTF/wtf/CMakeFiles                2> /dev/null;
rm -rf ./Tools/MiniBrowser/gtk/CMakeFiles         2> /dev/null;

rm ./CMakeCache.txt                              2> /dev/null;
rm ./CPackConfig.cmake                           2> /dev/null;
rm ./CPackSourceConfig.cmake                     2> /dev/null;
rm ./cmakeconfig.h                               2> /dev/null;
rm ./.ninja_deps                                 2> /dev/null;
rm ./.ninja_log                                  2> /dev/null;
rm ./build.ninja                                 2> /dev/null;

rm ./JavaScriptCore-*.gir          2> /dev/null;
rm ./JavaScriptCore-*.typelib      2> /dev/null;
rm ./WebKit2-*.gir                 2> /dev/null;
rm ./WebKit2-*.typelib             2> /dev/null;
rm ./WebKit2WebExtension-*.gir     2> /dev/null;
rm ./WebKit2WebExtension-*.typelib 2> /dev/null;

