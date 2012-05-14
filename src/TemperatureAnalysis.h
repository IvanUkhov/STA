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

	/* Auxiliary space */
	matrix_t Mtemp;
	vector_t Vtemp;

	public:

	TemperatureAnalysis(const HotSpot &hotspot);

	virtual void perform(const matrix_t &dynamic_power, matrix_t &temperature) = 0;
};

class TransientTemperatureAnalysis: public TemperatureAnalysis
{
	const Variation &_variation;

	double *Tlast, *Tnext, *BP;

	/* From the parent:
	 *
	 * matrix_t Mtemp;
	 * vector_t Vtemp;
	 */

	public:

	TransientTemperatureAnalysis(const HotSpot &hotspot, const Variation &variation)
		: TemperatureAnalysis(hotspot), _variation(variation)
	{
		if (_node_count < 3)
			throw std::runtime_error("The circuit seems to be too small.");

		Tlast = Mtemp[0];
		Tnext = Mtemp[1];
		BP = Mtemp[2];
	}

	void perform(const matrix_t &dynamic_power, matrix_t &temperature);
};

#endif
