#ifndef __TRANSIENT_TEMPERATURE_ANALYSIS_H__
#define __TRANSIENT_TEMPERATURE_ANALYSIS_H__

#include "TemperatureAnalysis.h"

class TransientTemperatureAnalysis: public TemperatureAnalysis
{
	matrix_t Mtemp;
	double *Tlast, *Tnext, *BP;

	public:

	TransientTemperatureAnalysis(const HotSpot &hotspot, const Variation &variation)
		: TemperatureAnalysis(hotspot, variation), Mtemp(3, _node_count)
	{
		Tlast = Mtemp[0];
		Tnext = Mtemp[1];
		BP = Mtemp[2];
	}

	void perform(const matrix_t &dynamic_power, matrix_t &temperature);

	protected:

	void compute_initial_covariance(const double *Pdyn, matrix_t &Cov);
};

#endif
