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

#include "laplace_young.h"

// Bring in everything from the libMesh namespace
using namespace libMesh;

// Let-s define the physical parameters of the equation
const Real kappa = 1.;
const Real sigma = 0.2;


// This function computes the Jacobian K(x)
void LaplaceYoung::jacobian (const NumericVector<Number> &soln,
			     SparseMatrix<Number>    &jacobian,
			     NonlinearImplicitSystem &system)
{
  // Get a constant reference to the mesh object.
  const MeshBase& mesh = es.get_mesh();

  // The dimension that we are running
  const unsigned int dim = mesh.mesh_dimension();

  // A reference to the \p DofMap object for this system.  The \p DofMap
  // object handles the index translation from node and element numbers
  // to degree of freedom numbers.  We will talk more about the \p DofMap
  // in future examples.
  const DofMap& dof_map = system.get_dof_map();

  // Get a constant reference to the Finite Element type
  // for the first (and only) variable in the system.
  FEType fe_type = dof_map.variable_type(0);

  // Build a Finite Element object of the specified type.  Since the
  // \p FEBase::build() member dynamically creates memory we will
  // store the object as an \p AutoPtr<FEBase>.  This can be thought
  // of as a pointer that will clean up after itself.
  AutoPtr<FEBase> fe (FEBase::build(dim, fe_type));

  // A 5th order Gauss quadrature rule for numerical integration.
  QGauss qrule (dim, FIFTH);

  // Tell the finite element object to use our quadrature rule.
  fe->attach_quadrature_rule (&qrule);

  // Here we define some references to cell-specific data that
  // will be used to assemble the linear system.
  // We begin with the element Jacobian * quadrature weight at each
  // integration point.
  const std::vector<Real>& JxW = fe->get_JxW();

  // The element shape functions evaluated at the quadrature points.
  const std::vector<std::vector<Real> >& phi = fe->get_phi();

  // The element shape function gradients evaluated at the quadrature
  // points.
  const std::vector<std::vector<RealGradient> >& dphi = fe->get_dphi();

  // Define data structures to contain the Jacobian element matrix.
  // Following basic finite element terminology we will denote these
  // "Ke". More detail is in example 3.
  DenseMatrix<Number> Ke;

  // This vector will hold the degree of freedom indices for
  // the element.  These define where in the global system
  // the element degrees of freedom get mapped.
  std::vector<dof_id_type> dof_indices;

  // Now we will loop over all the active elements in the mesh which
  // are local to this processor.
  // We will compute the element Jacobian contribution.
  MeshBase::const_element_iterator       el     = mesh.active_local_elements_begin();
  const MeshBase::const_element_iterator end_el = mesh.active_local_elements_end();

  for ( ; el != end_el; ++el)
    {
      // Store a pointer to the element we are currently
      // working on.  This allows for nicer syntax later.
      const Elem* elem = *el;

      // Get the degree of freedom indices for the
      // current element.  These define where in the global
      // matrix and right-hand-side this element will
      // contribute to.
      dof_map.dof_indices (elem, dof_indices);

      // Compute the element-specific data for the current
      // element.  This involves computing the location of the
      // quadrature points (q_point) and the shape functions
      // (phi, dphi) for the current element.
      fe->reinit (elem);

      // Zero the element Jacobian before
      // summing them.  We use the resize member here because
      // the number of degrees of freedom might have changed from
      // the last element.  Note that this will be the case if the
      // element type is different (i.e. the last element was a
      // triangle, now we are on a quadrilateral).
      Ke.resize (dof_indices.size(),
                 dof_indices.size());

      // Now we will build the element Jacobian.  This involves
      // a double loop to integrate the test funcions (i) against
      // the trial functions (j). Note that the Jacobian depends
      // on the current solution x, which we access using the soln
      // vector.
      //
      for (unsigned int qp=0; qp<qrule.n_points(); qp++)
        {
          Gradient grad_u;

          for (unsigned int i=0; i<phi.size(); i++)
            grad_u += dphi[i][qp]*soln(dof_indices[i]);

          const Number K = 1./std::sqrt(1. + grad_u*grad_u);

          for (unsigned int i=0; i<phi.size(); i++)
            for (unsigned int j=0; j<phi.size(); j++)
              Ke(i,j) += JxW[qp]*(
                                  K*(dphi[i][qp]*dphi[j][qp]) +
                                  kappa*phi[i][qp]*phi[j][qp]
                                  );
        }

      dof_map.constrain_element_matrix (Ke, dof_indices);

      // Add the element matrix to the system Jacobian.
      jacobian.add_matrix (Ke, dof_indices);
    }

  // That's it.
}


