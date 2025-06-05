cmake_minimum_required(VERSION 3.22)

#默认存放静态库的文件夹位置
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
#默认存放动态库的文件夹位置
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
#默认存放可执行文件的文件夹位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(3rdparty/spdlog)
if(NOT TARGET spdlog)
    message(FATAL_ERROR "spdlog not found")
    message(FATAL_ERROR "please run git submodule update --init --recursive")
endif()

add_subdirectory(3rdparty/ElaWidgetTools)
if(NOT TARGET ElaWidgetTools)
    message(FATAL_ERROR "ElaWidgetTools not found")
    message(FATAL_ERROR "please run git submodule update --init --recursive")
endif()

add_subdirectory(3rdparty/tinyxml2)
if(NOT TARGET tinyxml2)
    message(FATAL_ERROR "tinyxml2 not found")
    message(FATAL_ERROR "please run git submodule update --init --recursive")
endif()

set(PROJECT_NAME "FFmpegPlayer")
project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(FFmpeg_PREFIX_DIR "C:/ffmpeg6.1.1")
#set(QT_PREFIX_DIR "C:/Qt5/Qt5.15.2/5.15.2/msvc2019_64")

if (NOT DEFINED FFmpeg_PREFIX_DIR)
    message(FATAL_ERROR "Please set FFmpeg_PREFIX_DIR")
endif ()
#if (NOT DEFINED QT_PREFIX_DIR)
#    message(FATAL_ERROR "Please set QT_PREFIX_DIR")
#endif ()
#set(CMAKE_PREFIX_PATH "${QT_PREFIX_DIR}/lib/cmake")
message(STATUS "FFmpeg_PREFIX_DIR: ${FFmpeg_PREFIX_DIR}")
#message(STATUS "QT_PREFIX_DIR: ${QT_PREFIX_DIR}")

set(CMAKE_PREFIX_PATH "${QT_PREFIX_DIR}/lib/cmake")
set(QT_MODULES
        Core
        Gui
        Widgets
)
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS ${QT_MODULES} LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_MODULES} LinguistTools)
message(STATUS "Qt version: ${Qt${QT_VERSION_MAJOR}_VERSION}")


set(TS_FILES "translations/en.ts"
)
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_SOURCE_DIR}/resources/translations")
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES "${CMAKE_SOURCE_DIR}" ${TS_FILES})
else ()
    qt_create_translation(QM_FILES "${CMAKE_SOURCE_DIR}" ${TS_FILES})
endif ()

file(GLOB_RECURSE HEADERS "include/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
include_directories(${PROJECT_NAME} "src")

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_big_resources(RESOURCES resources.qrc )
else ()
    qt5_add_big_resources(RESOURCES resources.qrc )
endif ()

add_executable(FFmpegPlayer
        WIN32
        logo.rc
        main.cpp
        MyMainWindows.cpp
        MyMainWindows.h
        ${HEADERS}
        ${SOURCES}
        ${RESOURCES}
        ${TS_FILES}
)
#SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS  "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS")

foreach (QT_MODULE ${QT_MODULES})
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::${QT_MODULE})
endforeach (QT_MODULE)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${ELAWIDGETTOOLS_INCLUDE_DIRS}
        ${FFmpeg_PREFIX_DIR}/include
        ${SPDLOG_PREFIX_DIR}/include
)

file(GLOB_RECURSE FFMPEG_LIBS "${FFmpeg_PREFIX_DIR}/lib/*.lib")
target_link_directories(${PROJECT_NAME} PUBLIC ${ELAWIDGETTOOLS_LIBRARY_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC ${FFmpeg_PREFIX_DIR}/lib)

target_link_libraries(${PROJECT_NAME} PUBLIC
        ${FFMPEG_LIBS}
        ElaWidgetTools
        spdlog::spdlog_header_only
        tinyxml2::tinyxml2
)
