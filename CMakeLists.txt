cmake_minimum_required(VERSION 3.14)

project(hdrv LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core Quick Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Concurrent REQUIRED)

add_library(pfm STATIC
    dependencies/pfm/pfm_input_file.cpp
    dependencies/pfm/pfm_output_file.cpp
)
target_include_directories(pfm PUBLIC dependencies/pfm/include)

add_library(pic STATIC
    dependencies/pic/pic_input_file.cpp
    dependencies/pic/pic_output_file.cpp
)
target_include_directories(pic PUBLIC dependencies/pic/include)

add_library(tinyexr INTERFACE)
target_include_directories(tinyexr INTERFACE dependencies/tinyexr)

add_executable(hdrv WIN32
    viewer/Main.cpp
    viewer/viewer.qrc
    viewer/image/Image.cpp
    viewer/image/Image.hpp
    viewer/model/ImageCollection.cpp
    viewer/model/ImageCollection.hpp
    viewer/model/ImageDocument.cpp
    viewer/model/ImageDocument.hpp
    viewer/model/Settings.cpp
    viewer/model/Settings.hpp
    viewer/view/ImageArea.cpp
    viewer/view/ImageArea.hpp
    viewer/view/ImageRenderer.cpp
    viewer/view/ImageRenderer.hpp
    viewer/view/IPCClient.cpp
    viewer/view/IPCClient.hpp
    viewer/view/IPCServer.cpp
    viewer/view/IPCServer.hpp
    $<$<PLATFORM_ID:Windows>:media/hdrv.rc>
)
target_include_directories(hdrv PRIVATE viewer)
target_compile_definitions(hdrv PRIVATE NOMINMAX $<$<CONFIG:Debug>:QT_QML_DEBUG>)
target_link_libraries(hdrv PRIVATE pfm pic tinyexr Qt6::Core Qt6::Quick Qt6::Concurrent)

if (WIN32)

add_library(hdrv_thumbnails SHARED
    thumbnails/ClassFactory.cpp
    thumbnails/ClassFactory.hpp
    thumbnails/Registry.cpp
    thumbnails/Registry.hpp
    thumbnails/ThumbnailProvider.cpp
    thumbnails/ThumbnailProvider.hpp
    thumbnails/Thumbnails.cpp
    thumbnails/Thumbnails.hpp
    viewer/image/Image.cpp
    viewer/image/Image.hpp
)
target_include_directories(hdrv_thumbnails PRIVATE viewer thumbnails)
target_compile_definitions(hdrv_thumbnails PRIVATE NOMINMAX)
target_link_libraries(hdrv_thumbnails PRIVATE pfm pic tinyexr Qt6::Core Qt6::Gui)

endif(WIN32)

