/* W. H. Press, S. A. Teukolsky, W. T. Vetterling, and B. P. Flannery,
 * Numerical Recipes: The Art of Scientific Computing,
 * Cambridge University Press, 3rd edition, 2007.
 */

#ifndef __EIGENVALUE_DECOMPOSITION_H__
#define __EIGENVALUE_DECOMPOSITION_H__

#include "matrix.h"
#include <float.h>

class EigenvalueDecomposition
{
	const int n;

	matrix_t &z;
	vector_t &d, e;

	public:

	EigenvalueDecomposition(const matrix_t &M, matrix_t &U, vector_t &L)
		: n(M.rows()), z(U), d(L), e(n)
	{
		z = M;
		tred2();
		tqli();
		eigsrt(d, z);
	}

	private:

	void tred2();
	void tqli();
	void eigsrt(vector_t &d, matrix_t &v) const;

	inline double abs(const double &a) const
	{
		return a < 0 ? (-a) : a;
	}

	inline double sign(const double &a, const double &b) const
	{
		return (double)(b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));
	}

	inline double sqr(const double &a) const
	{
		return a * a;
	}

	inline double pythag(const double &a, const double &b) const
	{
		double absa = abs(a), absb = abs(b);
		return (absa > absb ? absa * sqrt(1.0 + sqr(absb / absa)) :
			(absb == 0.0 ? 0.0 : absb * sqrt(1.0 + sqr(absa / absb))));
	}
};

#endif
