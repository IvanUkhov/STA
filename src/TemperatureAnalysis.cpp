#include "TemperatureAnalysis.h"
#include "EigenvalueDecomposition.h"
#include "File.h"

TemperatureAnalysis::TemperatureAnalysis(const HotSpot &hotspot)
	: _processor_count(hotspot.processor_count()),
	_node_count(hotspot.node_count()),
	_sampling_interval(hotspot.sampling_interval()),
	_ambient_temperature(hotspot.ambient_temperature())
{
	A.resize(_node_count, _node_count);
	AT.resize(_node_count, _node_count);

	B.resize(_node_count, _node_count);
	BT.resize(_node_count, _node_count);

	Cm12.resize(_node_count);

	V.resize(_node_count);
	U.resize(_node_count, _node_count);
	UT.resize(_node_count, _node_count);

	Mtemp.resize(_node_count, _node_count);
	Vtemp.resize(_node_count);

	/* We have:
	 * C * dT/dt + G * T = P
	 */

	matrix_t &G = A;
	vector_t &C = Cm12;

	hotspot.fill_conductance(G);
	hotspot.fill_capacitance(C);

	size_t i;

	for (i = 0; i < _node_count; i++)
		Cm12[i] = sqrt(1.0 / C[i]);

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
	matrix_t &Gt = G;
	multiply_diagonal_matrix_matrix(Cm12, G, Mtemp);
	multiply_matrix_diagonal_matrix(Mtemp, Cm12, Gt);

	/* Eigenvalue decomposition:
	 * G' = U * V * U^T
	 */
	EigenvalueDecomposition decomposition(Gt, U, V);

	transpose_matrix(U, UT);

	/* The first coefficient:
	 * A = exp(- Gt * t)
	 *   = - U * exp(V * t) * U^T
	 */
	for (i = 0; i < _node_count; i++)
		Vtemp[i] = exp(- _sampling_interval * V[i]);
	multiply_matrix_diagonal_matrix(U, Vtemp, Mtemp);
	multiply_matrix_matrix(Mtemp, UT, A);

	transpose_matrix(A, AT);

	/* The second coefficient:
	 * B = - Gt^(-1) * (exp(- Gt * t) - I) * C^(-1/2) =
	 *   = U * diag(- (exp(- t * l0) - 1) / l0, ...) * U^T * C^(-1/2)
	 */
	for (i = 0; i < _node_count; i++)
		Vtemp[i] = - (Vtemp[i] - 1) / V[i];
	multiply_matrix_diagonal_matrix(U, Vtemp, Mtemp);
	multiply_matrix_matrix_diagonal_matrix(Mtemp, UT, Cm12, B);

	transpose_matrix(B, BT);
}

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
