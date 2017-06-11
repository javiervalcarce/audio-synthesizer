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

set (AUDIOSYNTHESIZER_VERSION_MAJOR 0)
set (AUDIOSYNTHESIZER_VERSION_MINOR 1)
set (AUDIOSYNTHESIZER_VERSION_MICRO 0)
set (AUDIOSYNTHESIZER_VERSION_STRING ${AUDIOSYNTHESIZER_VERSION_MAJOR}.${AUDIOSYNTHESIZER_VERSION_MINOR}.${AUDIOSYNTHESIZER_VERSION_MICRO})



include(FindPkgConfig)
pkg_search_module(JACK REQUIRED jack)
#pkg_search_module(ZLIB      REQUIRED zlib)

#${ZLIB_INCLUDE_DIRS}
include_directories (${JACK_INCLUDE_DIRS})
link_directories    (${JACK_LIBRARY_DIRS})


#set (audiosynthesizer_SRCS src/main.cpp src/waveform_generator.cpp src/stopwatch.cpp)
set (audiosynthesizer_SRCS src/main.cpp)

set (CMAKE_VERBOSE_MAKEFILE on)

# Directorios de ficheros cabecera y de bibliotecas (opciones -I y -L respectivamente)
include_directories (src/include)

# opciones de compilación (CFLAGS)
add_definitions ("-Wall -g")

add_executable(audiosynthesizer ${audiosynthesizer_SRCS})
target_link_libraries(audiosynthesizer ${JACK_LIBRARIES})
set_target_properties(audiosynthesizer PROPERTIES VERSION ${AUDIOSYNTHESIZER_VERSION_STRING})

#target_link_libraries(gstjvg ${GSTREAMER_LIBRARIES})
#target_link_libraries(gstjvg ${ZLIB_LIBRARIES})



