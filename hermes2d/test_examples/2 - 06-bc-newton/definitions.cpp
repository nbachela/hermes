#include "definitions.h"

/* Weak forms */

CustomWeakFormPoissonNewton::CustomWeakFormPoissonNewton(std::string mat_al, HermesFunction<double>* lambda_al,
                                                         std::string mat_cu, HermesFunction<double>* lambda_cu,
                                                         HermesFunction<double>* vol_src_term, std::string bdy_heat_flux,
                                                         double alpha, double t_exterior) : WeakForm(1)
{
  // Jacobian forms - volumetric.
  add_matrix_form(new WeakFormsH1::DefaultJacobianDiffusion<double>(0, 0, mat_al, lambda_al));
  add_matrix_form(new WeakFormsH1::DefaultJacobianDiffusion<double>(0, 0, mat_cu, lambda_cu));

  // Jacobian forms - surface.
  add_matrix_form_surf(new WeakFormsH1::DefaultMatrixFormSurf<double>(0, 0, bdy_heat_flux, new HermesFunction<double>(alpha)));

  // Residual forms - volumetric.
  add_vector_form(new WeakFormsH1::DefaultResidualDiffusion<double>(0, mat_al, lambda_al));
  add_vector_form(new WeakFormsH1::DefaultResidualDiffusion<double>(0, mat_cu, lambda_cu));
  add_vector_form(new WeakFormsH1::DefaultVectorFormVol<double>(0, HERMES_ANY, vol_src_term));

  // Residual forms - surface.
  add_vector_form_surf(new WeakFormsH1::DefaultResidualSurf<double>(0, bdy_heat_flux, new HermesFunction<double>(alpha)));
  add_vector_form_surf(new WeakFormsH1::DefaultVectorFormSurf<double>(0, bdy_heat_flux, new HermesFunction<double>(-alpha * t_exterior)));
};

/* Custom non-constant Dirichlet condition */

CustomDirichletCondition::CustomDirichletCondition(Hermes::vector<std::string> markers, 
                                                   double A, double B, double C)
    : EssentialBoundaryCondition(markers), A(A), B(B), C(C) 
{ 
}

EssentialBoundaryCondition<double>::EssentialBCValueType CustomDirichletCondition::get_value_type() const
{ 
  return EssentialBoundaryCondition::BC_FUNCTION; 
}

double CustomDirichletCondition::value(double x, double y, double n_x, double n_y, double t_x, double t_y) const 
{
  return A*x + B*y + C;
}

