cmake_minimum_required(VERSION 3.20)
project(ChannelPointPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt6 パッケージ検索
find_package(Qt6 COMPONENTS Core Widgets Network Test REQUIRED)

# ソースコード定義
set(PLUGIN_SOURCES
    src/ChannelPointPlugin.cpp
    src/modules/EffectManager.cpp
    src/modules/ObsNotifier.cpp
    src/modules/QueueManager.cpp
    src/modules/AnalyticsManager.cpp
    src/ui/VisualPreviewWidget.cpp
    src/ui/PluginMainWidget.cpp
)

set(PLUGIN_HEADERS
    src/ChannelPointPlugin.h
    src/types/ChannelPointTypes.h
    src/modules/EffectManager.h
    src/modules/ObsNotifier.h
    src/modules/QueueManager.h
    src/modules/AnalyticsManager.h
    src/ui/VisualPreviewWidget.h
    src/ui/PluginMainWidget.h
)

# プラグイン (DLL) ターゲットの追加
add_library(ChannelPointPlugin SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)
set_target_properties(ChannelPointPlugin PROPERTIES 
    PREFIX ""
    OUTPUT_NAME "libChannelPointPlugin"
)

target_include_directories(ChannelPointPlugin PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/src/shared"
)

target_link_libraries(ChannelPointPlugin PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# テストビルドターゲット
enable_testing()

add_executable(PluginAutoTest
    test/TestMain.cpp
    src/modules/EffectManager.cpp
    src/modules/ObsNotifier.cpp
    src/modules/QueueManager.cpp
    src/modules/AnalyticsManager.cpp
    src/ui/VisualPreviewWidget.cpp
)

target_include_directories(PluginAutoTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/src/shared"
)

target_link_libraries(PluginAutoTest PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Test
)

add_test(NAME PluginAutoTest COMMAND PluginAutoTest)
