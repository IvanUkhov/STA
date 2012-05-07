#include "TemperatureAnalysis.h"
#include "EigenvalueDecomposition.h"

TemperatureAnalysis::TemperatureAnalysis(const HotSpot &hotspot)
	: processor_count(hotspot.get_processor_count()),
	node_count(hotspot.get_node_count()),
	sampling_interval(hotspot.get_sampling_interval()),
	ambient_temperature(hotspot.get_ambient_temperature())
{
	matrix_t conductance;
	vector_t capacitance;

	hotspot.get_conductance(conductance);
	hotspot.get_capacitance(capacitance);

	size_t i, j;

	A.resize(node_count, node_count);
	B.resize(node_count, node_count);

	Cm12.resize(node_count);

	V.resize(node_count);
	U.resize(node_count, node_count);
	UT.resize(node_count, node_count);

	m_temp.resize(node_count, node_count);
	v_temp.resize(node_count);

	/* We have:
	 * C * dT/dt + G * T = P
	 */
	matrix_t &G = A;

	for (i = 0; i < node_count; i++) {
		for (j = 0; j < node_count; j++)
			G[i][j] = conductance[i][j];
		Cm12[i] = sqrt(1.0 / capacitance[i]);
	}

	matrix_t &Gt = G;

	/* We want to get rid of everything in front of dT/dt,
	 * but at the same time we want to keep the matrix in front of X
	 * symmetric, so we do the following substitution:
	 * T' = C^(1/2) * T
	 * G' = C^(-1/2) * G * C^(-1/2)
	 * P' = C^(-1/2) * P
	 *
	 * Eventually, we have:
	 * dT'/dt + G'T' = P'
	 */
	multiply_diagonal_matrix_matrix(Cm12, G, m_temp);
	multiply_matrix_diagonal_matrix(m_temp, Cm12, Gt);

	/* Eigenvalue decomposition:
	 * G' = U * V * U^T
	 */
	EigenvalueDecomposition S(Gt, U, V);

	transpose_matrix(U, UT);

	/* The first coefficient:
	 * A = exp(- Gt * t)
	 *   = - U * exp(V * t) * U^T
	 */
	for (i = 0; i < node_count; i++) v_temp[i] = - exp(sampling_interval * V[i]);
	multiply_matrix_diagonal_matrix(U, v_temp, m_temp);
	multiply_matrix_matrix(m_temp, UT, A);

	/* The second coefficient:
	 * B = - Gt^(-1) * (exp(- Gt * t) - I) * C^(-1/2) =
	 *   = U * diag((exp(t * l0) + 1) / l0, ...) * U^T * C^(-1/2)
	 */
	for (i = 0; i < node_count; i++) v_temp[i] = (v_temp[i] + 1) / V[i];
	multiply_matrix_diagonal_matrix(U, v_temp, m_temp);
	multiply_matrix_matrix_diagonal_matrix(m_temp, UT, Cm12, B);
}

void TransientTemperatureAnalysis::compute(const matrix_t &dynamic_power,
	matrix_t &temperature) const
{
	size_t step_count = dynamic_power.rows();
	size_t node_count = dynamic_power.cols();

	const double *P = dynamic_power.point();

	temperature.resize(dynamic_power);
	double *Tt = temperature.point();

	vector_t BP(node_count);

	/* The initial temperature is always zero.
	 * Let us treat this case separately.
	 *
	 * BP(0) = B * P(0)
	 * T'(0) = K * 0 + BP(0)
	 */
	multiply_matrix_incomplete_vector(B, P, processor_count, Tt);

	for (size_t i = 1; i < step_count; i++) {
		/* BP(i) = B * P(i) */
		multiply_matrix_incomplete_vector(B, P + i * processor_count,
			processor_count, BP);

		/* T'(i) = A * T'(i-1) + Q(i) */
		multiply_matrix_vector_plus_vector(A, Tt + (i - 1) * processor_count,
			BP, Tt + i * processor_count);
	}

	double *T = Tt;

	/* Return back to T' from T:
	 * T = C^(-1/2) * T'
	 *
	 * ... and do not forget about the ambience.
	 */
	for (size_t i = 0, k = 0; i < step_count; i++)
		for (size_t j = 0; j < processor_count; j++, k++)
			T[k] = Tt[k] * Cm12[j] + ambient_temperature;
}
