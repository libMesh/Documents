  ...
  // Create an equation systems object. This object can contain
  // multiple systems of different flavors for solving loosely coupled
  // physics.  Each system can contain multiple variables of different
  // approximation orders. The EquationSystems object needs a
  // reference to the mesh object, so the order of construction here
  // is important.
  EquationSystems equation_systems (mesh);

  // Now we declare the system and its variables.  We begin by adding
  // a "TransientLinearImplicitSystem" to the EquationSystems object,
  // and we give it the name "Simple System".
  equation_systems.add_system<TransientLinearImplicitSystem> ("Simple System");

  // Adds the variable "u" to "Simple System".  "u" will be
  // approximated using first-order approximation.
  equation_systems.get_system("Simple System").add_variable("u", FIRST);

  // Next we'll by add an "ExplicitSystem" to the EquationSystems
  // object, and we give it the name "Complex System".
  equation_systems.add_system<ExplicitSystem> ("Complex System");

  // Give "Complex System" three variables -- each with a different
  // approximation order.  Variables "c" and "T" will use first-order
  // Lagrange approximation, while variable "dv" will use a
  // second-order discontinuous approximation space.
  equation_systems.get_system("Complex System").add_variable("c", FIRST);
  equation_systems.get_system("Complex System").add_variable("T", FIRST);
  equation_systems.get_system("Complex System").add_variable("dv", SECOND, MONOMIAL);

  // Initialize the data structures for the equation system.
  equation_systems.init();

  // Prints information about the system to the screen.
  equation_systems.print_info();
  ...
