/* The libMesh Finite Element Library. */
/* Copyright (C) 2003  Benjamin S. Kirk */

/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */



 // <h1>Introduction Example 4 - Solving a 1D, 2D or 3D Poisson Problem in Parallel</h1>
 //
 // This is the fourth example program.  It builds on
 // the third example program by showing how to formulate
 // the code in a dimension-independent way.  Very minor
 // changes to the example will allow the problem to be
 // solved in one, two or three dimensions.
 //
 // This example will also introduce the PerfLog class
 // as a way to monitor your code's performance.  We will
 // use it to instrument the matrix assembly code and look
 // for bottlenecks where we should focus optimization efforts.
 //
 // This example also shows how to extend example 3 to run in
 // parallel.  Notice how little has changed!


// C++ include files that we need
#include <iostream>
#include <algorithm>
#include <math.h>
#include <set>

// Basic include file needed for the mesh functionality.
#include "libmesh/libmesh.h"
#include "libmesh/mesh.h"
#include "libmesh/mesh_generation.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/gnuplot_io.h"
#include "libmesh/linear_implicit_system.h"
#include "libmesh/equation_systems.h"

// Define the Finite Element object.
#include "libmesh/fe.h"

// Define Gauss quadrature rules.
#include "libmesh/quadrature_gauss.h"

// Define the DofMap, which handles degree of freedom
// indexing.
#include "libmesh/dof_map.h"

// Define useful datatypes for finite element
// matrix and vector components.
#include "libmesh/sparse_matrix.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/dense_matrix.h"
#include "libmesh/dense_vector.h"

// Define the PerfLog, a performance logging utility.
// It is useful for timing events in a code and giving
// you an idea where bottlenecks lie.
#include "libmesh/perf_log.h"

// The definition of a geometric element
#include "libmesh/elem.h"

// To impose Dirichlet boundary conditions
#include "libmesh/dirichlet_boundaries.h"
#include "libmesh/analytic_function.h"

#include "libmesh/string_to_enum.h"
#include "libmesh/getpot.h"

#include "poisson_problem.h"

// Bring in everything from the libMesh namespace
using namespace libMesh;



// Exact solution function prototype.
Real exact_solution (const Real x,
		     const Real y,
		     const Real z = 0.);

// Define a wrapper for exact_solution that will be needed below
void exact_solution_wrapper (DenseVector<Number>& output,
                             const Point& p,
                             const Real)
{
  output(0) = exact_solution(p(0),
			     (LIBMESH_DIM>1)?p(1):0,
			     (LIBMESH_DIM>2)?p(2):0);
}

