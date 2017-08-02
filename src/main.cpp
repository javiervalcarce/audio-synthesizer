// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Experimental audio synthesizer.
//
// Copyright (C) 2017 Javier Valcarce, <javier.valcarce@gmail.com>
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// platform
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <jack/jack.h>
#include <jack/midiport.h>

// c++
#include <cmath>

// app
#include "options.h"
#include "version.h"










enum class MidiMessageType {
      kProgramChange,
      kNoteOn,
      kNoteOff,
      kVolume,
      kPanoramic,
      kAllNotesOff,
      kAllControllersOff,
      kPitchBend,
      kMidiReset,
      // ...
      kUnknown
};


struct MidiMessage {
      int time;  // Sample index at which event is valid.
      
      MidiMessageType type;
      
      union {
            struct {
                  int channel;
                  int instrument;
            } program_change;

            struct {
                  int channel;
                  int pitch;
                  int velocity;  // if 0 => note_off
            } note_on;
            
            struct {
                  int channel;
                  int pitch;
                  int velocity;
            } note_off;
            
            struct {
                  int channel;
                  int volume;
            } volume;

            struct {
                  int channel;
                  int pan;
            } panoramic;
            
            struct {
                  int channel;
            } all_controllers_off;   // reset values
            
            struct {
                  int channel;
            } all_notes_off;   // all notes in this channel off

            
            struct {
                  int channel;
                  int bend;
            } pitch_bend;

      };
};







int n = 0;
float ramp=0.0;
float note_on;
float note_frqs[128];
unsigned char note = 0;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MidiMessageType ParseEvent(uint8_t first_octet) {
      if ((first_octet & 0xf0) == 0x90 ) {
            return MidiMessageType::kNoteOn;
      } else if ((first_octet & 0xf0) == 0x80) {
            return MidiMessageType::kNoteOff;
      }
      return MidiMessageType::kUnknown;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void calc_note_frqs(jack_default_audio_sample_t srate)
{
	int i;
	for (i=0; i < 128; i++) {
		note_frqs[i] = (2.0 * 440.0 / 32.0) * pow(2, (((jack_default_audio_sample_t)i - 9.0) / 12.0)) / srate;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
extern "C" int cpp_process (float* ibuf, float* obuf, int nframes) {

	memcpy(obuf, ibuf, sizeof(float) * nframes);
     

      // MIDI example
      jack_midi_event_t in_event;
      jack_nframes_t event_index = 0;
      jack_position_t position;
      jack_transport_state_t transport;

/*
      // get the midi port data
      void* port_buf = jack_port_get_buffer(input_midi_port, nframes);
  
  
      // get the transport state of the JACK server
      transport = jack_transport_query(client, &position );
  
      // input: get number of events, and process them.
      jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
      if (event_count > 0) {
            for (int i = 0; i < event_count; i++) {

                  jack_midi_event_get(&in_event, port_buf, i);
                  // Using "cout" in the JACK process() callback is NOT realtime, this is
                  // used here for simplicity.
                  std::cout << "Frame " << position.frame << "  Event: " << i << " SubFrame#: " << in_event.time << " \tMessage:\t"
                            << (long)in_event.buffer[0] << "\t" << (long)in_event.buffer[1]
                            << "\t" << (long)in_event.buffer[2] << std::endl;
            }
      }

      
      
      MidiMessageType e;

      jack_midi_event_get(&in_event, port_buf, 0);

      static bool enabled = false;
      float freq;
      
	for (int i = 0; i < nframes; i++) {
            
		if ((in_event.time == i) && (event_index < event_count)) {
                  e = ParseEvent(*(in_event.buffer));
                  
			switch (e) {
                  case MidiMessageType::kNoteOn:
				note = *(in_event.buffer + 1);
                        enabled = true;
                        break;
                        
                  case MidiMessageType::kNoteOff:
				note = *(in_event.buffer + 1);
                        enabled = false;
                        break;
                  case MidiMessageType::kUnknown:
                        break;
                  }
                  
			event_index++;
			if (event_index < event_count) {
				jack_midi_event_get(&in_event, port_buf, event_index);
                  }
		}


            n++;
            freq = 440.0F;
            
            // Dos tonos.
            
            //if (enabled) {
            //      out[i] = (float) 1.0 * cos(M_PI * freq / (float) sample_rate * 2.0 * n);
            //} else {
            //      out[i] = 0.0;
            //}
                        
		//out[i] = note_on*sin(2 * M_PI * ramp);
	}
*/
      
	return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" int cpp_init () {
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
