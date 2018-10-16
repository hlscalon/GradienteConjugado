#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <cmath>
#include <iostream>

ColumnVector gradienteConjugado(int rank, SparseMatrix & A, const ColumnVector & b) {
	int imax = 1000;
	double erro = 0.00001;
	int n = A.getRows();
	ColumnVector x(n);
	int i = 1;
	ColumnVector r(b);
	ColumnVector d(b);
	double sigma_novo = r * r;
	double sigma0 = sigma_novo;

	std::cout << "rank1 = " << rank << "\n";

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

	std::cout << "rank2 = " << rank << "\n";

	return x;
}
