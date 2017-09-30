#ifndef UTIL_TIMER_H
#define UTIL_TIMER_H

#if defined(HAS_CXX11) || defined(HAS_CXX0X)
#define USE_CHRONO
#endif

#ifdef USE_CHRONO
  #include <chrono>
#else
  #include <time.h>
#endif

namespace util 
{

#ifdef USE_CHRONO

  class Timer
  {
  protected:

    std::chrono::time_point<std::chrono::system_clock> m_start;
    std::chrono::time_point<std::chrono::system_clock> m_end;

  public:

    void start()
    {
      m_start = std::chrono::system_clock::now();
    }

    void stop()
    {
      m_end = std::chrono::system_clock::now();
    }

    float duration()
    {
      this->stop();
      return std::chrono::duration_cast<std::chrono::milliseconds>(m_end-m_start).count();
    }

    /**
     * Get Time.
     *
     * @return     The time between start and stop in milliseconds.
     */
    float operator()()const
    {
       return std::chrono::duration_cast<std::chrono::milliseconds>(m_end-m_start).count();
    }
  };

#else

  class Timer
  {
  protected:

    clock_t         m_start;    ///<  A data structure to identify the start event.
    clock_t         m_end;      ///<  A data structure to identify the start event.

  public:

    void start()
    {
      m_start = m_end = clock();
    }

    void stop()
    {
      m_end = clock();
    }

    /**
     * Get Time without stopping the clock (lap time).
     *
     * @return     The time between start and now in milliseconds.
     */
    float duration()
    {
      this->stop();

      // 2010-06-09 Kenny code review: why do we need the float cast? I thought an int multiplied by a
      //                               float would be type cast into a float automatically by the compiler?
      return ( m_end - m_start ) / ((float) CLOCKS_PER_SEC) * 1000.0f;
    }

    /**
     * Get Time.
     *
     * @return     The time between start and stop in milliseconds.
     */
    float operator()()const
    {
      assert( m_end > m_start || !"operator(): Must call stop before operator()"); /// Marek: this assertion gets in the way of testing quite a lot -- suggested change.

      // 2010-06-09 Kenny code review: why do we need the float cast? I thought an int multiplied by a
      //                               float would be type cast into a float automatically by the compiler?
      return ( m_end - m_start ) / ((float) CLOCKS_PER_SEC) * 1000.0f;
    }
    
  };

#endif

}  // end of namespace util

// UTIL_TIMER_H
#endif 
