#include "SparseMatrix.hpp"

#include <iostream>
#include <cassert>
#include <cmath>
#include <mpi.h>

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

	int sizeColsPtr = 0;
	if (_rank == 0) {
		sizeColsPtr = _colsPtr.size();
	}

	MPI_Bcast(&sizeColsPtr, 1, MPI_INT, 0, MPI_COMM_WORLD);

	std::vector<double> values;
	std::vector<int> rowsIdx;
	std::vector<int> colsPtr;

	if (_rank == 0) {
		values = _values;
		rowsIdx = _rowsIdx;
		colsPtr = _colsPtr;
	} else {
		values.resize(_nValues);
		rowsIdx.resize(_nValues);
		colsPtr.resize(sizeColsPtr);
	}

	MPI_Bcast(values.data(), _nValues, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(rowsIdx.data(), _nValues, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(colsPtr.data(), sizeColsPtr, MPI_INT, 0, MPI_COMM_WORLD);

	int metade = std::ceil(bCols / 2); // pega a linha do meio
	for (auto i = 0; i < metade; ++i) {
		for (auto k = colsPtr[i]; k < colsPtr[i + 1]; ++k) {
			colVector(rowsIdx[k]) += values[k] * b(i);
		}
	}

	const ColumnVector aux(colVector);
	for (auto i = 0; i < bCols; ++i) {
		colVector(i) += aux(bCols - 1 - i);
	}

	for (auto k = colsPtr[metade]; k < colsPtr[metade + 1]; ++k) {
		colVector(rowsIdx[k]) += values[k] * b(metade);
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
