#ifndef GRIT_PTHREAD_SCHEDULER_H
#define GRIT_PTHREAD_SCHEDULER_H

#include <grit_interface_scheduler.h>

#include <utilities/grit_compute_phase_labels.h>

#include <util_use_pthreads.h>
#include <util_log.h>

namespace grit
{

  namespace details
  {

    template<typename types>
    inline void* run_thread(void* md)
    {
      typedef typename types::subdomain_type SD;

      SD* domain = (SD*)(md);

      if (domain->parameters().verbose() && !(domain->parameters().silent_threads()))
      {
        util::Log logging;

        logging << "run_thread(): Working on subdomain " << domain->id() << util::Log::newline();
      }

      domain->algorithm().id() = domain->id();

      std::vector<unsigned int> labels;
      compute_phase_labels(domain->mesh(),labels);

      unsigned int operations_done = domain->algorithm().run(domain->mesh(), domain->attributes(), domain->parameters(), labels);
      domain->operations_done() = operations_done;

      return 0;
    }




    template<typename types>
    class PThreadScheduler
    : public InterfaceScheduler<types>
    {
    protected:

      typedef InterfaceScheduler<types> base_class;

    public:

      typedef typename types::mesh_impl                  MI;
      typedef typename types::math_types                 MT;

      typedef typename types::attributes_type            AMT;
      typedef typename types::domain_manager_type        DMT;
      typedef typename types::domain_decomposition_type  DDT;
      typedef typename types::monitor_type               MOT;
      typedef typename types::param_type                 PT;

      typedef typename types::real_type                  T;
      typedef typename types::vector3_type               V;

    protected:

      MOT              m_monitor;

    public:

      MOT        const & monitor()    const { return m_monitor; }

    public:

      MOT              & monitor()          { return m_monitor; }

    public:

      PThreadScheduler()
      {
      }

    public:


      void run(PT const & parameters)
      {
        unsigned int const number_of_subdomains = parameters.number_of_subdomains();

        monitor().reset();
        monitor().name() = "scheduler";
        monitor().init(parameters);

        unsigned int operations_done = 0;

        util::Log log;

        std::string const & newline = util::Log::newline();

        if (parameters.verbose())
        {
          log << "PThreadScheduler::run()" << newline;

          if (parameters.max_iterations("scheduler") < 2 && parameters.number_of_subdomains() > 1)
          {
            log << "PThreadScheduler::run() Warning: Possibly too low number of GRIT iterations for multithreaded setup." << newline;
          }
        }

        do
        {
          DDT domain_decomposition;

          if (parameters.verbose())
          {
            log << "PThreadScheduler::run() Entering main loop." << newline;
          }

          DMT domains = domain_decomposition.create_subdomains(parameters, base_class::mesh(), base_class::attributes());

          if (parameters.verbose())
          {
            log << "PThreadScheduler::run() Subdomains created." << newline;
          }

          std::vector<pthread_t> threads(number_of_subdomains);
          for (unsigned int tid = 0u; tid < number_of_subdomains; ++tid)
            pthread_create(&threads[tid], NULL, run_thread<types>, &(domains.subdomain(tid)));

          for (unsigned int tid = 0u; tid < number_of_subdomains; ++tid)
            pthread_join(threads[tid], NULL);

          for (unsigned int tid = 0u; tid < number_of_subdomains; ++tid)
            operations_done += domains.subdomain(tid).operations_done();

          if (parameters.verbose()) log << "PThreadScheduler::run(): operations_done = " << operations_done << newline;

          domain_decomposition.merge_subdomains(domains, parameters, base_class::mesh(), base_class::attributes());

          if (parameters.verbose()) log << "PTthreadScheduler::run(): Inside scheduler loop" << newline;

          m_monitor.update();

        } while( m_monitor.next() && operations_done );

        if (parameters.verbose())
        {
          log << "PTthreadScheduler::run(): Scheduler run completed!" << newline;
          log << "PTthreadScheduler::run(): number_of_subdomains  = " << number_of_subdomains << newline;
        }

      } // run()

    }; // class PThreadScheduler

  } // namespace details

} // namespace grit

// GRIT_PTHREAD_SCHEDULER_H
#endif

