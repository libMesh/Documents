void foo (const MeshBase & mesh)
{
  // Loop over all active elements owned by the local processor
  for (const Elem * elem : mesh.active_local_element_ptr_range())
    {
      // Loop over all the element's nodes
      for (const Node & node : elem->node_ref_range())
        bar(node);

      // Loop over all the element's neighbors
      for (const Elem * neigh : elem->neighbor_ptr_range())
        baz(neigh);
    }

  // Manual loop over the same elements
  for (auto elem_it = mesh.active_local_elements_begin(),
            end_it  = mesh.active_local_elements_end();
       elem_it != end_it;)
    {
      Elem & elem = **elem_it;
      manually_increment(elem_it);
    }
}
