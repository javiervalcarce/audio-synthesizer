// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "options.h"
#include "version.h"
#include <glib.h>

#include <string>
#include <sstream>
#include <cstdio>
#include <cassert>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Options::LoadOptionsFromCommandLine(int argc, char** argv) {

      GOptionContext* ctx;
      GError* err = NULL;

      gchar* str_host = NULL;


      GOptionEntry entries[] = {
            
            //{ "layer0",     0, 0, G_OPTION_ARG_FILENAME, &f0,           "Layer #0 in the composition", "PNG-FILE or :fbdev or :test1" },           
            { "frequency",  0, 0, G_OPTION_ARG_INT, &frequency,   "Frequency (Hz)",     "HZ" },
            { "decay",      0, 0, G_OPTION_ARG_INT, &decay,       "Decay",              "V"  },
            { NULL }
      };

      ctx = g_option_context_new("");

      gchar str[256];
      g_snprintf(str, sizeof(str), "This is audiosynth %s (publication date: %s, build date: %s)\n",
                 audiosynth::VersionString(),
                 audiosynth::PublicationDate(),
                 audiosynth::BuildDate());

      g_option_context_set_summary(ctx, str);
      //g_option_context_set_description(ctx, "DESPUÉS");

      g_option_context_set_help_enabled(ctx, TRUE);
      g_option_context_add_main_entries(ctx, entries, NULL);
      //g_option_context_add_group(ctx, gst_init_get_option_group());

      if (g_option_context_parse(ctx, &argc, &argv, &err) == FALSE) {
            // Unknown option, continue...
            printf("%s", err->message);
            g_error_free(err);
            return 1;
      }

      // Assign gchar* to std::string
      if (str_host != NULL) {
            //
      }
      return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
