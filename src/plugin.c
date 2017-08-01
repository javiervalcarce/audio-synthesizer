
#include <gst/gst.h>
#include "gstgzdec.h"
#include "gstaudiosynth1.h"


GST_DEBUG_CATEGORY(gst_gz_dec_debug);
GST_DEBUG_CATEGORY(gst_audio_synth1_debug);


static gboolean plugin_init(GstPlugin *plugin) {
	gboolean ret = TRUE;

	GST_DEBUG_CATEGORY_INIT(gst_gz_dec_debug, "gzdec", 0, "Template gzdec");
	GST_DEBUG_CATEGORY_INIT(gst_audio_synth1_debug, "audiosynth1", 0, "Template audiosynth1");
	
	ret = ret && gst_element_register(plugin, "gzdec", GST_RANK_NONE, GST_TYPE_GZDEC);
	ret = ret && gst_element_register(plugin, "audiosynth1", GST_RANK_NONE, GST_TYPE_AUDIOSYNTH1);
	
	return ret;
}


/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myjvg"
#endif

GST_PLUGIN_DEFINE(
	GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	gstjvg,
	"audio generator elements, that is, synthesizers",
	plugin_init,
	VERSION,
	"LGPL",
	"jvg", //GST_PACKAGE_NAME,
	"master" //GST_PACKAGE_ORIGIN
)

