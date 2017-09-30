#ifndef ELASTICITY_COMPUTE_ADAPTIVE_TIME_STEP_H
#define ELASTICITY_COMPUTE_ADAPTIVE_TIME_STEP_H

#include <elasticity_object.h>
#include <elasticity_parameters.h>
#include <elasticity_compute_semi_implicit_time_step.h>

#include <util_log.h>

#include <cassert>
#include <algorithm>

namespace elasticity
{

  /**
   * Data structure for storing global information about the
   * adaptive time-stepping across invokations. This is a bit
   * ugly but it allow us to keep the time-step interface unchanged
   * throughout the whole simulation base code.
   */
  class AdaptiveInfo
  {
  public:

    /**
     * The number of consecutive times in a row that the adaptive
     * time stepper used the same time-step size.
     */
    static int & unchanged()
    {
      static int value = 0;
      return value;
    }

    /**
     * The actual time step size being currently used.
     */
    static double & dt()
    {
      static double value = 0.0;
      return value;
    }

  };

  /**
   * This auxiliary function is intended to measure the "error" in
   * time-integration.  Object A is being time integrated with much
   * smaller time steps than B. We essentially want to estimate the
   * largest time-discretization "drift" error made by object B.
   */
  inline double accuracy(Object const & A, Object const & B)
  {
    using std::fabs;
    using std::max;

    double diff = 0;

    for(unsigned int i = 0u; i < A.m_px_new.size(); ++i)
    {
      diff =   max(diff, fabs( A.m_px_new[i] - B.m_px_new[i]));
      diff =   max(diff, fabs( A.m_py_new[i] - B.m_py_new[i]));
    }

    return diff;
  }

  inline void compute_adaptive_time_step(
                                         double const & dt
                                         , Object & obj                 ///< The elastic object to be time integrated
                                         , Parameters const & params    ///< Parameters controling simulation
  )
  {
    util::Log log;

    std::string const tab     = util::Log::tab();
    std::string const newline = util::Log::newline();

    using std::min;
    using std::max;

    log << "--- Adaptive time integration invoked" << util::Log::newline();

    double T_left = dt;

    // If adaptive time stepper is not initialized pick some sensible time-step value
    if (AdaptiveInfo::dt()<=0)
    {
      AdaptiveInfo::dt() = dt;
      AdaptiveInfo::dt() = max( params.min_dt(), min( params.max_dt(), AdaptiveInfo::dt()));
      AdaptiveInfo::dt() = min( AdaptiveInfo::dt(), T_left);
      log << tab << "Adaptive: initialized dt = " << AdaptiveInfo::dt() << newline;
    }

    while (T_left>0.0)
    {
      // First we try if we are allowed to increase the time-step size
      if( AdaptiveInfo::unchanged() > params.doubling_count())
      {
        AdaptiveInfo::dt() = AdaptiveInfo::dt()*2.0;
        AdaptiveInfo::dt() = max( params.min_dt(), min( params.max_dt(), AdaptiveInfo::dt()));
        AdaptiveInfo::dt() = min( AdaptiveInfo::dt(), T_left);

        AdaptiveInfo::unchanged() = 0;
        log << tab << tab << "Adaptive: doubling dt = " << AdaptiveInfo::dt() << newline;
      }

      // Then we make a test time-integration
      log << tab << "Adaptive: trying dt = " << AdaptiveInfo::dt() << newline;

      Object A = obj;
      Object B = obj;

      compute_semi_implicit_time_step(AdaptiveInfo::dt()/2.0, A, params);
      compute_semi_implicit_time_step(AdaptiveInfo::dt()/2.0, A, params);
      compute_semi_implicit_time_step(AdaptiveInfo::dt(), B, params);

      // If accuracy is acceptable we accept the time-step or if it is impossible to do schrinking to get better accuracy
      if( accuracy(A,B) <= params.halving_tolerance()  || T_left <= params.min_dt() )
      {

        obj = A;
        ++AdaptiveInfo::unchanged();

      }else{

        // While accuracy is unacceptable we keep on schrinking the step-size to get better accuracy
        do{

          AdaptiveInfo::dt() = AdaptiveInfo::dt()/2.0;
          AdaptiveInfo::dt() = max( params.min_dt(), min( params.max_dt(), AdaptiveInfo::dt()));
          AdaptiveInfo::dt() = min( AdaptiveInfo::dt(), T_left);

          log << tab << util::Log::tab <<"Adaptive: halving dt = " << AdaptiveInfo::dt() << newline;

          A = obj;
          B = obj;

          compute_semi_implicit_time_step(AdaptiveInfo::dt()/2.0, A, params);
          compute_semi_implicit_time_step(AdaptiveInfo::dt()/2.0, A, params);
          compute_semi_implicit_time_step(AdaptiveInfo::dt(), B, params);

          // Impossible to schring more, we have to give up an accept the accuracy
          if( AdaptiveInfo::dt() == params.min_dt())
            break;
          
        }while( accuracy(A,B) > params.halving_tolerance() );
        
        obj = A;
        
        AdaptiveInfo::unchanged() = 1;
      }
      
      log << tab << "Adaptive: accepted dt = " << AdaptiveInfo::dt() << newline;
      T_left -= AdaptiveInfo::dt();
    }
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_ADAPTIVE_TIME_STEP_H
#endif
