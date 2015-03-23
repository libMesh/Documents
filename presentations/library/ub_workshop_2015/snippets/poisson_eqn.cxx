// We now define the matrix assembly function for the Poisson system
// by computing the element matrices and right-hand sides.  We are
// omitting BCs for brevity.
void assemble_poisson(EquationSystems& es,
                      const std::string& system_name)
{
  // Get a constant reference to the mesh object.
  const MeshBase& mesh = es.get_mesh();

  // The dimension that we are running
  const unsigned int dim = mesh.mesh_dimension();

  // Get a reference to the LinearImplicitSystem we are solving
  LinearImplicitSystem& system = es.get_system<LinearImplicitSystem>("Poisson");
  
  // A reference to the DofMap object for this system.  The DofMap
  // object handles the index translation from node and element
  // numbers to degree of freedom numbers.  We will talk more about
  // the DofMap in future examples.
  const DofMap& dof_map = system.get_dof_map();

  // Get a constant reference to the Finite Element type for the first
  // (and only) variable in the system.
  FEType fe_type = dof_map.variable_type(0);

  // Build a Finite Element object of the specified type.  Since the
  // FEBase::build() member dynamically creates memory we will store
  // the object as an AutoPtr<FEBase>.  This can be thought of as a
  // pointer that will clean up after itself.
  AutoPtr<FEBase> fe (FEBase::build(dim, fe_type));
  
  // A 5th order Gauss quadrature rule for numerical integration.
  QGauss qrule (dim, FIFTH);

  // Tell the finite element object to use our quadrature rule.
  fe->attach_quadrature_rule (&qrule);

  // Here we define some references to cell-specific data that will be
  // used to assemble the linear system.  We begin with the element
  // Jacobian * quadrature weight at each integration point.
  const std::vector<Real>& JxW = fe->get_JxW();

  // The physical XY locations of the quadrature points on the
  // element.  These might be useful for evaluating spatially varying
  // material properties at the quadrature points.
  const std::vector<Point>& q_point = fe->get_xyz();

  // The element shape functions evaluated at the quadrature points.
  const std::vector<std::vector<Real> >& phi = fe->get_phi();

  // The element shape function gradients evaluated at the quadrature
  // points.
  const std::vector<std::vector<RealGradient> >& dphi = fe->get_dphi();

  // Define data structures to contain the element matrix and
  // right-hand-side vector contribution.  Following basic finite
  // element terminology we will denote these "Ke" and "Fe".
  DenseMatrix<Number> Ke;
  DenseVector<Number> Fe;

  // This vector will hold the degree of freedom indices for the
  // element.  These define where in the global system the element
  // degrees of freedom get mapped.
  std::vector<dof_id_type> dof_indices;

  // Now we will loop over all the elements in the mesh.  We will
  // compute the element matrix and right-hand-side contribution.  See
  // example 3 for a discussion of the element iterators.
  MeshBase::const_element_iterator       el     = mesh.active_local_elements_begin();
  const MeshBase::const_element_iterator end_el = mesh.active_local_elements_end();

  for ( ; el != end_el; ++el)
    {
      // Store a pointer to the element we are currently working on.
      // This allows for nicer syntax later.
      const Elem* elem = *el;

      // Get the degree of freedom indices for the current element.
      // These define where in the global matrix and right-hand-side
      // this element will contribute to.
      dof_map.dof_indices (elem, dof_indices);

      // Compute the element-specific data for the current element.
      // This involves computing the location of the quadrature points
      // (q_point) and the shape functions (phi, dphi) for the current
      // element.
      fe->reinit (elem);

      // Zero the element matrix and right-hand side before summing
      // them.  We use the resize member here because the number of
      // degrees of freedom might have changed from the last element.
      // Note that this will be the case if the element type is
      // different (i.e. the last element was a triangle, now we are
      // on a quadrilateral).
      Ke.resize (dof_indices.size(),
                 dof_indices.size());

      Fe.resize (dof_indices.size());
 
      // Now we will build the element matrix.  This involves a double
      // loop to integrate the test funcions (i) against the trial
      // functions (j).
      for (unsigned int qp=0; qp<qrule.n_points(); qp++)
        for (unsigned int i=0; i<phi.size(); i++)
          for (unsigned int j=0; j<phi.size(); j++)
            Ke(i,j) += JxW[qp]*(dphi[i][qp]*dphi[j][qp]);

      // Now we build the element right-hand-side contribution.  This
      // involves a single loop in which we integrate the "forcing
      // function" in the PDE against the test functions.
      for (unsigned int qp=0; qp<qrule.n_points(); qp++)
	for (unsigned int i=0; i<phi.size(); i++)
	  Fe(i) += JxW[qp]*10.*phi[i][qp];          
      
      // If we are using an adaptive mesh this will apply any hanging
      // node constraint equations
      dof_map.heterogenously_constrain_element_matrix_and_vector (Ke, Fe, dof_indices);

      // The element matrix and right-hand-side are now built for this
      // element.  Add them to the global matrix and right-hand-side
      // vector.  The SparseMatrix::add_matrix() and
      // NumericVector::add_vector() members do this for us.
      system.matrix->add_matrix (Ke, dof_indices);
      system.rhs->add_vector    (Fe, dof_indices);
    }
}