// Begin the main program.
int main (int argc, char** argv)
{
  // Initialize libMesh and any dependent libaries, like in example 2.
  LibMeshInit init (argc, argv);

  // Declare a performance log for the main program
  // PerfLog perf_main("Main Program");

  // Create a GetPot object to parse the command line
  GetPot command_line (argc, argv);

  // Check for proper calling arguments.
  if (argc < 3)
    {
      if (libMesh::processor_id() == 0)
        std::cerr << "Usage:\n"
                  <<"\t " << argv[0] << " -d 2(3)" << " -n 15"
                  << std::endl;

      // This handy function will print the file name, line number,
      // and then abort.  Currrently the library does not use C++
      // exception handling.
      libmesh_error();
    }

  // Brief message to the user regarding the program name
  // and command line arguments.
  else
    {
      std::cout << "Running " << argv[0];

      for (int i=1; i<argc; i++)
        std::cout << " " << argv[i];

      std::cout << std::endl << std::endl;
    }


  // Read problem dimension from command line.  Use int
  // instead of unsigned since the GetPot overload is ambiguous
  // otherwise.
  int dim = 2;
  if ( command_line.search(1, "-d") )
    dim = command_line.next(dim);

  // Skip higher-dimensional examples on a lower-dimensional libMesh build
  libmesh_example_assert(dim <= LIBMESH_DIM, "2D/3D support");

  // Create a mesh with user-defined dimension.
  // Read number of elements from command line
  int ps = 15;
  if ( command_line.search(1, "-n") )
    ps = command_line.next(ps);

  // Read FE order from command line
  std::string order = "SECOND";
  if ( command_line.search(2, "-Order", "-o") )
    order = command_line.next(order);

  // Read FE Family from command line
  std::string family = "LAGRANGE";
  if ( command_line.search(2, "-FEFamily", "-f") )
    family = command_line.next(family);

  // Cannot use discontinuous basis.
  if ((family == "MONOMIAL") || (family == "XYZ"))
    {
      if (libMesh::processor_id() == 0)
        std::cerr << "ex4 currently requires a C^0 (or higher) FE basis." << std::endl;
      libmesh_error();
    }

  // Create a mesh, with dimension to be overridden later, distributed
  // across the default MPI communicator.
  Mesh mesh(init.comm());

  // Use the MeshTools::Generation mesh generator to create a uniform
  // grid on the square [-1,1]^D.  We instruct the mesh generator
  // to build a mesh of 8x8 \p Quad9 elements in 2D, or \p Hex27
  // elements in 3D.  Building these higher-order elements allows
  // us to use higher-order approximation, as in example 3.

  Real halfwidth = dim > 1 ? 1. : 0.;
  Real halfheight = dim > 2 ? 1. : 0.;

  if ((family == "LAGRANGE") && (order == "FIRST"))
    {
      // No reason to use high-order geometric elements if we are
      // solving with low-order finite elements.
      MeshTools::Generation::build_cube (mesh,
                                         ps,
					 (dim>1) ? ps : 0,
					 (dim>2) ? ps : 0,
                                         -1., 1.,
                                         -halfwidth, halfwidth,
                                         -halfheight, halfheight,
                                         (dim==1)    ? EDGE2 :
                                         ((dim == 2) ? QUAD4 : HEX8));
    }

  else
    {
      MeshTools::Generation::build_cube (mesh,
					 ps,
					 (dim>1) ? ps : 0,
					 (dim>2) ? ps : 0,
                                         -1., 1.,
                                         -halfwidth, halfwidth,
                                         -halfheight, halfheight,
                                         (dim==1)    ? EDGE3 :
                                         ((dim == 2) ? QUAD9 : HEX27));
    }


  // Print information about the mesh to the screen.
  mesh.print_info();


  // Create an equation systems object.
  EquationSystems equation_systems (mesh);

  // Declare the system and its variables.
  // Create a system named "Poisson"
  LinearImplicitSystem& system =
    equation_systems.add_system<LinearImplicitSystem> ("Poisson");


  // Add the variable "u" to "Poisson".  "u"
  // will be approximated using second-order approximation.
  unsigned int u_var = system.add_variable("u",
                                           Utility::string_to_enum<Order>   (order),
                                           Utility::string_to_enum<FEFamily>(family));

  Poisson poisson(equation_systems);

  // Give the system a pointer to the matrix assembly
  // function.
  system.attach_assemble_object (poisson);

  // Construct a Dirichlet boundary condition object

  // Indicate which boundary IDs we impose the BC on
  // We either build a line, a square or a cube, and
  // here we indicate the boundaries IDs in each case
  std::set<boundary_id_type> boundary_ids;
  // the dim==1 mesh has two boundaries with IDs 0 and 1
  boundary_ids.insert(0);
  boundary_ids.insert(1);
  // the dim==2 mesh has four boundaries with IDs 0, 1, 2 and 3
  if(dim>=2)
  {
    boundary_ids.insert(2);
    boundary_ids.insert(3);
  }
  // the dim==3 mesh has four boundaries with IDs 0, 1, 2, 3, 4 and 5
  if(dim==3)
  {
    boundary_ids.insert(4);
    boundary_ids.insert(5);
  }

  // Create a vector storing the variable numbers which the BC applies to
  std::vector<unsigned int> variables(1);
  variables[0] = u_var;

  // Create an AnalyticFunction object that we use to project the BC
  // This function just calls the function exact_solution via exact_solution_wrapper
  AnalyticFunction<> exact_solution_object(exact_solution_wrapper);

  DirichletBoundary dirichlet_bc(boundary_ids,
                                 variables,
                                 &exact_solution_object);

  // We must add the Dirichlet boundary condition _before_
  // we call equation_systems.init()
  system.get_dof_map().add_dirichlet_boundary(dirichlet_bc);


  // Initialize the data structures for the equation system.
  equation_systems.init();

  // Print information about the system to the screen.
  equation_systems.print_info();
  mesh.print_info();

  // Solve the system "Poisson", just like example 2.
  system.solve();

  // After solving the system write the solution
  // to a GMV-formatted plot file.
  if(dim == 1)
  {
    GnuPlotIO plot(mesh,"Introduction Example 4, 1D",GnuPlotIO::GRID_ON);
    plot.write_equation_systems("gnuplot_script",equation_systems);
  }
#ifdef LIBMESH_HAVE_EXODUS_API
  else
  {
    ExodusII_IO (mesh).write_equation_systems ((dim == 3) ?
      "out_3.e" : "out_2.e",equation_systems);
  }
#endif // #ifdef LIBMESH_HAVE_EXODUS_API

  // All done.
  return 0;
}
