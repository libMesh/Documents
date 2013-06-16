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



// Define a wrapper for exact_solution that will be needed below
void one_func (DenseVector<Number>& output,
			   const Point&,
			   const Real)
{
  output(0) = 1.;
}
void zero_func (DenseVector<Number>& output,
			    const Point&,
			    const Real)
{
  output(0) = 0.;
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
  int dim = 3;

  std::string order = "FIRST";
  std::string family = "LAGRANGE";

  // Create a mesh, with dimension to be overridden later, distributed
  // across the default MPI communicator.
  Mesh mesh(init.comm());

  // Use the MeshTools::Generation mesh generator to create a uniform
  mesh.read("man.e");

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
  std::set<boundary_id_type> one_ids, zero_ids;
  one_ids.insert(2);
  zero_ids.insert(3);
  zero_ids.insert(4);
  zero_ids.insert(5);
  zero_ids.insert(6);

  // Create a vector storing the variable numbers which the BC applies to
  std::vector<unsigned int> variables(1);
  variables[0] = u_var;

  // Create an AnalyticFunction object that we use to project the BC
  AnalyticFunction<> one_solution_object(one_func);
  AnalyticFunction<> zero_solution_object(zero_func);

  DirichletBoundary one_bc(one_ids,
			   variables,
			   &one_solution_object);

  DirichletBoundary zero_bc(zero_ids,
			    variables,
			    &zero_solution_object);

  // We must add the Dirichlet boundary condition _before_
  // we call equation_systems.init()
  system.get_dof_map().add_dirichlet_boundary(one_bc);
  system.get_dof_map().add_dirichlet_boundary(zero_bc);


  // Initialize the data structures for the equation system.
  equation_systems.init();

  // Print information about the system to the screen.
  equation_systems.print_info();
  mesh.print_info();

  // Solve the system "Poisson", just like example 2.
  system.solve();

#ifdef LIBMESH_HAVE_EXODUS_API
  ExodusII_IO (mesh).write_equation_systems ("out.e",
					     equation_systems);
#endif // #ifdef LIBMESH_HAVE_EXODUS_API

  // All done.
  return 0;
}
