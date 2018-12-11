#ifndef MAGNETISM_COMPUTE_STATIC_POTENTIAL_H
#define MAGNETISM_COMPUTE_STATIC_POTENTIAL_H

#include <magnetism_assemble_problem.h>
#include <magnetism_compute_quality_measure.h>

#include <glue_triplet.h>
#include <glue_matlab_write_mesh.h>

#include <util_coo_matrix.h>
#include <util_solve_equation.h>
#include <util_matlab_write_vector.h>
#include <util_matlab_write_matrix.h>
#include <util_generate_filename.h>
#include <util_log.h>

#include <vector>

namespace magnetism
{

  /**
   * @param order_2nd      If true then second order Neumann BC is used otherwise we fall back on 1st order Neumann BC.
   *
   */
  template<typename T>
  inline void compute_static_potential(
                                       std::vector<unsigned int>    const & labels
                                       , unsigned int               const & magnet_label
                                       , std::vector<unsigned int>  const & magnet_labels
                                       , std::vector<glue::Triplet> const & triangles
                                       , std::vector<glue::Triplet> const & adjacent
                                       , bool                       const & order_2nd
                                       , std::vector<T>             const & px
                                       , std::vector<T>             const & py
                                       , T                          const & Mx
                                       , T                          const & My
                                       , std::vector<T>                   & phi  // Solution
                                       , std::vector<double>              & q    // Quality measure
                                       , bool                       const & use_host
                                       , unsigned int               const & max_iterations
                                       , T                          const & relative_tolerance
                                       , T                          const & absolute_tolerance
                                       , unsigned int               const & preconditioner_choice
                                       , unsigned int               const & solver_choice
                                       , bool                       const & write
                                       , bool                       const & append
                                       , std::string                const & filename
                                       )
  {
    util::COOMatrix<T> A;
    std::vector<T>     b;

    assemble_problem(
                       Mx
                     , My
                     , labels
                     , magnet_label
                     , triangles
                     , adjacent
                     , order_2nd
                     , px
                     , py
                     , A
                     , b
                     );

    phi.resize( A.cols() );
    std::fill( phi.begin(), phi.end(), 0.0);

    std::vector<T> residuals;

    util::solve_equation(
                         A
                         , phi
                         , b
                         , max_iterations
                         , relative_tolerance
                         , absolute_tolerance
                         , preconditioner_choice
                         , solver_choice
                         , (write ? &residuals : 0)
                         , use_host
                         );

    q.resize( A.cols() );
    std::fill( q.begin(), q.end(), 0.0);

    compute_quality_measure(labels, triangles, adjacent, px, py, phi, q);

    if(write)
    {
      static int count = 1;

      std::ofstream matlab;

      if( append )
      {
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
        std::string const stripped = filename.substr(0, filename.find_last_of(".") );
        std::string const unique_filename  = util::generate_filename(stripped, count, "m");

        matlab.open(unique_filename.c_str() );

        matlab << "close all;" << std::endl;
        matlab << "clear all;" << std::endl;
        matlab << "clc;" << std::endl;
      }

      matlab << "T_"   << count << "  = " << matlab_write_mesh(triangles)         << ";" << std::endl;
      matlab << "N_"   << count << "  = " << matlab_write_mesh(adjacent)          << ";" << std::endl;
      matlab << "px_"  << count << "  = " << util::matlab_write_vector( px )      << ";" << std::endl;
      matlab << "py_"  << count << "  = " << util::matlab_write_vector( py )      << ";" << std::endl;
      matlab << "phi_" << count << "  = " << util::matlab_write_vector( phi )     << ";" << std::endl;
      matlab << "q_" << count << "  = " << util::matlab_write_vector( q )     << ";" << std::endl;
      matlab << "L_"   << count << "  = " << util::matlab_write_vector( labels )  << ";" << std::endl;
      matlab << "R_"   << count << "  = " << util::matlab_write_vector( residuals )  << ";" << std::endl;

      matlab << "b_"   << count << "  = " << util::matlab_write_vector( b )  << ";" << std::endl;
      matlab << "A_"   << count << "  = " << A << ";" << std::endl;


      matlab << "figure('Visible','off');" << std::endl;
      matlab << "clf;" << std::endl;
      matlab << "semilogy(R_" << count << ",'b');" << std::endl;
      matlab << "xlabel('Iterations');" << std::endl;
      matlab << "ylabel('Residual norm');" << std::endl;
      matlab << "title('Residuals');" << std::endl;
      matlab << "print(gcf,'-depsc2','"
      << "residuals_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;
      matlab << "print(gcf,'-dpng','"
      << "residuals_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');"
      << std::endl;

      matlab << "figure('Visible','off');" << std::endl;
      matlab << "clf;" << std::endl;
      matlab << "triplot(T_" << count << ",px_" << count << ",py_" << count << ",'b');" << std::endl;
      matlab << "xlabel('x');" << std::endl;
      matlab << "ylabel('y');" << std::endl;
      matlab << "title('Computational Mesh');" << std::endl;
      matlab << "print(gcf,'-depsc2','"
      << "mesh_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;
      matlab << "print(gcf,'-dpng','"
      << "mesh_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');"
      << std::endl;

      matlab << "figure('Visible','off');" << std::endl;
      matlab << "clf;" << std::endl;
      matlab << "cdata = phi_" << count << "';" <<std::endl;
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
      matlab << "title('Magnetostatic Potential');" << std::endl;
      matlab << "print(gcf,'-depsc2','potential_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;
      matlab << "print(gcf,'-dpng','potential_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;


      //      matlab << "figure('Visible','off');" << std::endl;
      //      matlab << "clf;" << std::endl;
      //      matlab << "cdata = L_" << count << "';" << std::endl;
      //      matlab << "caxis( [ min(cdata(:))  max(cdata(:)) ] );" << std::endl;
      //      matlab << "patch('Faces', T_"
      //      <<  count
      //      << ", 'Vertices', [px_"
      //      << count
      //      << "', py_"
      //      << count
      //      << "'], 'FaceColor','flat','FaceVertexCData',cdata);"
      //      << std::endl;
      //      matlab << "colorbar;" << std::endl;
      //      matlab << "xlabel('x');" << std::endl;
      //      matlab << "ylabel('y');" << std::endl;
      //      matlab << "title('Labels');" << std::endl;
      //      matlab << "print(gcf,'-depsc2','labels_"
      //      << std::setw(8)
      //      << std::setfill('0')
      //      << count
      //      <<  "');" << std::endl;
      //      matlab << "print(gcf,'-dpng','labels_"
      //      << std::setw(8)
      //      << std::setfill('0')
      //      << count
      //      <<  "');" << std::endl;

      matlab << "figure('Visible','off');" << std::endl;
      matlab << "clf;" << std::endl;
      matlab << "cdata = q_" << count << "';" <<std::endl;
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
      matlab << "title('Quality Measure');" << std::endl;
      matlab << "print(gcf,'-depsc2','quality_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;
      matlab << "print(gcf,'-dpng','quality_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;


      matlab << "cx = (px_"<< count << "(T_" << count << "(:,1)) + px_" << count << "(T_"<< count <<"(:,2)) + px_"<< count <<"(T_"<< count <<"(:,3)))'/3.0;" << std::endl;
      matlab << "cy = (py_"<< count <<"(T_"<< count <<"(:,1)) + py_"<< count <<"(T_"<< count <<"(:,2)) + py_"<< count <<"(T_"<< count <<"(:,3)))'/3.0;" << std::endl;

      matlab << "min_x = min(px_"<< count <<"(:));" << std::endl;
      matlab << "max_x = max(px_"<< count <<"(:));" << std::endl;
      matlab << "min_y = min(py_"<< count <<"(:));" << std::endl;
      matlab << "max_y = max(py_"<< count <<"(:));" << std::endl;

      matlab << "width   = max_x - min_x;" << std::endl;
      matlab << "height  = max_y - min_y;" << std::endl;

      matlab << "[xq,yq] = meshgrid(min_x:width/40:max_x, min_y:height/40:max_y);" << std::endl;
      matlab << "vq = griddata(cx,cy,phi_"<< count <<",xq,yq);" << std::endl;

      //      matlab << "figure('Visible','off');" << std::endl;
      //      matlab << "clf;" << std::endl;
      //      matlab << "mesh(xq,yq,vq), hold on, plot3(cx,cy,phi_"<< count << ",'o'), hold off" << std::endl;
      //
      //      matlab << "print(gcf,'-depsc2','scatter_"
      //      << std::setw(8)
      //      << std::setfill('0')
      //      << count
      //      <<  "');" << std::endl;
      //
      //      matlab << "print(gcf,'-dpng','scatter_"
      //      << std::setw(8)
      //      << std::setfill('0')
      //      << count
      //      <<  "');" << std::endl;


      matlab << "figure('Visible','off');" << std::endl;
      matlab << "clf;" << std::endl;
      matlab << "[u, v] = gradient(vq);" << std::endl;
      matlab << "[xs,ys] = meshgrid(min_x:width/30:max_x, min_y:height/30:max_y);" << std::endl;

      matlab << "%vs = interp2(xq,yq,vq,xs,ys);" << std::endl;
      matlab << "%streamline(xq,yq,u,v,xs(vs<0),ys(vs<0));" << std::endl;
      matlab << "streamline(xq,yq,u,v,xs,ys);" << std::endl;

      matlab << "print(gcf,'-depsc2','streamlines_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;

      matlab << "print(gcf,'-dpng','streamlines_"
      << std::setw(8)
      << std::setfill('0')
      << count
      <<  "');" << std::endl;

      //      matlab << "figure('Visible','off');" << std::endl;
      //      matlab << "clf;" << std::endl;
      //      matlab << "plot(sort(eig(full(A_" << count << "))),'b');" << std::endl;
      //      matlab << "xlabel('Index');" << std::endl;
      //      matlab << "ylabel('Eigenvalue');" << std::endl;
      //      matlab << "title('Sorted Eigenvalues');" << std::endl;
      //      matlab << "print(gcf,'-depsc2','"
      //      << "eigenvalues_"
      //      << std::setw(8)
      //      << std::setfill('0')
      //      << count
      //      <<  "');" << std::endl;
      //      matlab << "print(gcf,'-dpng','"
      //      << "eigenvalues_"
      //      << std::setw(8)
      //      << std::setfill('0')
      //      << count
      //      <<  "');"
      //      << std::endl;
      
      
      matlab.flush();
      matlab.close();
      
      
      ++count;
    }
    
  }


}//namespace magnetism

// MAGNETISM_COMPUTE_STATIC_POTENTIAL_H
#endif
