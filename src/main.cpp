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



jack_port_t *input_port;
jack_port_t *input_midi_port;
jack_port_t *output_port;
jack_client_t *client;


int n = 0;
int sample_rate;




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int process (jack_nframes_t nframes, void *arg) {

      printf("process\n");
      
	jack_default_audio_sample_t *in, *out;

	in = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (input_port, nframes));
	out = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (output_port, nframes));


	// 440 Hz Tone
	for (int i = 0; i < nframes; i++) {
	      n++;
            // Dos tonos.
	      //out[i] = (float) 1.0 * cos(M_PI * 440 / (float) sample_rate * 2.0 * n) +
            //      (float) 0.5 * cos(M_PI * 220 / (float) sample_rate * 2.0 * n + M_PI/2);
            out[i] = 0.0;
	}

	//memcpy (out, in, sizeof(jack_default_audio_sample_t) * nframes);

      // MIDI example
      jack_midi_event_t in_event;
      jack_nframes_t event_index = 0;
      jack_position_t position;
      jack_transport_state_t transport;


      // get the midi port data
      void* port_buf = jack_port_get_buffer(input_midi_port, nframes);
  
  
      // get the transport state of the JACK server
      transport = jack_transport_query(client, &position );
  
      // input: get number of events, and process them.
      jack_nframes_t event_count = jack_midi_get_event_count(port_buf);
      if (event_count > 0) {
            for (int i=0; i<event_count; i++) {

                  jack_midi_event_get(&in_event, port_buf, i);
      
                  // Using "cout" in the JACK process() callback is NOT realtime, this is
                  // used here for simplicity.
                  std::cout << "Frame " << position.frame << "  Event: " << i << " SubFrame#: " << in_event.time << " \tMessage:\t"
                            << (long)in_event.buffer[0] << "\t" << (long)in_event.buffer[1]
                            << "\t" << (long)in_event.buffer[2] << std::endl;
            }
      }
      
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */

void jack_shutdown (void *arg) {
	exit (1);
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char** argv) {
      
	const char **ports;
	const char *client_name = "simple";
	const char *server_name = NULL;

	jack_options_t options = JackNullOption;
	jack_status_t status;

	// open a client connection to the JACK server
	client = jack_client_open(client_name, options, &status, server_name);
	if (client == NULL) {
		fprintf(stderr, "jack_client_open() failed, "
                    "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf(stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf(stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf(stderr, "unique name `%s' assigned\n", client_name);
	}


      
	// tell the JACK server to call `process()' whenever
	// there is work to be done.
	//
	jack_set_process_callback(client, process, 0);

	// tell the JACK server to call `jack_shutdown()' if
	//   it ever shuts down, either entirely, or if it
	//   just decides to stop calling us.
	//
	jack_on_shutdown(client, jack_shutdown, 0);

	// display the current sample rate.
	sample_rate = jack_get_sample_rate(client);

	printf ("engine sample rate: %" PRIu32 "\n", sample_rate);

	// create the ports
	input_port = jack_port_register(client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
      input_midi_port = jack_port_register(client, "midi_input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
	output_port = jack_port_register(client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
      

	if ((input_port == NULL) || (output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

      if (input_midi_port == NULL) {
		fprintf(stderr, "creating input midi port\n");
		exit (1);
	}

            
	// Tell the JACK server that we are ready to roll.  Our
	// process() callback will start running now. */

	if (jack_activate(client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	// Connect the ports.  You can't do this before the client is
	// activated, because we can't make connections to clients
	// that aren't running.  Note the confusing (but necessary)
	// orientation of the driver backend ports: playback ports are
	// "input" to the backend, and capture ports are "output" from
	// it.
	ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}

	if (jack_connect(client, ports[0], jack_port_name (input_port))) {
		fprintf (stderr, "cannot connect input ports\n");
	}

	free (ports);

	ports = jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect(client, jack_port_name (output_port), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	free (ports);

	// keep running until stopped by the user
      while (1) {
            usleep (50000);  // 50 ms
      }
      

	// this is never reached but if the program
	// had some other way to exit besides being killed,
	// they would be important to call.
	
	jack_client_close (client);
	exit (0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
