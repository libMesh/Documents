
// Examine the element subdomain 
const subdomain_id_type sid = elem->subdomain_id();

// Perform some operation for an element's boundary sides
for (const auto s : make_range(elem.n_sides()))
  {
    // Every local or ghosted element knows its neigbor.  If it has no
    // neighbor, then it lies on a physical boundary.
    if (!elem.neighbor_ptr(s))
      {
	// Construct the side as a full temporary element
        std::unique_ptr<const Elem> side = elem.build_side_ptr(s);

        unsigned int n_side_dofs = 0;

        // Loop over side nodes, add up degrees of freedom
        for (const Node & node : side->node_ref_range())
          n_side_dofs += node.n_comp(sys_num, var_num);
      }
  }
