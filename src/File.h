#ifndef __FILE_H__
#define __FILE_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

#include "matrix.h"

#define CHUNK_SIZE 512

class File
{
	public:

	static bool exist(const std::string &filename)
	{
		return std::ifstream(filename.c_str()).is_open();
	}

	static void store(const matrix_t &matrix, const char *filename)
	{
		std::ofstream stream(filename);

		store(matrix, stream);

		stream.close();
	}

	static void store(const matrix_t &matrix, std::ostream &stream,
		std::string label = "")
	{
		store(matrix, stream, label, matrix.cols());
	}

	static void store(const array_t<double> &array, std::ostream &stream,
		std::string label = "", size_t cols = 0)
	{
		size_t size = array.size(), rows, i, j, k;

		if (cols == 0) cols = size;
		rows = std::ceil((double)size / (double)cols);

		const double *p = array;

		if (!label.empty()) {
			for (i = 0; i < cols; i++)
				stream << label << i << '\t';
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
		std::ifstream stream(filename.c_str());

		load(matrix, stream);

		stream.close();
	}

	static void load(matrix_t &matrix, std::istream &stream)
	{
		std::string line;

		size_t read = 0, rows = 0, cols = 0, chunks = 0;
		array_t<double> storage;

		while (std::getline(stream, line)) {
			std::stringstream line_stream(line);

			double value;
			size_t i = 0;

			while (line_stream >> value) {
				i++;

				if (cols && i > cols)
					throw std::runtime_error("The matrix to load is inconsistent.");

				if ((read + 1) > CHUNK_SIZE * chunks) {
					/* We need more space! */
					chunks++;
					storage.extend(chunks * CHUNK_SIZE);
				}

				storage[read] = value;

				read++;
			}

			/* Remember the number of the columns in the first meaningful line */
			if (!cols) cols = i;

			if (cols != i)
				throw std::runtime_error("The matrix to load is inconsistent.");

			/* If there were some values found */
			if (i) rows++;
		}

		matrix.clone(storage, rows, cols);
	}
};

#endif
