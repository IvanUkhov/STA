#include "Solution.h"
#include "EigenvalueDecomposition.h"

Solution::Solution(
	size_t _processor_count, size_t _node_count,
	double _sampling_interval, double _ambient_temperature,
	const matrix_t &conductivity, const vector_t &capacitance) :

	processor_count(_processor_count),
	node_count(_node_count),
	sampling_interval(_sampling_interval),
	ambient_temperature(_ambient_temperature)
{
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
			G[i][j] = conductivity[i][j];
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
