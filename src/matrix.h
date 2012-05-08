#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <iostream>
#include <stdexcept>
#include <math.h>
#include <stdlib.h>

#include "array.h"

typedef array_t<double> vector_t;

struct matrix_t: public vector_t
{
	size_t _rows;
	size_t _cols;

	matrix_t() : vector_t(), _rows(0), _cols(0)
	{
	}

	matrix_t(size_t __rows, size_t __cols) :
		vector_t(__rows * __cols), _rows(__rows), _cols(__cols)
	{
	}

	matrix_t(const matrix_t &another) :
		vector_t(another), _rows(another._rows), _cols(another._cols)
	{
	}

	inline double *operator[](unsigned int row)
	{
		return &_data[_cols * row];
	}

	inline const double *operator[](unsigned int row) const
	{
		return &_data[_cols * row];
	}

	inline matrix_t &operator=(const matrix_t &another)
	{
		_rows = another._rows; _cols = another._cols;
		(void)vector_t::operator=(another);
		return *this;
	}

	inline void resize(size_t __rows, size_t __cols)
	{
		_rows = __rows; _cols = __cols;
		vector_t::resize(_rows * _cols);
	}

	inline void resize(const matrix_t &another)
	{
		_rows = another._rows; _cols = another._cols;
		vector_t::resize(_rows * _cols);
	}

	inline void extend(size_t __rows)
	{
		_rows = __rows;
		vector_t::extend(_rows * _cols);
	}

	inline void clone(const array_t<double> &array,
		size_t __rows, size_t __cols)
	{
		vector_t::clone(array, __rows * __cols);

		_rows = __rows;
		_cols = __cols;
	}

	inline size_t rows() const
	{
		return _rows;
	}

	inline size_t cols() const
	{
		return _cols;
	}
};

void transpose_matrix(
	const matrix_t &U, matrix_t &UT);
void multiply_diagonal_matrix_matrix(
	const vector_t &V, const matrix_t &M, matrix_t &R);
void multiply_matrix_diagonal_matrix(
	const matrix_t &M, const double *V, matrix_t &R);
void multiply_matrix_matrix(
	const matrix_t &M, const matrix_t &N, matrix_t &R);
void multiply_matrix_matrix(
	const matrix_t &M, const matrix_t &N, double *R);
void multiply_matrix_matrix_diagonal_matrix(
	const matrix_t &M, const matrix_t &N, const double *V, matrix_t &R);
void multiply_matrix_incomplete_vector(
	const matrix_t &M, const double *V, int m, double *R);
void multiply_matrix_vector_plus_vector(
	const matrix_t &M, const double *V, const double *A, double *R);
void multiply_matrix_vector_plus_vector(
	size_t n, const double *M, const double *V, const double *A, double *R);
void multiply_matrix_matrix_vector(
	const matrix_t &M, const matrix_t &N, const double *V, double *R);
void multiply_matrix_vector(
	const matrix_t &M, const double *V, double *R);

std::ostream &operator<< (std::ostream &o, const vector_t &vector);
std::ostream &operator<< (std::ostream &o, const matrix_t &matrix);

#endif
