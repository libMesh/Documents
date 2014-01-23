// loop over a range and determine the bounding box
void bounding_box(const ConstNodeRange &range)
{      
  ...
  for (ConstNodeRange::const_iterator it = range.begin(); 
       it != range.end(); ++it)
    {
      const Node *node = *it;
      
      for (unsigned int i=0; i<LIBMESH_DIM; i++)
	{
	  _vmin[i] = std::min(_vmin[i], (*node)(i));
	  _vmax[i] = std::max(_vmax[i], (*node)(i));
	}
    }
  ...
}

...
// Query the number of DOFs for a particular node in a system
const unsigned int n_dofs_per_node = node->n_dofs(sys_num);
