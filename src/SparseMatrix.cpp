#include "SparseMatrix.hpp"

#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>
#include "mpi.h"

void SparseMatrix::set(const int row, const int col, const double value) {
	_values[_colPtr] = value;
	_rowsIdx[_colPtr] = row;

	if (col != _lastCol) {
		_colsPtr.push_back(_colPtr);
		_lastCol = col;
	}

	_colPtr++;
}

ColumnVector SparseMatrix::multiMTX(const ColumnVector & b) const {
	const int bCols = b.getSize();

	assert(bCols == _nCols);

	ColumnVector colVector(bCols);

	int metade = _nCols / 2;

	for (auto i = 0; i < metade; ++i) {
		for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
			colVector(_rowsIdx[k]) += _values[k] * b(i);
		}
	}

	ColumnVector aux(colVector);
	for (int i = 0; i < _nCols; ++i) {
		colVector(i) += aux(_nCols - i - 1);
	}

	for (int k = _colsPtr[metade]; k < _colsPtr[metade + 1]; ++k) {
		colVector(_rowsIdx[k]) += _values[k] * b(metade);
	}

	return colVector;
}

ColumnVector SparseMatrix::multiBoeing(const ColumnVector & b) const {
	const int bCols = b.getSize();
	ColumnVector res(bCols);

	const int colsSize = _colsPtr.size();
	for (auto i = 0; i < colsSize - 1; ++i) {
		const int coluna = _colunas[i];
		for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
			double tmp = _values[k] * b(coluna);
			res(_rowsIdx[k]) += tmp;
			if (coluna != _rowsIdx[k]) { // se nao for diagonal
				res(bCols - _rowsIdx[k] - 1) += tmp;
			}
		}
	}

	ColumnVector res_total(bCols);
	MPI_Allreduce(res.data(), res_total.data(), bCols, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	return res_total;
}

ColumnVector SparseMatrix::operator*(const ColumnVector & b) const {
	if (_tipo == Tipo::MTX) {
		return this->multiMTX(b);
	}

	return this->multiBoeing(b);
}

void SparseMatrix::printCSC() const {
	std::stringstream iss;
	iss << "_rank = " << _rank << "\n";
	iss << "_values: " << "\n"; for (const auto & v : _values) iss << v << " "; iss << "\n";
	iss << "_rowsIdx: " << "\n"; for (const auto & v : _rowsIdx) iss << v << " "; iss << "\n";
	iss << "_colsPtr: " << "\n"; for (const auto & v : _colsPtr) iss << v << " "; iss << "\n";
	iss << "_colunas: " << "\n"; for (const auto & v : _colunas) iss << v << " "; iss << "\n";
	std::cout << iss.str();
}
