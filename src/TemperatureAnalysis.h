#ifndef __TEMPERATURE_ANALYSIS_H__
#define __TEMPERATURE_ANALYSIS_H__

#include "matrix.h"
#include "HotSpot.h"
#include "Variation.h"

class TemperatureAnalysis
{
	protected:

	const size_t _processor_count;
	const size_t _node_count;

	const double _sampling_interval;
	const double _ambient_temperature;

	/* == Main matrices and vectors == */

	/* Capacitance^(-1/2) */
	vector_t Cm12;

	/* Coefficients A and B */
	matrix_t A;
	matrix_t B;
	matrix_t AT;
	matrix_t BT;

	/* Eigenvector decomposition */
	vector_t V;
	matrix_t U;
	matrix_t UT;

	/* == Variations == */

	/* Dynamic power */
	vector_t Kdyn; /* incomplete */
	matrix_t GammaSdyn;

	private:

	/* == Auxiliary space == */

	matrix_t Mtemp;
	vector_t Vtemp;

	public:

	TemperatureAnalysis(const HotSpot &hotspot, const Variation &variation);

	virtual void perform(const matrix_t &dynamic_power, matrix_t &temperature) = 0;

	private:

	void prepare_memory();
	void prepare_thermal_coefficients(const HotSpot &hotspot);
	void prepare_variations(const Variation &variation);
};

#endif
