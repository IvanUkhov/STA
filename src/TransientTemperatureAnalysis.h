#ifndef __TRANSIENT_TEMPERATURE_ANALYSIS_H__
#define __TRANSIENT_TEMPERATURE_ANALYSIS_H__

#include "TemperatureAnalysis.h"

class TransientTemperatureAnalysis: public TemperatureAnalysis
{
	vector_t Vtemp1;
	vector_t Vtemp2;
	vector_t Vtemp3;

	matrix_t Mtemp1;
	matrix_t Mtemp2;
	matrix_t Mtemp3;
	matrix_t Mtemp4;
	matrix_t Mtemp5;
	matrix_t Mtemp6;

	public:

	TransientTemperatureAnalysis(const HotSpot &hotspot, const Variation &variation)
		: TemperatureAnalysis(hotspot, variation),
		Vtemp1(_node_count), Vtemp2(_node_count), Vtemp3(_node_count),
		Mtemp1(_node_count, _node_count), Mtemp2(_node_count, _node_count),
		Mtemp3(_node_count, _node_count), Mtemp4(_node_count, _node_count),
		Mtemp5(_node_count, _node_count), Mtemp6(_node_count, _node_count)
	{
	}

	void perform(const matrix_t &dynamic_power, matrix_t &temperature);

	protected:

	void update_covariance(matrix_t &CovT, matrix_t &CovTSigma, const double *Pdyn, bool initial,
		matrix_t &Mtemp1, matrix_t &Mtemp2, matrix_t &Mtemp3, matrix_t &Mtemp4) const;
};

#endif
