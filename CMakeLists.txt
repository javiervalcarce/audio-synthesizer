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

set (audiosynthesizer_SRCS src/main.cpp src/waveform_generator.cpp src/stopwatch.cpp)

set (CMAKE_VERBOSE_MAKEFILE on)

# Directorios de ficheros cabecera y de bibliotecas (opciones -I y -L respectivamente)
include_directories (src/include)

# opciones de compilación (CFLAGS)
add_definitions ("-Wall -g")

add_executable(audiosynthesizer ${audiosynthesizer_SRCS})
target_link_libraries(audiosynthesizer asound m pthread)
set_target_properties(audiosynthesizer PROPERTIES VERSION ${AUDIOSYNTHESIZER_VERSION_STRING})

