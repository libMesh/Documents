// access each Node on the element
for (unsigned int n=0; n<elem->n_nodes(); n++)
  {
    const Node *node = elem->get_node(n);

    // get a user-specified material property, based on
    // the subdomain the element belongs to
    const Real k_diff = my_matprop_func (elem->subdomain_id(), *node);
    ...
  }


// Perform some operation for elements on the boundary
for (unsigned int side=0; side<elem->n_sides(); side++)
  {
    // Every element knows its neigbor.  If it has no neighbor,
    // then it lies on a physical boundary.
    if (elem->neighbor(side) == NULL)
      {
	// Construct the side as a lower dimensional element
        AutoPtr<Elem> elem_side (elem->build_side(side));
	...
      }
    ...
  }
