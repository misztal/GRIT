#ifndef ELASTICITY_MATLAB_WRITE_OBJECT_H
#define ELASTICITY_MATLAB_WRITE_OBJECT_H

#include <elasticity_object.h>
#include <elasticity_parameters.h>
#include <elasticity_compute_triangle_areas.h>
#include <elasticity_compute_lumped_masses.h>
#include <elasticity_compute_kinetic_energy.h>
#include <elasticity_compute_potential_energy.h>
#include <elasticity_compute_strain_energy.h>
#include <elasticity_matlab_write_stress_and_strain.h>

#include <glue_compute_neighbors.h>
#include <glue_matlab_write_mesh.h>

#include <util_matlab_write_vector.h>
#include <util_profiling.h>
#include <util_generate_filename.h>
#include <util_log.h>

#include <iostream>
#include <fstream>

namespace elasticity
{

  inline void matlab_write_object(
                                  Object & obj
                                  , Parameters const & params
                                  )
  {
    //  util::Log log;

    unsigned int const N  = obj.m_triangles.size();           // Number of triangles
    unsigned int const V  = obj.m_px.size();                  // Number of vertices

    double const E    = params.E();     // Young modulus
    double const nu   = params.nu();    // Poisson ratio
    double const rho0 = params.rho0();  // Material space mass density
    double const g    = params.g();     // Acceleration of gravity
    //  double const c    = params.c();     // Viscous mass damping

    std::vector< double >  A0e(N);       // Material Triangle areas
    std::vector< double >  M(V);         // Lumped mass matrix

    glue::Neighbors neighbors;
    glue::compute_neighbors(obj.m_triangles,V,neighbors);

    compute_triangle_areas(obj.m_triangles, obj.m_px0, obj.m_py0, A0e);

    compute_lumped_masses(neighbors, V, A0e, rho0, M);

    RECORD( "Ekin",    compute_kinetic_energy(obj.m_vx,obj.m_vy,M) );
    RECORD( "Epot",    compute_potential_energy(g,obj.m_px,obj.m_py,M) );
    RECORD( "Estrain", compute_strain_energy(obj.m_triangles, obj.m_px0, obj.m_py0, obj.m_px, obj.m_py, E, nu) );

    static int count = 1;

    std::ofstream matlab;

    if(params.data_append())
    {
      std::string const filename  = params.data_output_path() + "/" + params.data_filename();

      matlab.open( filename.c_str() , std::ofstream::out | std::ofstream::app);

      if(count==1)
      {
        matlab << "close all;" << std::endl;
        matlab << "clear all;" << std::endl;
        matlab << "clc;" << std::endl;
      }
    }
    else
    {
      std::string const stripped = params.data_filename().substr(0, params.data_filename().find_last_of(".") );
      std::string const unique_filename  = params.data_output_path() + "/" + util::generate_filename(stripped, count, "m");

      matlab.open(unique_filename.c_str() );

      matlab << "close all;" << std::endl;
      matlab << "clear all;" << std::endl;
      matlab << "clc;" << std::endl;
    }

    matlab << "T_" << count << "   = " << matlab_write_mesh(obj.m_triangles)     << ";" << std::endl;
    matlab << "A0e_" << count << " = " << util::matlab_write_vector( A0e ) << ";" << std::endl;
    matlab << "M_" << count << "   = " << util::matlab_write_vector( M )   << ";" << std::endl;

    matlab << "px0_" << count << " = " << util::matlab_write_vector( obj.m_px0 ) << ";" << std::endl;
    matlab << "py0_" << count << " = " << util::matlab_write_vector( obj.m_py0 ) << ";" << std::endl;
    matlab << "px_" << count << "  = " << util::matlab_write_vector( obj.m_px )  << ";" << std::endl;
    matlab << "py_" << count << "  = " << util::matlab_write_vector( obj.m_py )  << ";" << std::endl;
    matlab << "vx_" << count << "  = " << util::matlab_write_vector( obj.m_vx )  << ";" << std::endl;
    matlab << "vy_" << count << "  = " << util::matlab_write_vector( obj.m_vy )  << ";" << std::endl;

    matlab << "figure('Visible','off');" << std::endl;
    matlab << "clf;" << std::endl;
    matlab << "triplot(T_" << count << ",px0_" << count << ",py0_" << count << ",'r');" << std::endl;
    matlab << "hold on" << std::endl;
    matlab << "triplot(T_" << count << ",px_" << count << ",py_" << count << ",'b');" << std::endl;
    matlab << "quiver(px_" << count << ",py_" << count << ",vx_" << count << ",vy_" << count << ",'k');" << std::endl;
    matlab << "hold off" << std::endl;
    matlab << "xlabel('x');" << std::endl;
    matlab << "ylabel('y');" << std::endl;
    matlab << "title('Simulation');" << std::endl;
    matlab << "print(gcf,'-depsc2','"
           << "simulation"
           << std::setw(8)
           << std::setfill('0')
           << count
           <<  "');" << std::endl;
    matlab << "print(gcf,'-dpng','"
           << "simulation"
           << std::setw(8)
           << std::setfill('0')
           << count
           <<  "');"
           << std::endl;
    matlab << "sigma_"
           << count
           << "  = "
           << matlab_write_stress_and_strain( obj.m_triangles, obj.m_px0, obj.m_py0, obj.m_px, obj.m_py, E, nu )
           << ";"
           << std::endl;

    std::string names[] = {
      "E_xx"
      , "E_xy"
      , "E_yy"
      , "S_xx"
      , "S_xy"
      , "S_yy"
      , "sigma_xx"
      , "sigma_xy"
      , "sigma_yy"
    };
    
    std::string titles[] = {
      "Green Strain: E_{xx}"
      , "Green Strain: E_{xy}"
      , "Green Strain: E_{yy}"
      , "2nd PK: S_{xx}"
      , "2nd PK: S_{xy}"
      , "2nd PK: S_{yy}"
      , "Cauchy Stress: \\sigma_{xx}"
      , "Cauchy Stress: \\sigma_{xy}"
      , "Cauchy Stress: \\sigma_{yy}"
    };

    for(unsigned int i=0u; i<9u; ++i)
    {
      matlab << "figure('Visible','off');" << std::endl;

      matlab << "clf;" << std::endl;

      matlab << "cdata = sigma_"
             << count
             << "(:,"
             << i+1
             << ");"
             << std::endl;

      matlab << "caxis( [ min(cdata(:))  max(cdata(:)) ] );" << std::endl;

      matlab << "patch('Faces', T_"
             <<  count
             << ", 'Vertices', [px_"
             << count
             << "', py_"
             << count
             << "'], 'FaceColor','flat','FaceVertexCData',cdata);"
             << std::endl;

      matlab << "colorbar;" << std::endl;
      matlab << "xlabel('x');" << std::endl;
      matlab << "ylabel('y');" << std::endl;
      matlab << "title('" << titles[i] << "');" << std::endl;

      matlab << "print(gcf,'-depsc2','"
             << names[i]
             << std::setw(8)
             << std::setfill('0')
             << count
             <<  "');" << std::endl;

      matlab << "print(gcf,'-dpng','"
             << names[i]
             << std::setw(8)
             << std::setfill('0')
             << count
             <<  "');" << std::endl;
    }

    matlab.flush();
    matlab.close();

    ++count;
  }

}// end of namespace elasticity

// ELASTICITY_MATLAB_WRITE_OBJECT_H
#endif
