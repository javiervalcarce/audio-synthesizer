// Hi Emacs, this is -*- coding: utf-8; mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef AUDIOSYNTH_STOPWATCH_H_
#define AUDIOSYNTH_STOPWATCH_H_

#include <ctime>
#include <cstdint>
#include <string>


/**
 * Stopwatch.
 *
 * This class is a C++ implementation of the Stopwatch class included in the Dart standard library.
 * See https://api.dartlang.org/apidocs/channels/be/dartdoc-viewer/dart:core.Stopwatch
 *
 * Javier Valcarce, <javier.valcarce@sepsa.es>
 */
class Instrument {
public:
      
      /**
       * Creates a Stopwatch in stopped state with a zero elapsed count.
       */
      Instrument(std::string name);

      virtual ~Instrument();
      

      void Compute(double* data, int size);

      void NoteOn(int note, int velocity);
      void NoteOff(int note, int velocity);
      
      
private:
      
      std::string name_;
            
};


#endif  // AUDIOSYNTH_STOPWATCH_H_
