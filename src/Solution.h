#ifndef __SOLUTION_H__
#define __SOLUTION_H__

#include "matrix.h"

class Solution
{
	protected:

	const size_t processor_count;
	const size_t node_count;

	const double sampling_interval;
	const double ambient_temperature;

	/* Capacitance^(-1/2) */
	vector_t Cm12;

	/* Coefficients A and B */
	matrix_t A;
	matrix_t B;

	/* Eigenvector decomposition */
	vector_t V;
	matrix_t U;
	matrix_t UT;

	/* Auxiliary space */
	matrix_t m_temp;
	vector_t v_temp;

	public:

	Solution(size_t _processor_count, size_t _node_count,
		double _sampling_interval, double _ambient_temperature,
		const matrix_t &conductivity, const vector_t &capacitance);
};

#endif
