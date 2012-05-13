#ifndef __VARIATION_H__
#define __VARIATION_H__

#include <string>

#include "Configuration.h"

class Variation
{
	public:

	/* Dynamic power */
	vector_t dynamic_ratio;
	matrix_t dynamic_correlation;

	/* Leakage power */
	vector_t leakage_nominal;
	matrix_t leakage_covariance;

	/* Environmental noise */
	matrix_t noise_covariance;

	Variation(size_t processor_count, const std::string &filename)
	{
		Configuration config(filename);

		size_t count, i;

		count = config.size();

		for (i = 0; i < count; i++) {
			if (config[i].name == "dynamic_ratio")
				config[i].to_vector(dynamic_ratio);
			else if (config[i].name == "dynamic_correlation")
				config[i].to_matrix(dynamic_correlation);
			else if (config[i].name == "leakage_nominal")
				config[i].to_vector(leakage_nominal);
			else if (config[i].name == "leakage_covariance")
				config[i].to_matrix(leakage_covariance);
			else if (config[i].name == "noise_covariance")
				config[i].to_matrix(noise_covariance);
		}

		if (processor_count != dynamic_ratio.size())
			throw std::runtime_error("The dynamic variation ratio vector is invalid.");
		if (processor_count != dynamic_correlation.rows() ||
			processor_count != dynamic_correlation.cols())
			throw std::runtime_error("The dynamic correlation matrix is invalid.");
		if (processor_count != leakage_nominal.size())
			throw std::runtime_error("The nominal leakage vector is invalid.");
		if (processor_count != leakage_covariance.rows() ||
			processor_count != leakage_covariance.cols())
			throw std::runtime_error("The leakage covariance matrix is invalid.");
		if (processor_count != noise_covariance.rows() ||
			processor_count != noise_covariance.cols())
			throw std::runtime_error("The noise covariance matrix is invalid.");
	}
};

#endif
