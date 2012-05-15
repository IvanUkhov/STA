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

	inline void extend(size_t __rows, size_t __cols = 0, bool nullify = false)
	{
		if (!__cols || __cols == _cols) {
			vector_t::extend(__rows * _cols, nullify);
			_rows = __rows;
			return;
		}

		double *__data = __ALLOC(double, __rows * __cols);
		if (nullify) __NULLIFY(__data, double, __rows * __cols);

		for (size_t i = 0; i < __rows && i < _rows; i++)
			for (size_t j = 0; j < __cols && j < _cols; j++)
				__data[i * __cols + j] = _data[i * _cols + j];

		replace(__data, __cols * __rows);

		_rows = __rows;
		_cols = __cols;
	}

	inline void shape(const array_t<double> &array,
		size_t __rows, size_t __cols)
	{
		if (array._size < __rows * __cols)
			throw std::runtime_error("Cannot shape the array.");

		vector_t::clone(array, __rows * __cols);

		_rows = __rows;
		_cols = __cols;
	}

	inline void copy(const matrix_t &another)
	{
		if (another._cols > _cols || another._rows > _rows)
			throw std::runtime_error("Cannot copy the matrix.");

		if (another._cols == _cols) {
			vector_t::copy(another);
			return;
		}

		for (size_t i = 0; i < another._rows; i++)
			for (size_t j = 0; j < another._cols; j++)
				_data[i * _cols + j] = another._data[i * another._cols + j];
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
void multiply_two_incomplete_bottom_diagonal_matrices_matrix(
	const double *V1, const double *V2, const matrix_t &M, size_t m, matrix_t &R);
void multiply_matrix_diagonal_matrix(
	const matrix_t &M, const double *V, matrix_t &R);
void multiply_matrix_matrix(
	const matrix_t &M, const matrix_t &N, matrix_t &R);
void multiply_matrix_matrix(
	const matrix_t &M, const matrix_t &N, double *R);
void multiply_matrix_matrix_diagonal_matrix(
	const matrix_t &M, const matrix_t &N, const double *V, matrix_t &R);
void multiply_matrix_incomplete_vector(
	const matrix_t &M, const double *V, size_t m, double *R);
void multiply_matrix_vector_plus_vector(
	const matrix_t &M, const double *V, const double *A, double *R);
void multiply_matrix_matrix_vector(
	const matrix_t &M, const matrix_t &N, const double *V, double *R);
void multiply_matrix_vector(
	const matrix_t &M, const double *V, double *R);
void multiply_two_incomplete_bottom_matrices(
	const matrix_t &M1, const matrix_t &M2, size_t n1, size_t n2, matrix_t &R);
void multiply_incomplete_bottom_matrix_matrix(
	const matrix_t &M1, const matrix_t &M2, size_t n, matrix_t &R);
void multiply_matrix_incomplete_bottom_matrix(
	const matrix_t &M1, const matrix_t &M2, size_t n, matrix_t &R);
void add_matrix_matrix(const matrix_t &M1, const matrix_t &M2, matrix_t &R);

std::ostream &operator<< (std::ostream &o, const vector_t &vector);
std::ostream &operator<< (std::ostream &o, const matrix_t &matrix);

#endif
