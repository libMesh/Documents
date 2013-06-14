 // This example also runs with the experimental Trilinos NOX solvers by specifying
 // the --use-trilinos command line argument.


// C++ include files that we need
#include <iostream>
#include <algorithm>
#include <cmath>

// Various include files needed for the mesh & solver functionality.
#include "libmesh/libmesh.h"
#include "libmesh/mesh.h"
#include "libmesh/mesh_refinement.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/equation_systems.h"
#include "libmesh/fe.h"
#include "libmesh/quadrature_gauss.h"
#include "libmesh/dof_map.h"
#include "libmesh/sparse_matrix.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/dense_matrix.h"
#include "libmesh/dense_vector.h"
#include "libmesh/elem.h"
#include "libmesh/string_to_enum.h"
#include "libmesh/getpot.h"

// The nonlinear solver and system we will be using
#include "libmesh/nonlinear_solver.h"
#include "libmesh/nonlinear_implicit_system.h"

// Necessary for programmatically setting petsc options
#ifdef LIBMESH_HAVE_PETSC
#include <petsc.h>
#endif

// Laplace-Young class definition
#include "laplace_young.h"

// Bring in everything from the libMesh namespace
using namespace libMesh;



// Begin the main program.
int main (int argc, char** argv)
{
  // Initialize libMesh and any dependent libaries, like in example 2.
  LibMeshInit init (argc, argv);

#if !defined(LIBMESH_HAVE_PETSC) && !defined(LIBMESH_HAVE_TRILINOS)
  if (libMesh::processor_id() == 0)
    std::cerr << "ERROR: This example requires libMesh to be\n"
              << "compiled with nonlinear solver support from\n"
              << "PETSc or Trilinos!"
              << std::endl;
  return 0;
#endif

#ifndef LIBMESH_ENABLE_AMR
  if (libMesh::processor_id() == 0)
    std::cerr << "ERROR: This example requires libMesh to be\n"
              << "compiled with AMR support!"
              << std::endl;
  return 0;
#else

  // Create a GetPot object to parse the command line
  GetPot command_line (argc, argv);

  // Check for proper calling arguments.
  if (argc < 3)
    {
      if (libMesh::processor_id() == 0)
        std::cerr << "Usage:\n"
                  <<"\t " << argv[0] << " -r 2"
                  << std::endl;

      // This handy function will print the file name, line number,
      // and then abort.
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


  // Read number of refinements
  int nr = 2;
  if ( command_line.search(1, "-r") )
    nr = command_line.next(nr);

  // Read FE order from command line
  std::string order = "FIRST";
  if ( command_line.search(2, "-Order", "-o") )
    order = command_line.next(order);

  // Read FE Family from command line
  std::string family = "LAGRANGE";
  if ( command_line.search(2, "-FEFamily", "-f") )
    family = command_line.next(family);

  // Cannot use dicontinuous basis.
  if ((family == "MONOMIAL") || (family == "XYZ"))
    {
      std::cout << "ex19 currently requires a C^0 (or higher) FE basis." << std::endl;
      libmesh_error();
    }

  if ( command_line.search(1, "-pre") )
    {
#ifdef LIBMESH_HAVE_PETSC
      //Use the jacobian for preconditioning.
      PetscOptionsSetValue("-snes_mf_operator",PETSC_NULL);
#else
      std::cerr<<"Must be using PetsC to use jacobian based preconditioning"<<std::endl;

      //returning zero so that "make run" won't fail if we ever enable this capability there.
      return 0;
#endif //LIBMESH_HAVE_PETSC
    }

  // Skip this 2D example if libMesh was compiled as 1D-only.
  libmesh_example_assert(2 <= LIBMESH_DIM, "2D support");

  // Create a mesh, with dimension to be overridden by the file,
  // distributed across the default MPI communicator.
  Mesh mesh(init.comm());

  mesh.read ("lshaped.xda");

  if (order != "FIRST")
    mesh.all_second_order();

  MeshRefinement(mesh).uniformly_refine(nr);

  // Print information about the mesh to the screen.
  mesh.print_info();

  // Create an equation systems object.
  EquationSystems equation_systems (mesh);

  LaplaceYoung laplace_young (equation_systems);

  // Creates a system named "Laplace-Young"
  NonlinearImplicitSystem& system =
    equation_systems.add_system<NonlinearImplicitSystem> ("Laplace-Young");


  // Here we specify the tolerance for the nonlinear solver and
  // the maximum of nonlinear iterations.
  equation_systems.parameters.set<Real>         ("nonlinear solver tolerance")          = 1.e-12;
  equation_systems.parameters.set<unsigned int> ("nonlinear solver maximum iterations") = 50;


  // Adds the variable "u" to "Laplace-Young".  "u"
  // will be approximated using second-order approximation.
  system.add_variable("u",
		      Utility::string_to_enum<Order>   (order),
		      Utility::string_to_enum<FEFamily>(family));

  // Give the system a pointer to the functions that update
  // the residual and Jacobian.
  system.nonlinear_solver->residual_object = &laplace_young;
  system.nonlinear_solver->jacobian_object = &laplace_young;

  // Initialize the data structures for the equation system.
  equation_systems.init();

  // Prints information about the system to the screen.
  equation_systems.print_info();

  // Solve the system "Laplace-Young", print the number of iterations
  // and final residual
  equation_systems.get_system("Laplace-Young").solve();

  // Print out final convergence information.  This duplicates some
  // output from during the solve itself, but demonstrates another way
  // to get this information after the solve is complete.
  std::cout << "Laplace-Young system solved at nonlinear iteration "
	    << system.n_nonlinear_iterations()
	    << " , final nonlinear residual norm: "
	    << system.final_nonlinear_residual()
	    << std::endl;

#ifdef LIBMESH_HAVE_EXODUS_API
  // After solving the system write the solution
  ExodusII_IO (mesh).write_equation_systems ("out.e",
                                       equation_systems);
#endif // #ifdef LIBMESH_HAVE_EXODUS_API
#endif // #ifndef LIBMESH_ENABLE_AMR

  // All done.
  return 0;
}
