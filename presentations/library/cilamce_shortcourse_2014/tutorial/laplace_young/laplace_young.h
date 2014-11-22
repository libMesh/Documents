#ifndef LAPLACE_YOUNG_H
#define LAPLACE_YOUNG_H

#include "libmesh/equation_systems.h"
#include "libmesh/nonlinear_implicit_system.h"

class LaplaceYoung : public NonlinearImplicitSystem::ComputeJacobian,
		     public NonlinearImplicitSystem::ComputeResidual
{
public:
  
  LaplaceYoung (EquationSystems &es_in) :
    es(es_in)
  {}

  virtual void jacobian (const NumericVector<Number>& X,
			 SparseMatrix<Number>& J,
			 NonlinearImplicitSystem& S);

  virtual void residual (const NumericVector<Number>& X,
			 NumericVector<Number>& R,
			 NonlinearImplicitSystem& S); 

private:
  EquationSystems &es;
};

#endif
