void foo (const MeshBase &mesh)
{
  // Loop over all nodes to operate on their geometry.
  for (const Node * node : mesh.node_ptr_range())
    function_taking_points(*node);

  // "local" nodes is a useful but tricky concept: a local element may
  // include a non-local node!
  for (const Node * node : mesh.local_node_ptr_range())
    function_taking_dofobjects(*node);

  // "active" nodes is an unused concept: the same node may
  // simultaneously be included by active and ancestor and "subactive"
  // elements.
}
