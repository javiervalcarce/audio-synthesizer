########################################################################################################################
# CMakefile para audio-synthesizer
#
# Para compilar hay que instalar previamente la herramienta cmake 2.8+, disponible para Linux, Windows y Mac, una vez
# instalada, hacemos:
#
# mkdir -p build
# cd build
# cmake ..
# make
########################################################################################################################
PROJECT(audiosynthesizer)
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.6)
set (CMAKE_LEGACY_CYGWIN_WIN32 0)


if (APPLE)
   # macOS 10.12
   # export PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig/
   add_definitions (-DPLATFORM_MACOS)
   set(ENV{PKG_CONFIG_PATH} "/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig/")
else(APPLE)
   add_definitions (-DPLATFORM_LINUX_INTEL)
endif(APPLE)


find_package(PkgConfig REQUIRED)
#include(FindPkgConfig)

pkg_search_module(JACK REQUIRED jack)
pkg_search_module(GLIB REQUIRED glib-2.0 gobject-2.0)
#pkg_search_module(CAIRO cairo)
pkg_search_module(GSTREAMER REQUIRED gstreamer-1.0)
#pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0)
pkg_search_module(ZLIB REQUIRED zlib)

include_directories (src/include)
include_directories (${JACK_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
link_directories    (${JACK_LIBRARY_DIRS} ${GLIB_LIBRARY_DIRS} ${GSTREAMER_LIBRARY_DIRS} ${ZLIB_LIBRARY_DIRS})

#add_definitions ("-std=c++11 -Wall -D_FILE_OFFSET_BITS=64")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -D_FILE_OFFSET_BITS=64")


# Plug-in GStreamer
set (libgstjvg_SRCS src/plugin.c src/gstgzdec.c src/gstaudiosynth1.c)

# Programa principal
set (audiosynthesizer_SRCS src/main.cpp src/options.cpp src/version.cpp src/instrument.cpp)



# Versión de la biblioteca de plugins libgstjvg.so
set (LIBGSTJVG_VERSION_MAJOR 0)
set (LIBGSTJVG_VERSION_MINOR 1)
set (LIBGSTJVG_VERSION_MICRO 0)
set (LIBGSTJVG_VERSION_STRING ${LIBGSTJVG_VERSION_MAJOR}.${LIBGSTJVG_VERSION_MINOR}.${LIBGSTJVG_VERSION_MICRO})


SET_SOURCE_FILES_PROPERTIES(src/plugin.c PROPERTIES COMPILE_DEFINITIONS VERSION="${LIBGSTJVG_VERSION_STRING}")
#SET_SOURCE_FILES_PROPERTIES(src/gstgzdec.c PROPERTIES COMPILE_DEFINITIONS VERSION="${LIBGSTJVG_VERSION_STRING}")
#SET_SOURCE_FILES_PROPERTIES(src/gstaudiosynth1.c PROPERTIES COMPILE_DEFINITIONS VERSION="${LIBGSTJVG_VERSION_STRING}")

option (DEBUG        "Build the project using debuging code" ON)
option (ENABLE_BZIP  "Enable Bzip2 support" OFF)

if(DEBUG)
  add_definitions(-g)
endif()
if(ENABLE_BZIP)
  add_definitions(-DENABLE_BZIP)
endif()


# Artefactos de salida
add_library(gstjvg SHARED ${libgstjvg_SRCS})
set_target_properties(gstjvg PROPERTIES VERSION ${LIBGSTJVG_VERSION_STRING})
target_link_libraries(gstjvg ${GSTREAMER_LIBRARIES})
target_link_libraries(gstjvg ${ZLIB_LIBRARIES})

add_executable(audiosynthesizer ${audiosynthesizer_SRCS})
target_link_libraries(audiosynthesizer ${JACK_LIBRARIES} ${GLIB_LIBRARIES} ${GSTREAMER_LIBRARIES} m pthread)

# En Ubuntu 16.04 es así:
#include_directories (/usr/include/gstreamer-1.0)
#include_directories (/usr/include/glib-2.0)
#include_directories (/usr/lib/x86_64-linux-gnu/glib-2.0/include)
#include_directories (/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include)
#link_directories    (/usr/lib/x86_64-linux-gnu)
# Dependencias en Ubuntu 16.04:
#target_link_libraries(gstjvg gobject-2.0)
#target_link_libraries(gstjvg gstreamer-1.0)
#target_link_libraries(gstjvg z)
#target_link_libraries(gstjvg gmodule-2.0)
#target_link_libraries(gstjvg m)
#target_link_libraries(gstjvg rt)
#target_link_libraries(gstjvg dl)
#target_link_libraries(gstjvg pcre)
#target_link_libraries(gstjvg pthread)


