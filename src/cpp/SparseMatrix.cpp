#include "SparseMatrix.hpp"

#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>

ColumnVector SparseMatrix::operator*(const ColumnVector & b) const {
	const int bCols = b.getSize();
	ColumnVector res(bCols);

	const int colsSize = _colsPtr.size();
	for (auto i = 0; i < colsSize - 1; ++i) {
		const int coluna = i;
		for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
			const int linha = _rowsIdx[k];
			const double valor = _values[k];
			res(linha) += valor * b(coluna);
			if (coluna != linha) {
				res(coluna) += valor * b(linha);
			}
		}
	}

	return res;
}

void SparseMatrix::printCSC() const {
	std::stringstream iss;
	iss << "_values: " << "\n"; for (const auto & v : _values) iss << v << " "; iss << "\n";
	iss << "_rowsIdx: " << "\n"; for (const auto & v : _rowsIdx) iss << v << " "; iss << "\n";
	iss << "_colsPtr: " << "\n"; for (const auto & v : _colsPtr) iss << v << " "; iss << "\n";
	iss << "_colunas: " << "\n"; for (const auto & v : _colunas) iss << v << " "; iss << "\n";
	std::cout << iss.str();
}
