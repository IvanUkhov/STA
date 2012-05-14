#ifndef __FILE_H__
#define __FILE_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cctype>
#include <vector>

#include "matrix.h"
#include "Utils.h"

#define CHUNK_SIZE 512

class File
{
	public:

	static bool exist(const std::string &filename)
	{
		return std::ifstream(filename.c_str()).is_open();
	}

	static void store(const matrix_t &matrix, std::ostream &stream,
		std::string label = "")
	{
		store(matrix, stream, label, matrix.cols());
	}

	static void store(const matrix_t &matrix, std::ostream &stream,
		std::vector<std::string> &labels)
	{
		store(matrix, stream, labels, matrix.cols());
	}

	static void store(const array_t<double> &array, std::ostream &stream,
		std::string label = "", size_t cols = 0)
	{
		std::vector<std::string> labels;

		if (!label.empty())
			for (size_t i = 0; i < cols; i++) {
				std::ostringstream stream;
				stream << label << i;
				labels.push_back(stream.str());
			}

		store(array, stream, labels, cols);
	}

	static void store(const array_t<double> &array, std::ostream &stream,
		std::vector<std::string> &labels, size_t cols = 0)
	{
		size_t size = array.size(), rows, i, j, k;

		if (cols == 0) cols = size;
		rows = std::ceil((double)size / (double)cols);

		const double *p = array;

		if (!labels.empty()) {
			if (labels.size() != cols)
				throw std::runtime_error("The number of labels is invalid.");

			for (i = 0; i < cols; i++) stream << labels[i] << '\t';

			stream << std::endl;
		}

		for (i = 0, k = 0; i < rows; i++) {
			for (j = 0; j < cols && k < size; j++, k++) {
				stream << p[k];
				if (j + 1 < cols) stream << '\t';
			}
			stream << std::endl;
		}
	}

	static void load(matrix_t &matrix, std::string &filename)
	{
		std::vector<std::string> labels;
		load(matrix, filename, labels);
	}

	static void load(matrix_t &matrix, std::string &filename,
		std::vector<std::string> &labels)
	{
		std::ifstream stream(filename.c_str());

		load(matrix, stream, labels);

		stream.close();
	}

	static void load(matrix_t &matrix, std::istream &stream)
	{
		std::vector<std::string> labels;
		load(matrix, stream, labels);
	}

	static void load(matrix_t &matrix, std::istream &stream,
		std::vector<std::string> &labels)
	{
		std::string line;

		size_t read = 0, rows = 0, cols = 0, chunks = 0;
		array_t<double> storage;

		while (std::getline(stream, line)) {
			line = Utils::strip(line);
			if (line.empty() || line[0] == '#') continue;

			size_t i = 0;
			std::stringstream line_stream(line);

			if (rows == 0 && isalpha(line[0])) {
				/* Reading labels */
				std::string label;
				while (line_stream >> label) {
					i++;
					labels.push_back(label);
				}

				if (!i) throw std::runtime_error("The input is inconsistent.");

				cols = i;
			}
			else {
				/* Reading numbers */
				double value;
				while (line_stream >> value) {
					i++;

					if ((read + 1) > CHUNK_SIZE * chunks) {
						/* We need more space! */
						chunks++;
						storage.extend(chunks * CHUNK_SIZE);
					}

					storage[read++] = value;
				}

				if (!i || (cols && cols != i))
					throw std::runtime_error("The input is inconsistent.");

				rows++;

				cols = i;
			}
		}

		matrix.shape(storage, rows, cols);
	}
};

#endif
