  ...
  // Construct an EquationSystems with the mesh it will annotate
  EquationSystems es (mesh);

  // Create a "TransientLinearImplicitSystem" owned by es
  es.add_system<TransientLinearImplicitSystem> ("Simple System");

  // Add the variable "u" to "Simple System", approximated as a
  // first-order Lagrange (one DoF per vertex) field.
  es.get_system("Simple System").add_variable("u", FIRST);

  // Create an "ExplicitSystem" owned by es; hang on to a reference.
  System & cs = es.add_system<ExplicitSystem> ("Complex System");

  // Give "Complex System" three variables.  Variables "c" and "T"
  // will be third-order with a hierarchic basis, while variable "dv"
  // will be second-order and discontinuous.
  cs.add_variable("c", THIRD, HIERARCHIC);
  cs.add_variable("T", THIRD, HIERARCHIC);
  cs.add_variable("dv", SECOND, MONOMIAL);

  // Initialize the data structures for all systems
  es.init();

  // Print information about the systems to the screen.
  es.print_info();
  ...
