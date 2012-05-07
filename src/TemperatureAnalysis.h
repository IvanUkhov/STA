#ifndef __TEMPERATURE_ANALYSIS_H__
#define __TEMPERATURE_ANALYSIS_H__

#include "matrix.h"
#include "HotSpot.h"

class TemperatureAnalysis
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

	TemperatureAnalysis(const HotSpot &hotspot);

	virtual void perform(const matrix_t &dynamic_power, matrix_t &temperature) const = 0;
};

class TransientTemperatureAnalysis: public TemperatureAnalysis
{
	public:

	TransientTemperatureAnalysis(const HotSpot &hotspot)
		: TemperatureAnalysis(hotspot)
	{
	}

	void perform(const matrix_t &dynamic_power, matrix_t &temperature) const;
};

#endif
