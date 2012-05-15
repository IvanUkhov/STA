#include "TransientTemperatureAnalysis.h"

void TransientTemperatureAnalysis::perform(const matrix_t &dynamic_power,
	matrix_t &temperature)
{
	size_t step_count = dynamic_power.rows();

	if (dynamic_power.cols() != _processor_count || step_count == 0)
		throw std::runtime_error("The power profile has an invalid size.");

	const double *P = dynamic_power;

	temperature.resize(dynamic_power);

	double *T = temperature;

	size_t i, j, k;

	/* The initial temperature is always zero.
	 * Let us treat this case separately.
	 */

	/* Expectation:
	 *
	 * BP(0) = B * P(0)
	 * E(T'(0)) = A * 0 + BP(0)
	 */
	multiply_matrix_incomplete_vector(B, P, _processor_count, Tlast);

	/* Variance:
	 *
	 */

	/* Return back form T from T':
	 * E(T) = C^(-1/2) * E(T')
	 *
	 * ... and do not forget about the ambience.
	 */
	for (j = 0, k = 0; j < _processor_count; j++, k++)
		T[k] = Tlast[j] * Cm12[j] + _ambient_temperature - KELVIN;

	/* Repeat for the rest */
	for (i = 1; i < step_count; i++) {
		/* BP(i) = B * P(i) */
		multiply_matrix_incomplete_vector(B, P + i * _processor_count,
			_processor_count, BP);

		/* E(T'(i)) = A * E(T'(i-1)) + Q(i) */
		multiply_matrix_vector_plus_vector(A, Tlast, BP, Tnext);

		/* Return back */
		for (j = 0; j < _processor_count; j++, k++)
			T[k] = Tnext[j] * Cm12[j] + _ambient_temperature - KELVIN;

		/* Swap */
		double *_ = Tlast;
		Tlast = Tnext;
		Tnext = _;
	}
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

void TransientTemperatureAnalysis::compute_initial_covariance(
	const double *Pdyn, matrix_t &Cov)
{
	matrix_t &Ldyn = Mtemp;
	matrix_t &Ldyn2 = Cov;

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
	multiply_matrix_incomplete_bottom_matrix(B, Mtemp, _processor_count, Cov);
}
