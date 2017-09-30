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

  Profiling::Monitor * Epot         = Profiling::get_monitor("Epot");
  Profiling::Monitor * Ekin         = Profiling::get_monitor("Ekin");
  Profiling::Monitor * Estrain      = Profiling::get_monitor("Estrain");

  file << "Epot = "    << util::matlab_write_vector( Epot->get_values() )    << ";" << std::endl;
  file << "Ekin = "    << util::matlab_write_vector( Ekin->get_values() )    << ";" << std::endl;
  file << "Estrain = " << util::matlab_write_vector( Estrain->get_values() ) << ";" << std::endl;

  file << util::matlab_write_profiling() << std::endl;

  file << "figure('Visible','off');" << std::endl;
  file << "plot(Epot,'r');" << std::endl;
  file << "hold on;" << std::endl;
  file << "plot(Ekin,'g');" << std::endl;
  file << "plot(Estrain,'b');" << std::endl;
  file << "hold off;"<< std::endl;
  file << "legend('Potential','Kinetic','Strain');" << std::endl;
  file << "xlabel('Time step nummber');" << std::endl;
  file << "ylabel('Energy');" << std::endl;
  file << "title('Mechanical Energy');" << std::endl;

  file.flush();
  file.close();

  logging << "Done writing profile data..." << newline;
}

// WRITE_PROFILING_H
#endif
