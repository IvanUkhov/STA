#ifndef __VARIATION_H__
#define __VARIATION_H__

#include <string>
#include <cmath>

#include "Configuration.h"
#include "EigenvalueDecomposition.h"

class Variation
{
	public:

	vector_t dynamic_ratio;
	matrix_t dynamic_correlation;

	Variation(size_t processor_count, const std::string &filename)
	{
		Configuration config(filename);

		size_t count = config.size();

		for (size_t i = 0; i < count; i++) {
			if (config[i].name == "dynamic_ratio")
				config[i].to_vector(dynamic_ratio);
			else if (config[i].name == "dynamic_correlation")
				config[i].to_matrix(dynamic_correlation);
		}

		/* Validate! */
		if (processor_count != dynamic_ratio.size())
			throw std::runtime_error("The dynamic variation ratio vector is invalid.");
		if (processor_count != dynamic_correlation.rows() ||
			processor_count != dynamic_correlation.cols())
			throw std::runtime_error("The dynamic correlation matrix is invalid.");
	}
};

#endif
