// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "version.h"
#include <string>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define AUDIOSYNTH_VERSION_MAJOR 0
#define AUDIOSYNTH_VERSION_MINOR 4
#define AUDIOSYNTH_VERSION_MICRO 0
#define AUDIOSYNTH_VERSION_EXTRA ""
#define AUDIOSYNTH_PUBLICATION_DATE "IPT"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int audiosynth::VersionMajor() {
      return AUDIOSYNTH_VERSION_MAJOR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int audiosynth::VersionMinor() {
      return AUDIOSYNTH_VERSION_MINOR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int audiosynth::VersionMicro() {
      return AUDIOSYNTH_VERSION_MICRO;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* audiosynth::VersionExtra() {
      return AUDIOSYNTH_VERSION_EXTRA;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* audiosynth::VersionString() {
      return STR(AUDIOSYNTH_VERSION_MAJOR) "." STR(AUDIOSYNTH_VERSION_MINOR) "." STR(AUDIOSYNTH_VERSION_MICRO) AUDIOSYNTH_VERSION_EXTRA;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool audiosynth::VersionCheck(int major, int minor, int micro) {
      if (major != AUDIOSYNTH_VERSION_MAJOR) {
            // Es una rama distinta, no es compatible
            return false;
      }
      if (minor > AUDIOSYNTH_VERSION_MINOR) {
            // El número minor mínimo exigido es mayor que la de esta biblioteca
            return false;
      } else if (minor == AUDIOSYNTH_VERSION_MINOR) {
            if (micro > AUDIOSYNTH_VERSION_MICRO) {
                  // El número micro mínimo exigido es mayor que la de esta biblioteca (dentro de la rama major.minor)
                  return false;
            }
      }
      return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* audiosynth::BuildDate() {
      return __DATE__ " " __TIME__;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* audiosynth::PublicationDate() {
      return AUDIOSYNTH_PUBLICATION_DATE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
