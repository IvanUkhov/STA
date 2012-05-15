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

	vector_t *Tlast = &Vtemp1;
	vector_t *Tnext = &Vtemp2;

	vector_t &BP = Vtemp3;

	matrix_t &CovT = Mtemp5;
	matrix_t &CovTSigma = Mtemp6;

	/* The initial temperature is always zero.
	 * Let us treat this case separately.
	 */

	/* BP(0) = B * P(0)
	 * E(T'(0)) = A * 0 + BP(0)
	 */
	multiply_matrix_incomplete_vector(B, P, _processor_count, *Tlast);

	CovT.nullify();
	CovTSigma.nullify();

	update_covariance(CovT, CovTSigma, P, true /* initial */,
		Mtemp1 /* temp */, Mtemp2 /* temp */, Mtemp3 /* temp */, Mtemp4 /* temp */);

	/* Return back form T from T':
	 * E(T) = C^(-1/2) * E(T')
	 *
	 * ... and do not forget about the ambience.
	 */
	for (j = 0, k = 0; j < _processor_count; j++, k++)
		T[k] = (*Tlast)[j] * Cm12[j] + _ambient_temperature - KELVIN;

	/* Repeat for the rest */
	for (i = 1; i < step_count; i++) {
		/* BP(i) = B * P(i) */
		multiply_matrix_incomplete_vector(B, P + i * _processor_count,
			_processor_count, BP);

		/* E(T'(i)) = A * E(T'(i-1)) + Q(i) */
		multiply_matrix_vector_plus_vector(A, *Tlast, BP, *Tnext);

		update_covariance(CovT, CovTSigma, P + i * _processor_count, false /* not initial */,
			Mtemp1 /* temp */, Mtemp2 /* temp */, Mtemp3 /* temp */, Mtemp4 /* temp */);

		/* Return back */
		for (j = 0; j < _processor_count; j++, k++)
			T[k] = (*Tnext)[j] * Cm12[j] + _ambient_temperature - KELVIN;

		/* Swap */
		vector_t *_ = Tlast;
		Tlast = Tnext;
		Tnext = _;
	}
}

void TransientTemperatureAnalysis::update_covariance(
	matrix_t &CovT, matrix_t &CovTSigma, const double *Pdyn, bool initial,
	matrix_t &Mtemp1, matrix_t &Mtemp2, matrix_t &Mtemp3, matrix_t &Mtemp4) const
{
	/* Cov(T'(i+1)) = <A, Cov(T'(i))> + <B, Ldyn(i)^2>
	 *              + <<A, Cov(T'(i), Sigma) * Ldyn(i), B>>
	 *
	 * where:
	 *
	 * Sigma -- is a multidimensional normal random variable.
	 */

	/* Ldyn = diag(E(Pdyn)) * Kdyn * Gamma(Sdyn)
	 *
	 * where:
	 *
	 * E(Pdyn) -- the vector of the nominal power consumption,
	 * Kdyn -- the variation ratio vector of the dynamic power,
	 * Gamma -- the eigenvalue decomposition operator (U * V^(1/2)),
	 * Sdyn -- the correlation matrix of the dynamic power.
	 */

	/* Ldyn(i) */
	matrix_t &Ldyn = Mtemp1;
	multiply_two_incomplete_bottom_diagonal_matrices_matrix(
		Pdyn, Kdyn, GammaSdyn, _processor_count, Ldyn);

	/* Ldyn(i)^2 */
	matrix_t &Ldyn2 = Mtemp2;
	multiply_two_incomplete_bottom_matrices(
		Ldyn, Ldyn, _processor_count, _processor_count, Ldyn2);

	/* B * Ldyn(i)^2 * B^T */
	multiply_incomplete_bottom_matrix_matrix(Ldyn2, BT, _processor_count, Mtemp4);
	/* + 1 */ multiply_matrix_incomplete_bottom_matrix(B, Mtemp4, _processor_count, CovT);

	if (initial) {
		/* Cov(T'(1), Sigma) = A * Cov(T'(0), Sigma) + B * Ldyn(i)
		 *                   = B * Ldyn(0)
		 */
		multiply_matrix_incomplete_bottom_matrix(B, Ldyn, _processor_count, CovTSigma);
		return;
	}

	/* A * Cov(T'(i)) * A^T */
	multiply_matrix_matrix(A, CovT, Mtemp1);
	multiply_matrix_matrix(Mtemp1, AT, Mtemp2);

	/* +2 */ add_matrix_matrix(CovT, Mtemp2, CovT);

	/* Ldyn(i) * B^T */
	multiply_incomplete_bottom_matrix_matrix(
		Ldyn, BT, _processor_count, Mtemp2);

	/* Cov(T'(i), Sigma) * Ldyn(i) * B^T*/
	multiply_matrix_incomplete_bottom_matrix(
		CovTSigma, Mtemp2, _processor_count, Mtemp3);

	/* A * Cov(T'(i), Sigma) * Ldyn(i) * B^T */
	matrix_t &ACovTSigmaLdynBT = Mtemp3;
	multiply_matrix_matrix(A, Mtemp3, ACovTSigmaLdynBT);

	/* +3 */ add_matrix_matrix(CovT, ACovTSigmaLdynBT, CovT);

	matrix_t &BLdynCovTSigmaTAT = Mtemp2;
	transpose_matrix(ACovTSigmaLdynBT, BLdynCovTSigmaTAT);

	/* +4 */ add_matrix_matrix(CovT, BLdynCovTSigmaTAT, CovT);

	/* Cov(T'(i+1), Sigma) = A * Cov(T'(i), Sigma) + B * Ldyn(i) */
	multiply_matrix_matrix(A, CovTSigma, Mtemp2);
	multiply_matrix_incomplete_bottom_matrix(B, Ldyn, _processor_count, Mtemp3);
	add_matrix_matrix(Mtemp2, Mtemp3, CovTSigma);
}
