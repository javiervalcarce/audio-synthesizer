// Hi Emacs, this is -*- coding: utf-8; mode: c; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
//
// ./autogen.sh
// make
// sudo make install
// export GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0
// gst-launch-1.0 filesrc location=file.txt.gz ! gzdec ! filesink location="file.txt"
//

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gstgzdec.h"


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
                                           "Inflate a deflated input byte stream",
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

      filter->strm.zalloc    = Z_NULL;
      filter->strm.zfree     = Z_NULL;
      filter->strm.opaque    = Z_NULL;
      filter->strm.next_in   = Z_NULL;
      filter->strm.avail_in  = 0;
      filter->strm.next_out  = Z_NULL;
      filter->strm.avail_out = 0;

      //inflateInit(&filter->strm);
      // gzip deflate format compatibility
      inflateInit2(&filter->strm, 16 + MAX_WBITS);
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
// Handles sink events.
static gboolean gst_gz_dec_sink_event(GstPad* pad, GstObject* parent, GstEvent* event) {
      GstGzDec *filter;
      gboolean ret;
      GstCaps* caps;
      
      filter = GST_GZDEC(parent);

      GST_LOG_OBJECT(filter, "Received %s event: %" GST_PTR_FORMAT, GST_EVENT_TYPE_NAME(event), event);

      switch (GST_EVENT_TYPE(event)) {
      case GST_EVENT_CAPS:
            gst_event_parse_caps(event, &caps);
            /* do something with the caps */
            /* and forward */
            ret = gst_pad_event_default(pad, parent, event);
            break;
      case GST_EVENT_EOS:
            g_print("EOS. Ignore it\n");
            return TRUE;
            break;
      default:
            ret = gst_pad_event_default(pad, parent, event);
            break;
      }
      return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Processing core.
// GST_PAD_MODE_PUSH scheduling mode
static GstFlowReturn gst_gz_dec_chain(GstPad* pad, GstObject* parent, GstBuffer* ibuf) {

      const int kOBufSize = 4096;
      GstGzDec *filter;
      GstFlowReturn ret;
      GstBuffer* obuf;
      GstMapInfo imap;
      GstMapInfo omap;
      int r;
      unsigned have;
      gboolean quit;
      
      filter = GST_GZDEC (parent);
      obuf = gst_buffer_new_allocate(NULL, kOBufSize, NULL);
      
      gst_buffer_map(ibuf, &imap, GST_MAP_READ);
      gst_buffer_map(obuf, &omap, GST_MAP_WRITE);

      g_print("New input block of %d bytes ----gstreamer %d.%d---------------------------------------------------------------------------------\n", imap.size, GST_VERSION_MAJOR, GST_VERSION_MINOR);
      
      //for (int i = 0; i < gst_buffer_get_size(ibuf); i++) {
      //      g_print("%02x ", imap.data[i]);
      //}

      filter->strm.next_in   = imap.data;
      filter->strm.avail_in  = imap.size;
      filter->strm.next_out  = omap.data;
      filter->strm.avail_out = omap.size;
            
      quit = FALSE;
      ret = GST_FLOW_OK;

      int count = 0;
      
      while (quit == FALSE) {

            count++;
            
            if (filter->silent == FALSE) {
                  g_print("count = %d avail_in  = %d avail_out = %d", count, filter->strm.avail_in, filter->strm.avail_out);
            }

            r = inflate(&filter->strm, Z_SYNC_FLUSH);

            if (filter->silent == FALSE) {
                  g_print(" r = %d avail_in  = %d avail_out = %d\n", r, filter->strm.avail_in, filter->strm.avail_out);
            }
            
            switch (r) {
            case Z_OK:                  // progress could be achieved, continue calling inflate()
            case Z_BUF_ERROR:           // no progress due to (1) no input data or (2) no space in output buffer

                  if (filter->strm.avail_in == 0) {
                        // (1) no more input data
                        have = kOBufSize - filter->strm.avail_out;                  
                        g_print("NO MORE INPUT DATA\n");

                        gst_buffer_set_size(obuf, have);
                        gst_buffer_unmap(obuf, &omap);
                        ret = gst_pad_push(filter->srcpad, obuf);
                        obuf = NULL; // gst_pad_push() took ownership of buffer
                        
                        if (ret != GST_FLOW_OK) {
                              GST_DEBUG_OBJECT (filter, "pad_push failed: %s", gst_flow_get_name(ret));
                        }

                        quit = TRUE;
                        
                  } else if (filter->strm.avail_out == 0) {
                        // (2) no more space in output buffer
                        have = kOBufSize - filter->strm.avail_out;                  
                        g_print("RUN OUT OF SPACE, Alloc new obuf\n");
                  
                        // No space left on output buffer: send buffer
                        gst_buffer_set_size(obuf, have);
                        gst_buffer_unmap(obuf, &omap);
                        ret = gst_pad_push(filter->srcpad, obuf);
                        obuf = NULL; // gst_pad_push() took ownership of buffer
                        
                        if (ret != GST_FLOW_OK) {
                              GST_DEBUG_OBJECT (filter, "pad_push failed: %s", gst_flow_get_name(ret));
                        }
                        
                        // Alloc new buffer to continue decompressing input data
                        obuf = gst_buffer_new_allocate(NULL, kOBufSize, NULL);
                        gst_buffer_map(obuf, &omap, GST_MAP_WRITE);

                        filter->strm.next_out  = omap.data;
                        filter->strm.avail_out = omap.size;
                  }
                  
                  break;
                  
            case Z_STREAM_END:

                  quit = TRUE;
                  
                  have = kOBufSize - filter->strm.avail_out;                  
                  g_print("STREAM_END. OK.\n", have, kOBufSize);
                  
                  gst_buffer_set_size(obuf, have);

                  // Send remanent buffer
                  gst_buffer_unmap(obuf, &omap);
                  ret = gst_pad_push(filter->srcpad, obuf);
                  obuf = NULL; // gst_pad_push() took ownership of buffer

                  if (ret != GST_FLOW_OK) {
                        GST_DEBUG_OBJECT(filter, "pad_push failed: %s", gst_flow_get_name (ret));
                  }
                  
                  GST_DEBUG_OBJECT(filter, "ZLIB STREAM_END. OK. Sending EOS.");
                  gst_pad_push_event(filter->srcpad, gst_event_new_eos());

                  // Reset zlib internal state to be ready for the next stream / segment.
                  inflateInit2(&filter->strm, 16 + MAX_WBITS);
                  count = 0;
                  
                  break;
                  
            case Z_ERRNO:
            case Z_STREAM_ERROR:    // zlib internal data structure error
            case Z_NEED_DICT:       // zlib needs a dictionay at this point
            case Z_DATA_ERROR:      // zlib data stream error
            case Z_MEM_ERROR:       // not enought memory
                  
                  
                  quit = TRUE;

                  if (filter->silent == FALSE) {
                        g_print("Bad input format. Is gziped?\n");
                  }
                  
                  inflateEnd(&filter->strm);
                  ret = GST_FLOW_ERROR;  
                                 
                  break;
            }
      
      }

      return ret; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
