// Hi Emacs, this is -*- mode: c; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "gstaudiosynth1.h"

#include <stdio.h>

#include <gst/gst.h>
#include <jack/jack.h>
#include <jack/midiport.h>



GST_DEBUG_CATEGORY_STATIC(gst_audio_synth1_debug);
#define GST_CAT_DEFAULT gst_audio_synth1_debug

// Filter signals and args
enum {
      // FILL ME
      LAST_SIGNAL
};

enum {
      PROP_0,
      PROP_SILENT
};


// The pads of the element, its capabilities and formats.
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE (
      "sink",
      GST_PAD_SINK,
      GST_PAD_ALWAYS,
      GST_STATIC_CAPS ("ANY")
      );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE (
      "src",
      GST_PAD_SRC,
      GST_PAD_ALWAYS,
      GST_STATIC_CAPS ("ANY")
      );

#define gst_audio_synth1_parent_class parent_class
G_DEFINE_TYPE (GstAudioSynth1, gst_audio_synth1, GST_TYPE_ELEMENT);



static void          gst_audio_synth1_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void          gst_audio_synth1_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean      gst_audio_synth1_sink_event   (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_audio_synth1_chain        (GstPad * pad, GstObject * parent, GstBuffer * buf);


// C - C++ bridge
extern int cpp_init ();
extern int cpp_process (float* ibuf, float* obuf, int nframes);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int process(jack_nframes_t nframes, void *arg) {

      //GstAudioSynth1* filter = (GstAudioSynth1*) arg;
      GstAudioSynth1* filter  = GST_AUDIOSYNTH1(arg);
      
      printf("process %d\n", nframes);
      
	float* in;
      float* out;

	in  = (float*) jack_port_get_buffer(filter->input_port, nframes);
	out = (float*) jack_port_get_buffer(filter->output_port, nframes);

      return cpp_process(in, out, nframes);
}
static void jack_shutdown(void *arg) {
	exit(1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GObject vmethod implementations
// initialize the audiosynth1's class 
static void gst_audio_synth1_class_init(GstAudioSynth1Class * klass) {
      GObjectClass *gobject_class;
      GstElementClass *gstelement_class;

      gobject_class = (GObjectClass *) klass;
      gstelement_class = (GstElementClass *) klass;

      gobject_class->set_property = gst_audio_synth1_set_property;
      gobject_class->get_property = gst_audio_synth1_get_property;

      g_object_class_install_property(
            gobject_class,
            PROP_SILENT,
            g_param_spec_boolean("silent", "Silent", "Produce verbose output ?", FALSE, G_PARAM_READWRITE));

      gst_element_class_set_details_simple(
            gstelement_class,
            "AudioSynth1",
            "Audio synthesizer #1",
            "Audio synthesizer #1",
            "Javier Valcarce <javier.valcarce@gmail.com>");
      
      gst_element_class_add_pad_template(
            gstelement_class,
            gst_static_pad_template_get(&src_factory));
      gst_element_class_add_pad_template(
            gstelement_class,
            gst_static_pad_template_get(&sink_factory));
      
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_audio_synth1_init(GstAudioSynth1* filter) {
      
      filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
      gst_pad_set_event_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_audio_synth1_sink_event));
      gst_pad_set_chain_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_audio_synth1_chain));

      GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
      gst_element_add_pad (GST_ELEMENT(filter), filter->sinkpad);


      filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");      
      GST_PAD_SET_PROXY_CAPS (filter->srcpad);
      gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

      filter->silent = FALSE;


      

      // Jackd initialization
      const char **ports;
      const char *client_name = "simple";
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;

	// open a client connection to the JACK server
	filter->client = jack_client_open(client_name, options, &status, server_name);
	if (filter->client == NULL) {
		fprintf(stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf(stderr, "Unable to connect to JACK server\n");
		}
		exit(1);
	}
	if (status & JackServerStarted) {
		fprintf(stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(filter->client);
		fprintf(stderr, "unique name `%s' assigned\n", client_name);
	}

      
	// tell the JACK server to call `process()' whenever
	// there is work to be done.
	//
	jack_set_process_callback(filter->client, process, filter);

	// tell the JACK server to call `jack_shutdown()' if
	//   it ever shuts down, either entirely, or if it
	//   just decides to stop calling us.
	//
	jack_on_shutdown(filter->client, jack_shutdown, 0);

	// display the current sample rate.
	filter->sample_rate = jack_get_sample_rate(filter->client);

	printf ("engine sample rate: %" PRIu32 "\n", filter->sample_rate);

	// create the ports
	filter->input_port = jack_port_register(filter->client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
      filter->input_midi_port = jack_port_register(filter->client, "midi_input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
	filter->output_port = jack_port_register(filter->client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
      

	if ((filter->input_port == NULL) || (filter->output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		exit (1);
	}

      if (filter->input_midi_port == NULL) {
		fprintf(stderr, "creating input midi port\n");
		exit (1);
	}

            
	// Tell the JACK server that we are ready to roll.  Our
	// process() callback will start running now. */

	if (jack_activate(filter->client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}

	// Connect the ports.  You can't do this before the client is
	// activated, because we can't make connections to clients
	// that aren't running.  Note the confusing (but necessary)
	// orientation of the driver backend ports: playback ports are
	// "input" to the backend, and capture ports are "output" from
	// it.
	ports = jack_get_ports(filter->client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
	if (ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		exit (1);
	}

	if (jack_connect(filter->client, ports[0], jack_port_name (filter->input_port))) {
		fprintf (stderr, "cannot connect input ports\n");
	}

	free (ports);

	ports = jack_get_ports(filter->client, NULL, NULL, JackPortIsPhysical | JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		exit (1);
	}

	if (jack_connect(filter->client, jack_port_name (filter->output_port), ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}

	free (ports);



      
      //jack_client_close (client);
	//exit (0);


      
      // Initialize C++ engine.
      cpp_init();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_audio_synth1_set_property(GObject * object, guint prop_id, const GValue* value, GParamSpec* pspec) {
      GstAudioSynth1 *filter = GST_AUDIOSYNTH1(object);

      switch (prop_id) {
      case PROP_SILENT:
            filter->silent = g_value_get_boolean(value);
            break;
      default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
      }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_audio_synth1_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
      GstAudioSynth1 *filter = GST_AUDIOSYNTH1(object);

      switch (prop_id) {
      case PROP_SILENT:
            g_value_set_boolean(value, filter->silent);
            break;
      default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
      }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GstElement vmethod implementations 
// this function handles sink events
static gboolean gst_audio_synth1_sink_event(GstPad* pad, GstObject* parent, GstEvent* event) {
      GstAudioSynth1 *filter;
      gboolean ret;

      filter = GST_AUDIOSYNTH1(parent);

      GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME (event), event);

      switch (GST_EVENT_TYPE (event)) {
      case GST_EVENT_CAPS:
      {
            GstCaps * caps;

            gst_event_parse_caps (event, &caps);
            /* do something with the caps */

            /* and forward */
            ret = gst_pad_event_default (pad, parent, event);
            break;
      }
      default:
            ret = gst_pad_event_default (pad, parent, event);
            break;
      }
      return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_audio_synth1_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {

      GstAudioSynth1 *filter;

      filter = GST_AUDIOSYNTH1 (parent);
      
      if (filter->silent == FALSE) {
            g_print ("I'm plugged, therefore I'm in.\n");
      }
      
      /* just push out the incoming buffer without touching it */
      return gst_pad_push(filter->srcpad, buf);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
