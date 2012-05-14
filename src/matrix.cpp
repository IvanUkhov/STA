#include "matrix.h"
#include "float.h"

void transpose_matrix(
	const matrix_t &U, matrix_t &UT)
{
	int i, j;
	int n = U.rows();
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			UT[j][i] = U[i][j];
}

void multiply_diagonal_matrix_matrix(
	const vector_t &V, const matrix_t &M, matrix_t &R)
{
	int i, j;
	int n = M.rows();
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			R[i][j] = V[i] * M[i][j];
}

void multiply_two_incomplete_bottom_diagonal_matrices_matrix(
	const double *V1, const double *V2, const matrix_t &M, size_t m, matrix_t &R)
{
	size_t i, j;
	size_t n = M.rows();

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			R[i][j] = V1[i] * V2[i] * M[i][j];

	for (; i < n; i++)
		for (j = 0; j < n; j++)
			R[i][j] = 0;
}

void multiply_matrix_diagonal_matrix(
	const matrix_t &M, const double *V, matrix_t &R)
{
	int i, j;
	int n = M.rows();
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			R[i][j] = V[j] * M[i][j];
}

void multiply_matrix_matrix(
	const matrix_t &M, const matrix_t &N, matrix_t &R)
{
	int i, j, k;
	int n = M.rows();
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			R[i][j] = 0;
			for (k = 0; k < n; k++)
				R[i][j] += M[i][k] * N[k][j];
		}
	}
}

void multiply_matrix_matrix(
	const matrix_t &M, const matrix_t &N, double *R)
{
	int i, j, k, l;
	int n = M.rows();
	for (i = 0, l = 0; i < n; i++) {
		for (j = 0; j < n; j++, l++) {
			R[l] = 0;
			for (k = 0; k < n; k++)
				R[l] += M[i][k] * N[k][j];
		}
	}
}

void multiply_matrix_matrix_diagonal_matrix(
	const matrix_t &M, const matrix_t &N, const double *V, matrix_t &R)
{
	int i, j, k;
	int n = M.rows();
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			R[i][j] = 0;
			for (k = 0; k < n; k++)
				R[i][j] += M[i][k] * N[k][j];
			R[i][j] *= V[j];
		}
	}
}

void multiply_matrix_incomplete_vector(
	const matrix_t &M, const double *V, size_t m, double *R)
{
	int i, j;
	int n = M.rows();
	for (i = 0; i < n; i++) {
		R[i] = 0;
		for (j = 0; j < m; j++)
			R[i] += M[i][j] * V[j];
	}
}

void multiply_matrix_vector_plus_vector(
	const matrix_t &M, const double *V, const double *A, double *R)
{
	int i, j;
	int n = M.rows();
	for (i = 0; i < n; i++) {
		R[i] = 0;
		for (j = 0; j < n; j++)
			R[i] += M[i][j] * V[j];
		R[i] += A[i];
	}
}

void multiply_matrix_matrix_vector(
	const matrix_t &M, const matrix_t &N, const double *V, double *R)
{
	int i, j, k;
	int n = M.rows();
	double sum;
	for (i = 0; i < n; i++) {
		R[i] = 0;
		for (j = 0; j < n; j++) {
			sum = 0;
			for (k = 0; k < n; k++)
				sum += M[i][k] * N[k][j];
			R[i] += sum * V[j];
		}
	}
}

void multiply_matrix_vector(
	const matrix_t &M, const double *V, double *R)
{
	int i, j;
	int n = M.rows();
	for (i = 0; i < n; i++) {
		R[i] = 0;
		for (j = 0; j < n; j++)
			R[i] += M[i][j] * V[j];
	}
}

void multiply_two_incomplete_bottom_matrices(
	const matrix_t &M1, const matrix_t &M2, size_t n1, size_t n2, matrix_t &R)
{
	int i, j, k;
	int n = M1.cols();

	R.nullify();

	for (i = 0; i < n1; i++)
		for (j = 0; j < n; j++)
			for (k = 0; k < n2; k++)
				R[i][j] += M1[i][k] * M2[k][j];
}

void multiply_incomplete_bottom_matrix_matrix(
	const matrix_t &M1, const matrix_t &M2, size_t n1, matrix_t &R)
{
	int i, j, k;
	int n = M1.cols();

	R.nullify();

	for (i = 0; i < n1; i++)
		for (j = 0; j < n; j++)
			for (k = 0; k < n; k++)
				R[i][j] += M1[i][k] * M2[k][j];
}

void multiply_matrix_incomplete_bottom_matrix(
	const matrix_t &M1, const matrix_t &M2, size_t n2, matrix_t &R)
{
	int i, j, k;
	int n = M1.cols();

	R.nullify();

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			for (k = 0; k < n2; k++)
				R[i][j] += M1[i][k] * M2[k][j];
}

std::ostream &operator<< (std::ostream &o, const vector_t &vector)
{
	size_t size = vector.size();

	for (size_t i = 0; i < size; i++) {
		if (i > 0) o << '\t';
		o << vector[i];
	}

	o << std::endl;

	return o;
}

std::ostream &operator<< (std::ostream &o, const matrix_t &matrix)
{
	size_t rows = matrix.rows();
	size_t cols = matrix.cols();

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			if (j > 0) o << '\t';
			o << matrix[i][j];
		}
		o << std::endl;
	}

	return o;
}
