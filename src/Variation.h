#ifndef __VARIATION_H__
#define __VARIATION_H__

#include "Configuration.h"
#include "EigenvalueDecomposition.h"

class Variation
{
	const size_t _processor_count;
	const size_t _node_count;

	/* Dynamic power */
	vector_t Kdyn;
	matrix_t GammaSdyn;

	matrix_t Mtemp;
	vector_t Vtemp;

	public:

	Variation(size_t processor_count, size_t node_count, const std::string &filename);

	void compute_initial_variance(const matrix_t &B, const matrix_t &BT,
		const double *Pdyn, matrix_t &Var);
};

#endif
