#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "configuration.h"

std::string white_spaces(" \t\f\v\n\r");

std::string chomp(const std::string &line)
{
	std::string out;

	size_t found;

	found = line.find_first_not_of(white_spaces);
	if (found != std::string::npos) out = line.substr(found);

	found = out.find_last_not_of(white_spaces);
	if (found != std::string::npos) out.erase(found + 1);
	else out.clear();

	return out;
}

matrix_t parameter_t::to_matrix() const
{
	matrix_t matrix;
	to_matrix(matrix);
	return matrix;
}

void parameter_t::to_matrix(matrix_t &matrix) const
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
			matrix[right][left] = 1;
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

vector_t parameter_t::to_vector() const
{
	vector_t vector;
	to_vector(vector);
	return vector;
}

void parameter_t::to_vector(vector_t &vector) const
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

parameters_t::parameters_t(const std::string &filename)
{
	update(filename);
}

void parameters_t::update(const std::string &filename)
{
	std::ifstream file(filename.c_str());

	if (!file.is_open())
		throw std::runtime_error("Cannot open the tuning file.");

	update(file);
}

void parameters_t::append(const std::string &name, const std::string &value)
{
	push_back(parameter_t(chomp(name), chomp(value)));
}

void parameters_t::update(std::istream &main_stream)
{
	main_stream.exceptions(std::ios::failbit | std::ios::badbit);

	std::string line, name, value;
	size_t found;

	while (true) {
		try {
			std::getline(main_stream, line);
		}
		catch (...) {
			break;
		}

		line = chomp(line);

		if (line.empty()) continue;
		else if (line[0] == '#') continue;
		else if (line[0] == '@') {
			if (!name.empty()) append(name, value);

			found = line.find_first_of(white_spaces);

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

std::ostream &operator<< (std::ostream &o, const parameter_t &parameter)
{
	o << parameter.name << ": " << parameter.value << std::endl;

	return o;
}

std::ostream &operator<< (std::ostream &o, const parameters_t &parameters)
{
	size_t length = parameters.size();

	for (size_t i = 0; i < length; i++)
		o << parameters[i] << std::endl;

	return o;
}
