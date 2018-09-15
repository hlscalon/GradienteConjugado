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

	assert(_nCols == bCols);

	ColumnVector colVector(bCols);

	int metade = std::ceil(bCols / 2); // pega a linha do meio
	for (auto i = 0; i < metade; ++i) { // bCols
		for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
			colVector(_rowsIdx[k]) += _values[k] * b(i);
		}
	}

	const ColumnVector aux(colVector);
	for (auto i = 0; i < bCols; ++i) {
		colVector(i) += aux(bCols - 1 - i);
	}

	for (auto k = _colsPtr[metade]; k < _colsPtr[metade + 1]; ++k) {
		colVector(_rowsIdx[k]) += _values[k] * b(metade);
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
