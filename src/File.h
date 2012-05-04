#ifndef __FILE_H__
#define __FILE_H__

#include <fstream>
#include <string>

#include "matrix.h"

class File
{
	public:

	static bool exist(const std::string &filename)
	{
		return std::ifstream(filename.c_str()).is_open();
	}

	static void dump(const matrix_t &matrix, const char *filename)
	{
		dump(matrix.pointer(), matrix.rows(), matrix.cols(), filename);
	}

	static void dump(const matrix_t &matrix, const std::string &filename)
	{
		dump(matrix.pointer(), matrix.rows(), matrix.cols(), filename.c_str());
	}

	static void dump(const double *matrix, size_t rows, size_t cols,
		const char *filename)
	{
		std::ofstream stream(filename);

		for (size_t i = 0; i < rows; i++) {
			for (size_t j = 0; j < cols; j++) {
				stream << matrix[i * cols + j];
				if (j + 1 < cols) stream << '\t';
			}
			stream << std::endl;
		}

		stream.close();
	}
};

#endif