// Here we compute the residual R(x) = K(x)*x - f. The current solution
// x is passed in the soln vector
void LaplaceYoung::residual (const NumericVector<Number> &soln,
			     NumericVector<Number> &residual,
			     NonlinearImplicitSystem &system)
{
  // Get a constant reference to the mesh object.
  const MeshBase& mesh = es.get_mesh();

  // The dimension that we are running
  const unsigned int dim = mesh.mesh_dimension();
  libmesh_assert_equal_to (dim, 2);

  // A reference to the \p DofMap object for this system.  The \p DofMap
  // object handles the index translation from node and element numbers
  // to degree of freedom numbers.  We will talk more about the \p DofMap
  // in future examples.
  const DofMap& dof_map = system.get_dof_map();

  // Get a constant reference to the Finite Element type
  // for the first (and only) variable in the system.
  FEType fe_type = dof_map.variable_type(0);

  // Build a Finite Element object of the specified type.  Since the
  // \p FEBase::build() member dynamically creates memory we will
  // store the object as an \p AutoPtr<FEBase>.  This can be thought
  // of as a pointer that will clean up after itself.
  AutoPtr<FEBase> fe (FEBase::build(dim, fe_type));

  // A 5th order Gauss quadrature rule for numerical integration.
  QGauss qrule (dim, FIFTH);

  // Tell the finite element object to use our quadrature rule.
  fe->attach_quadrature_rule (&qrule);

  // Declare a special finite element object for
  // boundary integration.
  AutoPtr<FEBase> fe_face (FEBase::build(dim, fe_type));

  // Boundary integration requires one quadraure rule,
  // with dimensionality one less than the dimensionality
  // of the element.
  QGauss qface(dim-1, FIFTH);

  // Tell the finte element object to use our
  // quadrature rule.
  fe_face->attach_quadrature_rule (&qface);

  // Here we define some references to cell-specific data that
  // will be used to assemble the linear system.
  // We begin with the element Jacobian * quadrature weight at each
  // integration point.
  const std::vector<Real>& JxW = fe->get_JxW();

  // The element shape functions evaluated at the quadrature points.
  const std::vector<std::vector<Real> >& phi = fe->get_phi();

  // The element shape function gradients evaluated at the quadrature
  // points.
  const std::vector<std::vector<RealGradient> >& dphi = fe->get_dphi();

  // Define data structures to contain the resdual contributions
  DenseVector<Number> Re;

  // This vector will hold the degree of freedom indices for
  // the element.  These define where in the global system
  // the element degrees of freedom get mapped.
  std::vector<dof_id_type> dof_indices;

  // Now we will loop over all the active elements in the mesh which
  // are local to this processor.
  // We will compute the element residual.
  residual.zero();

  MeshBase::const_element_iterator       el     = mesh.active_local_elements_begin();
  const MeshBase::const_element_iterator end_el = mesh.active_local_elements_end();

  for ( ; el != end_el; ++el)
    {
      // Store a pointer to the element we are currently
      // working on.  This allows for nicer syntax later.
      const Elem* elem = *el;

      // Get the degree of freedom indices for the
      // current element.  These define where in the global
      // matrix and right-hand-side this element will
      // contribute to.
      dof_map.dof_indices (elem, dof_indices);

      // Compute the element-specific data for the current
      // element.  This involves computing the location of the
      // quadrature points (q_point) and the shape functions
      // (phi, dphi) for the current element.
      fe->reinit (elem);

      // We use the resize member here because
      // the number of degrees of freedom might have changed from
      // the last element.  Note that this will be the case if the
      // element type is different (i.e. the last element was a
      // triangle, now we are on a quadrilateral).
      Re.resize (dof_indices.size());

      // Now we will build the residual. This involves
      // the construction of the matrix K and multiplication of it
      // with the current solution x. We rearrange this into two loops:
      // In the first, we calculate only the contribution of
      // K_ij*x_j which is independent of the row i. In the second loops,
      // we multiply with the row-dependent part and add it to the element
      // residual.

      for (unsigned int qp=0; qp<qrule.n_points(); qp++)
        {
          Number u = 0;
          Gradient grad_u;

          for (unsigned int j=0; j<phi.size(); j++)
            {
              u      += phi[j][qp]*soln(dof_indices[j]);
              grad_u += dphi[j][qp]*soln(dof_indices[j]);
            }

          const Number K = 1./std::sqrt(1. + grad_u*grad_u);

          for (unsigned int i=0; i<phi.size(); i++)
            Re(i) += JxW[qp]*(
                              K*(dphi[i][qp]*grad_u) +
                              kappa*phi[i][qp]*u
                              );
        }

      // At this point the interior element integration has
      // been completed.  However, we have not yet addressed
      // boundary conditions.

      // The following loops over the sides of the element.
      // If the element has no neighbor on a side then that
      // side MUST live on a boundary of the domain.
      for (unsigned int side=0; side<elem->n_sides(); side++)
        if (elem->neighbor(side) == NULL)
          {
            // The value of the shape functions at the quadrature
            // points.
            const std::vector<std::vector<Real> >&  phi_face = fe_face->get_phi();

            // The Jacobian * Quadrature Weight at the quadrature
            // points on the face.
            const std::vector<Real>& JxW_face = fe_face->get_JxW();

            // Compute the shape function values on the element face.
            fe_face->reinit(elem, side);

            // Loop over the face quadrature points for integration.
            for (unsigned int qp=0; qp<qface.n_points(); qp++)
              {
                // This is the right-hand-side contribution (f),
                // which has to be subtracted from the current residual
                for (unsigned int i=0; i<phi_face.size(); i++)
                  Re(i) -= JxW_face[qp]*sigma*phi_face[i][qp];
              }
          }

      dof_map.constrain_element_vector (Re, dof_indices);
      residual.add_vector (Re, dof_indices);
    }

  // That's it.
}
