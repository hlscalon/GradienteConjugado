#include "SparseMatrix.hpp"

#include <iostream>
#include <cassert>
#include <cmath>

void SparseMatrix::set(const int row, const int col, const double value) {
	_values[_colPtr] = value;
	_rowsIdx[_colPtr] = row;

	if (col != _lastCol) {
		_colsPtr.push_back(_colPtr);
		_lastCol = col;
	}

	_colPtr++;
}

ColumnVector SparseMatrix::operator*(const ColumnVector & b) const {
	const int bCols = b.getSize();

	assert(bCols == _nCols * 2 + 1);

	ColumnVector colVector(bCols);

	for (auto i = 0; i < _nCols; ++i) { // bCols / 2
		for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
			colVector(_rowsIdx[k]) += _values[k] * b(i);
		}
	}

	const ColumnVector aux(colVector);
	for (auto i = 0; i < bCols; ++i) {
		colVector(i) += aux(bCols - 1 - i);
	}

	for (auto k = _colsPtr[_nCols]; k < _colsPtr[_nCols + 1]; ++k) {
		colVector(_rowsIdx[k]) += _values[k] * b(_nCols);
	}

	return colVector;
}

void SparseMatrix::printCSC() const {
	std::cout << "_values: " << "\n";
	for (const auto & v : _values) {
		std::cout << v << " ";
	}
	std::cout << "\n_rowsIdx: " << "\n";
	for (const auto & v : _rowsIdx) {
		std::cout << v << " ";
	}
	std::cout << "\n_colsPtr: " << "\n";
	for (const auto & v : _colsPtr) {
		std::cout << v << " ";
	}
	std::cout << "\n";
}
