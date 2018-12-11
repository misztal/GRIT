#ifndef MAGNETISM_COMPUTE_QUALITY_MEASURE_H
#define MAGNETISM_COMPUTE_QUALITY_MEASURE_H

#include <magnetism_compute_triangle_centers.h>
#include <magnetism_compute_triangle_areas.h>
#include <magnetism_compute_1ring.h>
#include <magnetism_compute_linear_regression.h>

#include <glue_triplet.h>

#include <util_log.h>

#include <vector>

namespace magnetism
{
  // 8-04-2015 Marek: removed refinement and coarsening thresholds from the arguments list
  inline void compute_quality_measure(
                                      std::vector<unsigned int>    const & labels
                                      , std::vector<glue::Triplet> const & triangles
                                      , std::vector<glue::Triplet> const & adjacent
                                      , std::vector<double>        const & px
                                      , std::vector<double>        const & py
                                      , std::vector<double>        const & phi
                                      , std::vector<double>              & q
                                      )
  {
    std::vector<double> cx;
    std::vector<double> cy;
    compute_triangle_centers(triangles, px, py, cx, cy);

    std::vector<double> area;
    compute_triangle_areas(triangles, px, py, area);

    unsigned int const N = triangles.size();   // Number of triangles

    //--- Compute quality measure for each triangle ------------------------------
    for (unsigned int i = 0u; i<N; ++i)
    {
      //--- Compute neighbor's neighbors (1 ring) --------------------------------
      std::vector<unsigned int> ring;
      compute_1ring(labels, triangles, adjacent, i, ring);

      //--- Compute distance d to linear regression on ring ----------------------
      double error = 0.0;
      compute_linear_regression(i, ring, cx, cy, phi, error);

      // 8-04-2015 Marek: Removing the squaring and area-weighting, as well as the conversion between qualities.
      q[i] = error;
      
    }
  }
  
}//namespace magnetism

// MAGNETISM_COMPUTE_QUALITY_MEASURE_H
#endif
