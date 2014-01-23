#include "poisson_problem.h"



void Poisson::assemble ()
{
  const MeshBase& mesh = es.get_mesh();

  ConstElemRange assembly_elem_range (mesh.active_local_elements_begin(),
				      mesh.active_local_elements_end());

  PerfLog perf_log ("Matrix Assembly");
  perf_log.push ("assembly loop");
  Threads::parallel_for (
			 // the range over which we will perform threaded operations
			 assembly_elem_range,

			 // the function object to apply to each element in the range
			 *this);

  perf_log.pop("assembly loop");
}


void Poisson::operator()(const ConstElemRange &range) const
{
    // Get a constant reference to the mesh object.
  const MeshBase& mesh = es.get_mesh();

  // The dimension that we are running
  const unsigned int dim = mesh.mesh_dimension();

  // Get a reference to the LinearImplicitSystem we are solving
  LinearImplicitSystem& system = es.get_system<LinearImplicitSystem>("Poisson");

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

  // The physical XY locations of the quadrature points on the element.
  // These might be useful for evaluating spatially varying material
  // properties at the quadrature points.
  const std::vector<Point>& q_point = fe->get_xyz();

  // The element shape functions evaluated at the quadrature points.
  const std::vector<std::vector<Real> >& phi = fe->get_phi();

  // The element shape function gradients evaluated at the quadrature
  // points.
  const std::vector<std::vector<RealGradient> >& dphi = fe->get_dphi();

  // Define data structures to contain the element matrix
  // and right-hand-side vector contribution.  Following
  // basic finite element terminology we will denote these
  // "Ke" and "Fe". More detail is in example 3.
  DenseMatrix<Number> Ke;
  DenseVector<Number> Fe;

  // This vector will hold the degree of freedom indices for
  // the element.  These define where in the global system
  // the element degrees of freedom get mapped.
  std::vector<dof_id_type> dof_indices;

  // Now we will loop over all the elements in the mesh.
  // We will compute the element matrix and right-hand-side
  // contribution.  See example 3 for a discussion of the
  // element iterators.
  for (ConstElemRange::const_iterator el = range.begin(); el != range.end(); ++el)
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

      // Zero the element matrix and right-hand side before
      // summing them.  We use the resize member here because
      // the number of degrees of freedom might have changed from
      // the last element.  Note that this will be the case if the
      // element type is different (i.e. the last element was a
      // triangle, now we are on a quadrilateral).
      Ke.resize (dof_indices.size(),
                 dof_indices.size());

      Fe.resize (dof_indices.size());

      // Now we will build the element matrix.  This involves
      // a double loop to integrate the test funcions (i) against
      // the trial functions (j).
      //
      // We have split the numeric integration into two loops
      // so that we can log the matrix and right-hand-side
      // computation seperately.

      for (unsigned int qp=0; qp<qrule.n_points(); qp++)
        for (unsigned int i=0; i<phi.size(); i++)
          for (unsigned int j=0; j<phi.size(); j++)
            Ke(i,j) += JxW[qp]*(dphi[i][qp]*dphi[j][qp]);

      // Now we build the element right-hand-side contribution.
      // This involves a single loop in which we integrate the
      // "forcing function" in the PDE against the test functions.

      for (unsigned int qp=0; qp<qrule.n_points(); qp++)
        {
          // fxy is the forcing function for the Poisson equation.
          // In this case we set fxy to be a finite difference
          // Laplacian approximation to the (known) exact solution.
          //
          // We will use the second-order accurate FD Laplacian
          // approximation, which in 2D on a structured grid is
          //
          // u_xx + u_yy = (u(i-1,j) + u(i+1,j) +
          //                u(i,j-1) + u(i,j+1) +
          //                -4*u(i,j))/h^2
          //
          // Since the value of the forcing function depends only
          // on the location of the quadrature point (q_point[qp])
          // we will compute it here, outside of the i-loop
          const Real x = q_point[qp](0);
#if LIBMESH_DIM > 1
          const Real y = q_point[qp](1);
#else
          const Real y = 0.;
#endif
#if LIBMESH_DIM > 2
          const Real z = q_point[qp](2);
#else
          const Real z = 0.;
#endif
          const Real eps = 1.e-3;

          const Real uxx = (exact_solution(x-eps,y,z) +
                            exact_solution(x+eps,y,z) +
                            -2.*exact_solution(x,y,z))/eps/eps;

          const Real uyy = (exact_solution(x,y-eps,z) +
                            exact_solution(x,y+eps,z) +
                            -2.*exact_solution(x,y,z))/eps/eps;

          const Real uzz = (exact_solution(x,y,z-eps) +
                            exact_solution(x,y,z+eps) +
                            -2.*exact_solution(x,y,z))/eps/eps;

          Real fxy;
          if(dim==1)
          {
            // In 1D, compute the rhs by differentiating the
            // exact solution twice.
            const Real pi = libMesh::pi;
            fxy = (0.25*pi*pi)*sin(.5*pi*x);
          }
          else
          {
            fxy = - (uxx + uyy + ((dim==2) ? 0. : uzz));
          }

          // Add the RHS contribution
          for (unsigned int i=0; i<phi.size(); i++)
            Fe(i) += JxW[qp]*fxy*phi[i][qp];
        }

      // If this assembly program were to be used on an adaptive mesh,
      // we would have to apply any hanging node constraint equations
      // Also, note that here we call heterogenously_constrain_element_matrix_and_vector
      // to impose a inhomogeneous Dirichlet boundary conditions.
      dof_map.heterogenously_constrain_element_matrix_and_vector (Ke, Fe, dof_indices);

      // The element matrix and right-hand-side are now built
      // for this element.  Add them to the global matrix and
      // right-hand-side vector.  The \p SparseMatrix::add_matrix()
      // and \p NumericVector::add_vector() members do this for us.
      {
	Threads::spin_mutex::scoped_lock lock(assembly_mutex);

	system.matrix->add_matrix (Ke, dof_indices);
	system.rhs->add_vector    (Fe, dof_indices);
      }
    }
}
