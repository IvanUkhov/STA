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
	const matrix_t &M, const double *V, int m, double *R)
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

void multiply_matrix_vector_plus_vector(
	size_t n, const double *M, const double *V, const double *A, double *R)
{
	int i, j;
	for (i = 0; i < n; i++) {
		R[i] = 0;
		for (j = 0; j < n; j++)
			R[i] += M[i * n + j] * V[j];
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
