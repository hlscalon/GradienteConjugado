#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <cmath>

ColumnVector gradienteConjugado(const SparseMatrix & A, const ColumnVector & b, int & iteracoes) {
	int imax = 100000;
	double erro = 0.00001;
	int n = A.getRows();
	ColumnVector x(n);
	int i = 1;
	ColumnVector r(b);
	ColumnVector d(b);
	double sigma_novo = r * r;
	double sigma0 = sigma_novo;

	while (i < imax && sigma_novo > (std::pow(erro, 2) * sigma0)) {
		ColumnVector q = A * d;
		double alpha = sigma_novo / (d * q);
		x = x + (d * alpha);

		if (i % 50 == 0) {
			r = b - (A * x);
		} else {
			r = r - (q * alpha);
		}

		double sigma_velho = sigma_novo;
		sigma_novo = r * r;
		double beta = sigma_novo / sigma_velho;
		d = r + (d * beta);
		++i;
	}

	iteracoes = i - 1;

	return x;
}
