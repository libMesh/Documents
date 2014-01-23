int main (int argc, char** argv)
{
  // Initialize the library.  This is necessary because the library
  // may depend on a number of other libraries (i.e. MPI and PETSc)
  // that require initialization before use.  When the LibMeshInit
  // object goes out of scope, other libraries and resources are
  // finalized.
  LibMeshInit init (argc, argv);

  // Create a mesh
  Mesh mesh;
  
  // Read the input mesh.
  mesh.read ("in.exo");
  
  // Print information about the mesh to the screen.
  mesh.print_info();
  
  // Write the output mesh
  mesh.write ("out.dat");

  ...
}
