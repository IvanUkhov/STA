#ifndef __FILE_H__
#define __FILE_H__

#include <iostream>
#include <fstream>
#include <string>

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

	static void store(const matrix_t &matrix, std::ostream &stream)
	{
		const double *p = matrix;
		size_t rows = matrix.rows();
		size_t cols = matrix.cols();

		for (size_t i = 0; i < rows; i++) {
			for (size_t j = 0; j < cols; j++) {
				stream << p[i * cols + j];
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

			if (!cols) cols = i;

			if (cols != i)
				throw std::runtime_error("The matrix to load is inconsistent.");

			rows++;
		}

		matrix.clone(storage, rows, cols);
	}
};

#endif
