// Hi Emacs, this is -*- coding: utf-8; mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef AUDIOSYNTH_OPTIONS_H_
#define AUDIOSYNTH_OPTIONS_H_
/**
 * This module contains the data structures that hold the applicación configuration in a global sense. It includes the
 * options at command line and options in config files and also the dynamic configuration received in execution time
 * (like DHCP IP address, hostname, routes, etc).
 */
#include <string>
#include <cstdint>


/**
 * App configuration set at command line.
 */
struct Options {
      Options() {
            frequency = 440.0;  // Hz
            decay = 1.0;        // coeficient
      }
      
      double  frequency;
      double  decay;  
      
      /**
       * Retrieves application's options from command line.
       * 
       * @param argc The same parameter received at main() function entry point.
       * @param argv The same parameter received at main() function entry point.
       */
      int LoadOptionsFromCommandLine(int argc, char** argv);

      /**
       * Retrieves application's options from a JSON configuration file containing a tree of key=value pairs in a
       * structure that resembles the MS Windows Registry.
       *
       * @param filename Configuration file path.
       */
      //int LoadOptionsFromFile(std::string filename);
      
      /** 
       * Merge the file options set and command line options set.
       *
       * Some options can be specified both in configuration file and as a command line option. The options specified at
       * command line takes precedence over those written in configuration file. For example: if you specify "rows = 2" in the
       * configuration file and "--rows=6" then "rows" will be 6 (not 2).
       *
       * This function overwrites the file options with the corresponding command line ones (higher precedence).
       */
      //void MergeCommandLineAndFileOPtions();
};


#endif  // AUDIOSYNTH_OPTIONS_H_
