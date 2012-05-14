#include <string>
#include <cmath>

#include "Variation.h"

Variation::Variation(size_t processor_count, size_t node_count, const std::string &filename)
	: _processor_count(processor_count), _node_count(node_count)
{
	Configuration config(filename);

	matrix_t &Mtemp2 = GammaSdyn;

	matrix_t &Sdyn = Mtemp;

	/* We have:
	 * Sdyn -- the correlation matrix of the dynamic power,
	 * Kdyn -- the variation ratio of the dynamic power.
	 */

	size_t i;
	size_t count = config.size();

	for (i = 0; i < count; i++) {
		if (config[i].name == "dynamic_ratio")
			config[i].to_vector(Kdyn);
		else if (config[i].name == "dynamic_correlation")
			config[i].to_matrix(Sdyn);
	}

	/* Validate! */
	if (_processor_count != Kdyn.size())
		throw std::runtime_error("The dynamic variation ratio vector is invalid.");
	if (_processor_count != Sdyn.rows() ||
		_processor_count != Sdyn.cols())
		throw std::runtime_error("The dynamic correlation matrix is invalid.");

	/* Because of 'extend' later, we do not need to do this:
	 * Mtemp.resize(_processor_count, _processor_count);
	 *
	 * It is the same as Sdyn and GammaSdyn.
	 */
	Mtemp2.resize(_node_count, _node_count);
	Vtemp.resize(_node_count);

	/* Extend to the dimensions of the circuit,
	 * fill the diagonal with 1s, everywhere else -- with 0s.
	 */
	Sdyn.extend(_node_count, _node_count, true /* nullify */);
	for (i = _processor_count; i < _node_count; i++)
		Sdyn[i][i] = 1;

	/* Gamma(Sdyn) = U * V^(1/2) */
	EigenvalueDecomposition decomposition(Sdyn, Mtemp2, Vtemp);

	for (i = 0; i < _node_count; i++)
		Vtemp[i] = std::sqrt(Vtemp[i]);

	multiply_matrix_diagonal_matrix(Mtemp2, Vtemp, GammaSdyn);
}

/* Ldyn = diag(E(Pdyn)) * Kdyn * Gamma(Sdyn)
 *
 * where:
 *
 * E(Pdyn) -- the vector of the nominal power consumption,
 * Kdyn -- the variation ratio vector of the dynamic power,
 * Gamma -- the eigenvalue decomposition operator (U * V^(1/2)),
 * Sdyn -- the correlation matrix of the dynamic power.
 */

void Variation::compute_initial_variance(
	const matrix_t &B, const matrix_t &BT, const double *Pdyn, matrix_t &Var)
{
	matrix_t &Ldyn = Mtemp;
	matrix_t &Ldyn2 = Var;

	/* Variance:
	 *
	 * Cov(T'(0)) = <A, 0> + <B, Ldyn(0)^2> + <<A, 0 * Ldyn(0), B>>
	 *            = <B, Ldyn(0)^2>
	 */

	/* Ldyn(0)^2 */
	multiply_two_incomplete_bottom_diagonal_matrices_matrix(
		Pdyn, Kdyn, GammaSdyn, _processor_count, Ldyn);

	/* B * Ldyn(0)^2 * B^T */
	multiply_two_incomplete_bottom_matrices(
		Ldyn, Ldyn, _processor_count, _processor_count, Ldyn2);
	multiply_incomplete_bottom_matrix_matrix(Ldyn2, BT, _processor_count, Mtemp);
	multiply_matrix_incomplete_bottom_matrix(B, Mtemp, _processor_count, Var);
}
