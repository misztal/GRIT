#ifndef GRIT_BATCH_H
#define GRIT_BATCH_H

#include <grit_quality_analyzer.h>
#include <grit_do_operations.h>

#include <util_profiling.h>
#include <util_log.h>

#include <string>

namespace grit
{
  namespace details
  {

    // 2016-03-05 Kenny code review: Add some proper description of what a "batch" is so new GRIT developers get the idea.
    /**
     *
     *
     *
     * @tparam S               simplex type
     * @tparam E               logic expression type
     * @tparam Measure         implements InterfaceQualityMeasure<S>
     * @tparam Operation       implements InterfaceMeshOperation<S>
     * @tparam Assignment      implements InterfaceAttributeAssignment<S>
     * @tparam types           type binder
     */
    template<
              typename S
            , typename E
            , typename Measure
            , typename Operation
            , typename Assignment
            , typename types
            >
    inline unsigned int batch(
                              std::string                              const & operation_name
                              , std::vector<unsigned int>              const & labels
                              , LogicExpression<E>                     const & condition
                              , typename types::param_type             const & parameters
                              , typename types::attributes_type              & attributes
                              , InterfaceMesh                                & mesh
                              )
    {
      util::Log log;

      std::string const tab     = util::Log::tab();
      std::string const newline = util::Log::newline();

      unsigned int operations_done = 0u;

      if (parameters.profiling())
        START_TIMER(operation_name);

      if (parameters.verbose() && !(parameters.silent_threads()))
        log << "BATCH: " << operation_name << newline;

      for (unsigned int i = 0u; i < labels.size(); ++i)
      {
        unsigned int      const & label            = labels[i];
        unsigned int      const    max_iterations  = parameters.get_max_iterations( operation_name, label);

        for (unsigned int iteration=0u; iteration< max_iterations; ++iteration)
        {
          SimplexSet         const A         = compute_phase( mesh, label);
          Measure            const measure   = Measure( operation_name, label, parameters, mesh, attributes);
          QualityAnalyzer<S>       analyzer  = QualityAnalyzer<S>( A, measure);
          Operation                operation = Operation( operation_name, label, parameters);
          Assignment         const assign    = Assignment();

          operations_done += do_operations( mesh, analyzer, operation, condition, assign, parameters, attributes );
        }

      }

      if (parameters.verbose() && !(parameters.silent_threads()))
        log << "Leaving BATCH: " << operation_name << newline;

      if (parameters.profiling())
        STOP_TIMER(operation_name);

      return operations_done;
    }


  } // namespace details

} // namespace grit

// GRIT_BATCH_OPERATION_H
#endif

