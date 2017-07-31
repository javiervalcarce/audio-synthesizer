// Hi Emacs, this is -*- coding: utf-8; mode: c; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
//
// ./autogen.sh
// export GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0
// gst-launch-1.0 filesrc location=file.txt.gz ! gzdec ! filesink location="ofile.txt"
//

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include "gstgzdec.h"
#include <zlib.h>



GST_DEBUG_CATEGORY_STATIC (gst_gz_dec_debug);
#define GST_CAT_DEFAULT gst_gz_dec_debug

/* Filter signals and args */
enum {
      /* FILL ME */
      LAST_SIGNAL
};

enum {
      PROP_0,
      PROP_SILENT
};


// Pad templates.
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                    GST_PAD_SINK,
                                                                    GST_PAD_ALWAYS,
                                                                    GST_STATIC_CAPS ("ANY")
                                                                    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
                                                                   GST_PAD_SRC,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS ("ANY")
                                                                   );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define gst_gz_dec_parent_class parent_class
G_DEFINE_TYPE (GstGzDec, gst_gz_dec, GST_TYPE_ELEMENT);


static void gst_gz_dec_class_init     (GstGzDecClass* klass);
static void gst_gz_dec_init           (GstGzDec* filter);
static void gst_gz_dec_set_property   (GObject* object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_gz_dec_get_property   (GObject* object, guint prop_id, GValue * value, GParamSpec * pspec);
static gboolean gst_gz_dec_sink_event (GstPad* pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_gz_dec_chain (GstPad* pad, GstObject * parent, GstBuffer * buf);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize the gzdec's class once. GObject vmethod implementations.
//
static void gst_gz_dec_class_init (GstGzDecClass * klass) {

      GObjectClass* gobject_class;
      GstElementClass* gstelement_class;

      gobject_class = (GObjectClass *) klass;
      gstelement_class = (GstElementClass *) klass;

      gobject_class->set_property = gst_gz_dec_set_property;
      gobject_class->get_property = gst_gz_dec_get_property;

      g_object_class_install_property(
                                      gobject_class,
                                      PROP_SILENT,
                                      g_param_spec_boolean ("silent",
                                                            "Silent",
                                                            "Produce verbose output ?",
                                                            FALSE,
                                                            G_PARAM_READWRITE));

      gst_element_class_set_details_simple(gstelement_class,
                                           "GzDec",
                                           "Inflate an input byte stream",
                                           "Basic inflate plugin",
                                           "Javier Valcarce <javier.valcarce@gmail.com>");

      gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get (&src_factory));
      gst_element_class_add_pad_template(gstelement_class, gst_static_pad_template_get (&sink_factory));
      
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_gz_dec_init(GstGzDec* filter) {
      
      filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
      
      gst_pad_set_event_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_gz_dec_sink_event));
      gst_pad_set_chain_function(filter->sinkpad, GST_DEBUG_FUNCPTR(gst_gz_dec_chain));

      GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
      gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

      filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
      GST_PAD_SET_PROXY_CAPS (filter->srcpad);
      gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

      
      filter->silent = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_gz_dec_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec) {
      GstGzDec *filter = GST_GZDEC (object);

      switch (prop_id) {
      case PROP_SILENT:
            filter->silent = g_value_get_boolean (value);
            break;
      default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
      }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void gst_gz_dec_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec) {
      GstGzDec *filter = GST_GZDEC (object);

      switch (prop_id) {
      case PROP_SILENT:
            g_value_set_boolean (value, filter->silent);
            break;
      default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
      }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* GstElement vmethod implementations */
/* this function handles sink events */
static gboolean gst_gz_dec_sink_event (GstPad * pad, GstObject * parent, GstEvent * event) {
      GstGzDec *filter;
      gboolean ret;

      filter = GST_GZDEC (parent);

      GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
                      GST_EVENT_TYPE_NAME (event), event);

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
static GstFlowReturn gst_gz_dec_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {
      GstGzDec *filter;

      filter = GST_GZDEC (parent);

      if (filter->silent == FALSE) {
            g_print("I'm plugged, therefore I'm in.\n");
      }

      
      // MINE...
      // ---------------------------------------------------------------------------------------------------------------
      /*
      z_stream strm;
      int ret;
      unsigned have;

      strm.zalloc    = Z_NULL;
      strm.zfree     = Z_NULL;
      strm.opaque    = Z_NULL;
      strm.next_in   = Z_NULL;
      strm.avail_in  = 0;

      ret = inflateInit(&strm);

      strm.next_in   = tmp;
      strm.avail_in  = header.content_size;
      strm.next_out  = ozbuf;
      strm.avail_out = kTestReportMaxSize;
      
      ret = inflate(&strm, Z_FINISH);
      if (ret == Z_STREAM_ERROR) {
            return kErrorZlib;
      }
      
      switch (ret) {
      case Z_NEED_DICT:
            ret = Z_DATA_ERROR;     // and fall through 
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
            inflateEnd(&strm);
            return kErrorZlib;
      }
      
      have = kTestReportMaxSize - strm.avail_out;
            
      if (strm.avail_in != 0) {
            return kErrorZlib;
      }

      inflateEnd(&strm);

      if (ret != Z_STREAM_END) {
            return kErrorZlib;
      }

      //printf("GetHardwareTestReport: El informe descomprimido pesa %d\n", have);
      
      //report->assign((const char*) ozbuf, have);  // ya incluye el '\0' final
      // ---------------------------------------------------------------------------------------------------------------

*/

      
      /* just push out the incoming buffer without touching it */
      return gst_pad_push (filter->srcpad, buf);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean gzdec_init(GstPlugin* gzdec) {
      /* debug category for fltering log messages
       *
       * exchange the string 'Template gzdec' with your description
       */
      GST_DEBUG_CATEGORY_INIT (gst_gz_dec_debug, "gzdec", 0, "Template gzdec");
      return gst_element_register (gzdec, "gzdec", GST_RANK_NONE, GST_TYPE_GZDEC);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstgzdec"
#endif

/* gstreamer looks for this structure to register gzdecs
 *
 * exchange the string 'Template gzdec' with your gzdec description
 */
GST_PLUGIN_DEFINE (
                   GST_VERSION_MAJOR,
                   GST_VERSION_MINOR,
                   gzdec,
                   "Inflate plugin",
                   gzdec_init,
                   VERSION,
                   "LGPL",
                   "GStreamer",
                   "http://gstreamer.net/"
                   )
