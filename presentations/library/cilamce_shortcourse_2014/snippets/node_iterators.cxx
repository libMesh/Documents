void foo (const MeshBase &mesh)
{
  // We will now loop over all nodes.
  MeshBase::const_node_iterator       node_it  = mesh.nodes_begin();
  const MeshBase::const_node_iterator node_end = mesh.nodes_end();
  
  for ( ; node_it != node_end; ++node_it)
    {
      // the current node pointer
      const Node* node = *node_it;
      ...
    }
  ...
}
