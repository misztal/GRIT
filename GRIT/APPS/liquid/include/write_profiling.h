#ifndef WRITE_PROFILING_H
#define WRITE_PROFILING_H

#include <util_config_file.h>
#include <util_log.h>
#include <util_profiling.h>
#include <util_matlab_write_profiling.h>

#include <string>
#include <fstream>

inline void write_profiling(util::ConfigFile const & settings )
{
  std::string  const output_path  = settings.get_value("output_path","");

  util::Log        logging;
  std::string const newline = util::Log::newline();

  std::string filename = output_path + "/profiling.m";

  std::ofstream file;

  file.open(filename.c_str());

  file << "close all;" << std::endl;
  file << "clear all;" << std::endl;
  file << "clc;"       << std::endl;

  file << util::matlab_write_profiling() << std::endl;

  file.flush();
  file.close();

  logging << "Done writing profile data..." << newline;
}

// WRITE_PROFILING_H
#endif
