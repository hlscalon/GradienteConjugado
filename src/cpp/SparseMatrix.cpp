#include "SparseMatrix.hpp"

#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>

ColumnVector SparseMatrix::operator*(const ColumnVector & b) const {
	const int bCols = b.getSize();
	ColumnVector res(bCols);

	for (int r = 0; r < bCols; ++r) {
		for (int c = 0; c < bCols; ++c) {
			res(r) += _matriz[r][c] * b(c);
		}
	}

	return res;
}

void SparseMatrix::print() const {
	std::stringstream iss;
	for (int r = 0; r < _nRows; ++r) {
		for (int c = 0; c <_nCols; ++c) {
			iss << _matriz[r][c] << " ";
		}
		iss << "\n";
	}
	std::cout << iss.str();
}
