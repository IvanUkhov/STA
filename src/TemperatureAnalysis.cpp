#include "TemperatureAnalysis.h"
#include "EigenvalueDecomposition.h"

TemperatureAnalysis::TemperatureAnalysis(const HotSpot &hotspot,
	const Variation &variation)

	: _processor_count(hotspot.processor_count()),
	_node_count(hotspot.node_count()),
	_sampling_interval(hotspot.sampling_interval()),
	_ambient_temperature(hotspot.ambient_temperature())
{
	prepare_memory();
	prepare_thermal_coefficients(hotspot);
	prepare_variations(variation);
}

void TemperatureAnalysis::prepare_memory()
{
	/* Main matrices and vectors */
	Cm12.resize(_node_count);

	A.resize(_node_count, _node_count);
	AT.resize(_node_count, _node_count);

	B.resize(_node_count, _node_count);
	BT.resize(_node_count, _node_count);

	V.resize(_node_count);
	U.resize(_node_count, _node_count);
	UT.resize(_node_count, _node_count);

	/* Variations */
	GammaSdyn.resize(_node_count, _node_count);

	/* Auxiliary space */
	Mtemp.resize(_node_count, _node_count);
	Vtemp.resize(_node_count);
}

void TemperatureAnalysis::prepare_thermal_coefficients(const HotSpot &hotspot)
{
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

void TemperatureAnalysis::prepare_variations(const Variation &variation)
{
	size_t i;

	/* We have:
	 * Kdyn -- the variation ratio of the dynamic power.
	 * Sdyn -- the correlation matrix of the dynamic power,
	 */
	Kdyn.clone(variation.dynamic_ratio);

	matrix_t &Sdyn = Mtemp;
	matrix_t &Mtemp2 = GammaSdyn;

	/* Extend the correlation matrix to the dimensions of the circuit,
	 * fill the diagonal with 1s, everywhere else -- with 0s.
	 */
	Sdyn.nullify();
	Sdyn.copy(variation.dynamic_correlation);
	for (i = _processor_count; i < _node_count; i++)
		Sdyn[i][i] = 1;

	/* Gamma(Sdyn) = U * V^(1/2) */
	EigenvalueDecomposition decomposition(Sdyn, Mtemp2, Vtemp);

	for (i = 0; i < _node_count; i++)
		Vtemp[i] = std::sqrt(Vtemp[i]);

	multiply_matrix_diagonal_matrix(Mtemp2, Vtemp, GammaSdyn);
}
