#ifndef UTIL_CONFIG_FILE_H
#define UTIL_CONFIG_FILE_H

#include <util_string_helper.h>
#include <util_log.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace util
{
  // 2015-10-20 Kenny: This cfg-file parser is very much tied into the grit design choices. Hence, it is really not a generic util tool so much any longer... Maybe we should re-design this some day in the future.

  /**
   * Reads in a simple configuration file with following format
   *
   * A single value is given as
   *
   *   name = value
   *
   * A list of values are given as
   *
   *   name = value1 value2 value3
   *
   * Comments starts at # and goes to the end of the line like this
   *
   *  # I am a comment
   *  name = value # the rest of this line is a comment
   *
   * A value can not have white space deliminators. Hence "too big"
   * will be considered 2 values. Whereas "filename.txt" is considered
   * one string.
   *
   * All names and values are case-sensitive.
   */
  class ConfigFile
  {
  protected:

    std::map< std::string, std::vector<std::string> > m_values;

    std::map< std::string, ConfigFile >               m_scopes;

    std::map< std::string, std::map< std::string, ConfigFile >  > m_instance;

  public:

    std::vector<std::string> get_values(std::string const & name) const
    {
      return m_values.at(name);
    }

    std::string get_value(std::string const & name, std::string const & default_value) const
    {
      if(m_values.find(name) != m_values.end())
        return m_values.at(name)[0];
      return default_value;
    }

    std::string get_value(std::string const & name) const
    {
      return get_value(name,"");
    }

    bool exist(std::string const & name) const
    {
      return !(m_values.find(name) == m_values.end());
    }

  public:

    std::string get_value(
                          std::string const & operation_name
                          , std::string const & label_value
                          , std::string const & parameter_name
                          , std::string const & default_value
                          ) const
    {
      if(m_instance.find(operation_name) == m_instance.end())
        return default_value;

      if(m_instance.at(operation_name).find(label_value) == m_instance.at(operation_name).end())
        return default_value;

      return m_instance.at(operation_name).at(label_value).get_value(parameter_name, default_value);
    }

    std::string get_value(
                          std::string const & operation_name
                          , std::string const & label_value
                          , std::string const & parameter_name
                          ) const
    {
      return get_value(operation_name,label_value,parameter_name, "");
    }

  public:

    ConfigFile()
    {
    }

    virtual ~ConfigFile(){}

  public:

    void clear()
    {
      m_values.clear();
    }

    bool load(
                std::string const &  filename
              , bool const & throw_on_errors = true
              )
    {
      util::Log log;

      if( filename.empty() )
      {
        if(throw_on_errors)
          throw std::invalid_argument("empty file name");

        return false;
      }

      std::ifstream file;
      file.open(filename.c_str());

      if (!file.good())
      {
        log <<  "file not found: " << filename <<  Log::newline();

        if(throw_on_errors)
          throw std::logic_error("file not found " + filename);

        return false;
      }
      log << "Parsing cfg-file: " << filename << Log::newline();

      std::string const whitespaces(" \t\f\v\n\r");

      while (!file.eof())
      {
        std::string line;

        std::getline(file, line);

        //--- First remove all comments
        size_t const comment_pos = line.find_first_of("#");
        if(comment_pos != std::string::npos)
        {
          line = line.substr(0,comment_pos);
        }

        if(line.empty())
          continue;

        //--- Strip whitespaces and convert into an array of string tokens
        std::vector<std::string> tokens;

        size_t start_pos  = 0;
        size_t end_pos    = 0;

        while (end_pos != std::string::npos)
        {
          start_pos  = line.find_first_not_of(whitespaces, end_pos);
          end_pos    = line.find_first_of(whitespaces, start_pos);

          if (start_pos != std::string::npos)
          {
            std::string token = line.substr(start_pos, end_pos - start_pos);
            tokens.push_back(token);
          }
        }

        if(tokens.size()>0)
        {
          //--- Examine array of string tokens to make sure they have proper format
          if(tokens.size() < 3)
          {
            log << "incorrect syntax in line :" << line << Log::newline();

            if(throw_on_errors)
              throw std::logic_error("incorrect syntax in line :" + line);

            return false;
          }

          //--- test if we got an import of another cfg file
          if( tokens[0].compare("import") == 0 )
          {
            // We know this is a recursive import of a cfg-file
            if( tokens[1].compare("=")== 0 )
            {
              if( ! this->load( tokens[2] ) )
              {
                log << "could not import  " << tokens[2] << " from " << line << Log::newline();

                if(throw_on_errors)
                  throw std::logic_error("could not import  " + tokens[2] + " from " + line );

                return false;
              }
            }else{
              // We must be reading in a new scope from some other cfg-file

              std::string const scope_name = tokens[1];

              if( ! this->m_scopes[scope_name].load( tokens[3] ) )
              {
                // 2015-09-16 Kenny: It would be nice to print out the
                //                   full path of the file to help users
                //                   setting up the right paths in cfg-files.
                log << "could not import  "
                    << tokens[3]
                    << " from "
                    << line
                    << Log::newline();

                if(throw_on_errors)
                  throw std::logic_error("could not import  " + tokens[3] + " from " + line );


                return false;
              }
            }
          }
          // 2015-10-24 Marek: or else it breaks with import statements.
          else if (tokens[1].compare("=")!= 0)
          {
            log << "incorrect syntax in line :" << line << Log::newline();

            if(throw_on_errors)
              throw std::logic_error("incorrect syntax in line :" + line);

            return false;
          }

          if( tokens[0].compare("assign") == 0 )
          {
            if(tokens.size() !=  5)
            {
              log << "incorrect syntax in line :" << line << Log::newline();

              if(throw_on_errors)
                throw std::logic_error("incorrect syntax in line :" + line);

              return false;
            }

            std::string const operation_name = tokens[2];
            std::string const label_value    = tokens[3];
            std::string const scope_name     = tokens[4];

            m_instance[operation_name][label_value] = m_scopes[scope_name];
          }

          if( tokens[0].compare("override") == 0 )
          {
            if(tokens.size() !=  6)
            {
              log << "incorrect syntax in line :" << line << Log::newline();

              if(throw_on_errors)
                throw std::logic_error("incorrect syntax in line :" + line);

              return false;
            }

            std::string const operation_name     = tokens[2];
            std::string const label_value        = tokens[3];
            std::string const parameter_name     = tokens[4];
            std::string const parameter_value    = tokens[5];

           if( m_instance[operation_name][label_value].m_values[parameter_name].empty() )
             m_instance[operation_name][label_value].m_values[parameter_name].push_back(parameter_value);
            else
              m_instance[operation_name][label_value].m_values[parameter_name][0] = parameter_value;
          }

          //--- Now we can store the config setting ----------------------------
          // 2015-10-04 Marek: if we read from multiple files, and they contain
          // variables with the same names, this will cause overwriting, changing
          // it to append instead.
          if ( m_values[tokens[0]].size() == 0 )
            m_values[tokens[0]] = std::vector<std::string>( tokens.begin() + 2, tokens.end() );
          else
            for ( std::vector<std::string>::iterator it = tokens.begin()+2; it != tokens.end(); ++it )
              m_values[tokens[0]].push_back(*it);

        }
      }

      return true;
    }

  };

}//namespace util

// UTIL_CONFIG_FILE_H
#endif
