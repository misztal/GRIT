#ifndef UTIL_LOG_H
#define UTIL_LOG_H


#include <util_use_pthreads.h>

#include <string>
#include <iostream>
#include <fstream>

namespace util
{

  class LogInfo
  {
  public:

    static bool & console()
    {
      static bool value = true;
      return value;
    }

    static bool & on()
    {
      static bool value = true;
      return value;
    }

    static std::ofstream & stream()
    {
      static std::ofstream value;
      return value;
    }

    static std::string & filename()
    {
      static std::string value = "log.txt";
      return value;
    }

    static bool & first_time()
    {
      static bool value = true;
      return value;
    }

  };


  class Log
  {
  public:

    static std::string tab()     { return "\t"; };
    static std::string newline() { return "\n"; };

  public:

    Log()
    {}


    ~Log()
    {}

  };

  // 2015-11-01 Kenny: If have multiple threads trying to write to the log at the same time is this then "synch"/"protected" properly?
  //
  //                   We could hide this using a RAII paradigm (https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization)
  //
  //                   We could just make a util::ThreadSafeLog class for this?


// /**
//  * This uses a RAII to make sure no other threads work on a log instance while it is in scope. One
//  * must be carefull though. Creating a global variable would prevent any threads from using the log.
//  *
//  */
//  class ThreadSafeLog : public Log
//  {
//  private:
//
//    ThreadSafeLog( ThreadSafeLog const & );
//    ThreadSafeLog & operator=(ThreadSafeLog const & );
//
//  protected:
//
//    typedef pthread_mutex_t mutex_type;
//
//
//    static mutex_type & get_mutex()
//    {
//      static mutex_type my_mutex = PTHREAD_MUTEX_INITIALIZER;
//      return my_mutex;
//    }
//
//  public:
//
//    ThreadSafeLog()
//    {
//      pthread_mutex_lock( &get_mutex() );
//    }
//
//
//    ~ThreadSafeLog()
//    {
//      pthread_mutex_unlock( &get_mutex() );
//    }
//    
//  };


  // This is a non-thread safe log output function... using several threads causes funny looking output... However, this version works perfectly with instances of ThreadSafeLog
  //  template<typename T>
  //  inline Log & operator<<(Log & log, T const & data)
  //  {
  //    if(LogInfo::on())
  //    {
  //      if(LogInfo::console())
  //      {
  //        std::cout << data;
  //      }
  //      else
  //      {
  //        if(LogInfo::first_time())
  //        {
  //          LogInfo::stream().open(LogInfo::filename().c_str(), std::ofstream::out );
  //          LogInfo::first_time() = false;
  //        }
  //        else
  //        {
  //          LogInfo::stream().open(LogInfo::filename().c_str(), std::ofstream::out | std::ofstream::app);
  //        }
  //        LogInfo::stream() << data;
  //        LogInfo::stream().flush();
  //        LogInfo::stream().close();
  //      }
  //    }
  //
  //    return log;
  //  }



  template<typename T>
  inline Log & operator<<(Log & log, T const & data)
  {
    // 2015-11-04 Kenny: I added a simple "global" mutex to make sure logging
    // looks okay when multiple threads output at the same time. The RAII
    // paradigm could be an alternative way of dealing with this... but it
    // requires us to go through our ocde and make sure no "global scope" log
    // instances is blocking any threads... This is tricky to do now.

#ifndef WIN32

    static pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock( &my_mutex );

#endif

    if(LogInfo::on())
    {
      if(LogInfo::console())
      {
        std::cout << data;
      }
      else
      {
        if(LogInfo::first_time())
        {
          LogInfo::stream().open(LogInfo::filename().c_str(), std::ofstream::out );
          LogInfo::first_time() = false;
        }
        else
        {
          LogInfo::stream().open(LogInfo::filename().c_str(), std::ofstream::out | std::ofstream::app);
        }
        LogInfo::stream() << data;
        LogInfo::stream().flush();
        LogInfo::stream().close();
      }
    }

#ifndef WIN32

    pthread_mutex_unlock( &my_mutex );

#endif 

    return log;
  }


} //namespace util

// UTIL_LOG_H
#endif
