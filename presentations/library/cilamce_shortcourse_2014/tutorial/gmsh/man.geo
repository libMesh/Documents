//
// man is 186.0566 lang... 
//
//Mesh.Algorithm3D   = 4 ;
Geometry.Tolerance = 1.e-6;
MeshSize =  2.  ;
StdSize  = 10.  ;

Merge "man_model.stl" ;

//
// een windtunneltje
//
Point(1) = { 1.5 * StdSize,-13.0, 1 * StdSize, MeshSize};
Point(2) = {-1.5 * StdSize,-13.0, 1 * StdSize, MeshSize};
Point(3) = {-1.5 * StdSize,-13.0,-2 * StdSize, MeshSize};
Point(4) = { 1.5 * StdSize,-13.0,-2 * StdSize, MeshSize};

Line(1)  = {1 , 2};
Line(2)  = {2 , 3};
Line(3)  = {3 , 4};
Line(4)  = {4 , 1};


Line Loop(1) = {1, 2, 3, 4};
Plane Surface(2) = {1};

Extrude{0, 3.0 * StdSize, 0 }{ Surface{2}; }
//Delete{ Volume{1}; }

Surface Loop(1) = {1};
Surface Loop(2) = { 2, 13, 17, 21, 25, 26};

//Volume(1) = {1};
Volume(2) = {2,1};

Physical Volume("Fluid")  = {2};

Physical Surface("Man  ") = {1};
Physical Surface("Floor") = {2};
Physical Surface("Inlet") = {13};
Physical Surface("Outlet") = {21};
Physical Surface("Walls") = {17, 25, 26};

