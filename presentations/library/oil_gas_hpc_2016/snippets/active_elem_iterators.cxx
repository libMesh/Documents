void foo (const MeshBase &mesh)
{
  // Now we will loop over all the elements in the mesh that
  // live on the local processor. We will compute the element
  // matrix and right-hand-side contribution.  Since the mesh
  // may be refined we want to only consider the ACTIVE elements,
  // hence we use a variant of the \p active_elem_iterator.
  MeshBase::const_element_iterator
    el     = mesh.active_local_elements_begin();
  const MeshBase::const_element_iterator
    end_el = mesh.active_local_elements_end(); 
  
  for ( ; el != end_el; ++el)
    {    
      // Store a pointer to the element we are currently
      // working on.  This allows for nicer syntax later.
      const Elem* elem = *el;
      ...
    }
  ...
}
