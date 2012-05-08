#ifndef __HOTSPOT_H__
#define __HOTSPOT_H__

extern "C" {
#include <hotspot/util.h>
#include <hotspot/flp.h>
#include <hotspot/temperature.h>
#include <hotspot/temperature_block.h>
}

#include "matrix.h"

#define KELVIN 273.15

class HotSpot
{
	protected:

	size_t _node_count;
	size_t _processor_count;

	double _sampling_interval;
	double _ambient_temperature;

	thermal_config_t config;
	flp_t *floorplan;
	RC_model_t *model;

	public:

	HotSpot(const std::string &floorplan_filename,
		const std::string &config_filename,
		const std::string &config_line = "");
	virtual ~HotSpot();

	inline size_t node_count() const
	{
		return _node_count;
	}

	inline size_t processor_count() const
	{
		return _processor_count;
	}

	inline double sampling_interval() const
	{
		return _sampling_interval;
	}

	inline double ambient_temperature() const
	{
		return _ambient_temperature;
	}

	void fill_conductance(matrix_t &conductance) const;
	void fill_capacitance(vector_t &capacitance) const;
};

#endif
