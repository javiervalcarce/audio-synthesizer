// Hi Emacs, this is -*- coding: utf-8; mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef AUDIOSYNTH_INSTRUMENT_H_
#define AUDIOSYNTH_INSTRUMENT_H_

#include <ctime>
#include <cstdint>
#include <string>


/**
 * Monophonic instrument.
 * 
 */
class MonophonicInstrument {
public:
      
      /**
       * Creates a Stopwatch in stopped state with a zero elapsed count.
       */
      MonophonicInstrument(std::string name, int sample_block_size = 32);

      virtual ~MonophonicInstrument();
      
      virtual int  SampleBlockSize() const =0;
      virtual int  PlayingNote() const =0;
      
      virtual void NoteOn(int note, int velocity) =0;
      virtual void NoteOff(int velocity) = 0;
      virtual void Reset() =0;    
      virtual void Compute(double* data, int size) =0;

      
private:
      
      std::string name_;
      int sample_block_size_;
      int playing_note_;
      
};


class MyInstrument : public MonophonicInstrument {
public:
      MyInstrument(std::string name);
      ~MyInstrument();

      virtual int  SampleBlockSize() const =0;
      virtual int  PlayingNote() const =0;

      void NoteOn(int note, int velocity) override;
      void NoteOff(int velocity) override;
      void Reset() override;    
      void Compute(double* data, int size) override;
      
private:
      
      
};



#endif  // AUDIOSYNTH_INSTRUMENT_H_
