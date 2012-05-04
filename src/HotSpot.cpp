#include <sstream>

#include "HotSpot.h"

size_t read_config_line(str_pair *table, size_t max, const std::string &line)
{
	size_t count = 0;

	std::string name, value;
	std::stringstream stream(line);

	while (true) {
		stream >> name;

		if (name.empty() || stream.eof() || stream.bad())
			throw std::runtime_error("The configuration stream is bad.");

		stream >> value;

		if (stream.bad())
			throw std::runtime_error("The configuration stream is bad.");

		strcpy(table[count].name, name.c_str());
		strcpy(table[count].value, value.c_str());

		count++;

		if (stream.eof()) break;
	}

	return count;
}

HotSpot::HotSpot(const std::string &floorplan_filename,
	const std::string &config_filename, const std::string &config_line)
{
	config = default_thermal_config();

	if (!config_filename.empty()) {
		str_pair table[MAX_ENTRIES];
		size_t i = read_str_pairs(table, MAX_ENTRIES,
			const_cast<char *>(config_filename.c_str()));
		thermal_config_add_from_strs(&config, table, i);
	}

	if (!config_line.empty()) {
		str_pair table[MAX_ENTRIES];
		size_t i = read_config_line(table, MAX_ENTRIES, config_line);
		thermal_config_add_from_strs(&config, table, i);
	}

	floorplan = read_flp(const_cast<char *>(floorplan_filename.c_str()), FALSE);

	model = alloc_RC_model(&config, floorplan);

	populate_R_model(model, floorplan);
	populate_C_model(model, floorplan);

	node_count = model->block->n_nodes;
	processor_count = floorplan->n_units;

	sampling_interval = config.sampling_intvl;
	ambient_temperature = config.ambient;
}

HotSpot::~HotSpot()
{
	delete_RC_model(model);
	free_flp(floorplan, FALSE);
}

void HotSpot::get_conductance(matrix_t &conductance) const
{
	conductance.resize(node_count, node_count);

	double **b = model->block->b;

	for (size_t i = 0; i < node_count; i++)
		for (size_t j = 0; j < node_count; j++)
			conductance[i][j] = b[i][j];
}

void HotSpot::get_capacitance(vector_t &capacitance) const
{
	capacitance.resize(node_count);

	double *a = model->block->a;

	for (size_t i = 0; i < node_count; i++)
		capacitance[i] = a[i];
}
