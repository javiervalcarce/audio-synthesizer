// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "options.h"
#include "version.h"

#include <glib.h>
#include <gst/gst.h>

#include <string>
#include <sstream>
#include <cstdio>
#include <cassert>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Options::LoadOptionsFromCommandLine(int argc, char** argv) {

      GOptionContext* ctx;
      GError* err = NULL;

      gchar* f0 = NULL;
      gchar* f1 = NULL;
      gchar* f2 = NULL;
      gchar* f3 = NULL;
      gchar* str_host = NULL;

      global_alpha_enable = false;
      global_alpha_value  = -1.0;

      key_color_enable = false;
      key_color_value  = -1;


      GOptionEntry entries[] = {
            //{ "layer0",     0, 0, G_OPTION_ARG_FILENAME, &f0,           "Layer #0 in the composition", "PNG-FILE or :fbdev or :test1" },
            
            { "frequency", 0, 0, G_OPTION_ARG_INT,      &frequency,   "Freq",                       "PORT" },
            { "instrument", 0, 0, G_OPTION_ARG_INT,      &html5_port,   "TCP Server Port for <video> tag",         "PORT" },
            { NULL }
      };


      ctx = g_option_context_new("");


      gchar str[256];
      g_snprintf(str, sizeof(str), "This is videosynth %s (publication date: %s, build date: %s)\n",
                 videosynth::VersionString(),
                 videosynth::PublicationDate(),
                 videosynth::BuildDate());

      g_option_context_set_summary(ctx, str);
      //g_option_context_set_description(ctx, "DESPUÉS");

      g_option_context_set_help_enabled(ctx, TRUE);
      g_option_context_add_main_entries(ctx, entries, NULL);
      g_option_context_add_group(ctx, gst_init_get_option_group());

      if (g_option_context_parse(ctx, &argc, &argv, &err) == FALSE) {
            // Unknown option, continue...
            g_warning("%s", err->message);
            exit(1);
            g_error_free(err);
      }

      // Assign gchar* to std::string
      if (f0 != NULL) {
            layer0 = f0;
      }
      if (f1 != NULL) {
            layer1 = f1;
      }
      if (f2 != NULL) {
            layer2 = f2;
      }
      if (f3 != NULL) {
            layer3 = f3;
      }

      // Opción --udpmc_host=
      if (str_host != NULL) {
            udpmc_host = str_host;
      }


      if (global_alpha_value != -1.0) {
            global_alpha_enable = true;
      }

      if (key_color_value != -1) {
            key_color_enable = true;
      }


      return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
