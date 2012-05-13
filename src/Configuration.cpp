#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Configuration.h"
#include "Utils.h"

matrix_t Parameter::to_matrix() const
{
	matrix_t matrix;
	to_matrix(matrix);
	return matrix;
}

void Parameter::to_matrix(matrix_t &matrix) const
{
	std::stringstream stream(value);
	std::string token;
	double one;

	stream >> token;

	if (token == "diagonal_matrix") {
		size_t size;

		stream >> size;

		matrix.resize(size, size);
		matrix.nullify();

		size_t i = 0;

		/* Read what is available */
		while (i < size && stream >> one) {
			matrix[i][i] = one;
			i++;
		}

		/* Fill in the rest with the last one */
		for (; i < size; i++)
			matrix[i][i] = one;
	}
	else if (token == "relation") { /* A symmetric matrix of 0s and 1s */
		size_t size;

		stream >> size;

		matrix.resize(size, size);
		matrix.nullify();

		size_t left, right;
		std::string token;

		/* Read pairwise */
		while (true) {
			if (!(stream >> token) ||
				!(std::istringstream(token) >> left)) break;

			if (!(stream >> token)) break; /* Delimiter */

			if (!(stream >> token) ||
				!(std::istringstream(token) >> right)) break;

			if (left >= size || right >= size)
				throw std::runtime_error("The format of the relation is invalid.");

			matrix[left][right] = 1;
		}
	}
	else if (token == "matrix") {
		size_t rows, cols;

		stream >> rows >> cols;

		matrix.resize(rows, cols);

		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
				stream >> matrix[i][j];
	}
	else
		throw std::runtime_error("The matrix format is unknown.");
}

vector_t Parameter::to_vector() const
{
	vector_t vector;
	to_vector(vector);
	return vector;
}

void Parameter::to_vector(vector_t &vector) const
{
	std::stringstream stream(value);
	std::string token;
	size_t size, i;
	double one;

	stream >> token;

	if (token == "vector") {
		stream >> size;

		vector.resize(size);
		vector.nullify();

		i = 0;

		/* Read what is available */
		while (i < size && stream >> one)
			vector[i++] = one;

		/* Fill in the rest with the last one */
		for (; i < size; i++)
			vector[i] = one;
	}
	else
		throw std::runtime_error("The vector format is unknown.");
}

Configuration::Configuration(const std::string &filename)
{
	update(filename);
}

void Configuration::update(const std::string &filename)
{
	std::ifstream file(filename.c_str());

	if (!file.is_open())
		throw std::runtime_error("Cannot open the tuning file.");

	update(file);
}

void Configuration::append(const std::string &name, const std::string &value)
{
	push_back(Parameter(Utils::strip(name), Utils::strip(value)));
}

void Configuration::update(std::istream &stream)
{
	std::string line, name, value;
	size_t found;

	while (std::getline(stream, line)) {
		line = Utils::strip(line);
		if (line.empty() || line[0] == '#') continue;

		if (line[0] == '@') {
			if (!name.empty()) append(name, value);

			found = line.find_first_of(WHITE_SPACES);

			name = line.substr(1, found);
			value = line.substr(found);
		}
		else if (!name.empty()) {
			if (value.empty()) value = line;
			else value = value + "\n" + line;
		}
	}

	if (!name.empty()) append(name, value);
}

std::ostream &operator<< (std::ostream &o, const Parameter &parameter)
{
	o << parameter.name << ": " << parameter.value << std::endl;

	return o;
}

std::ostream &operator<< (std::ostream &o, const Configuration &config)
{
	size_t length = config.size();

	for (size_t i = 0; i < length; i++)
		o << config[i] << std::endl;

	return o;
}
