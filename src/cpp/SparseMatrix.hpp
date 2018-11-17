#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "ColumnVector.hpp"

#include <vector>

class SparseMatrix {
public:

	SparseMatrix(const int nRows, const int nCols) :
		_nRows(nRows), _nCols(nCols), _matriz(nRows, std::vector<double>(nRows, 0)) {}

	inline int getCols() const {
		return _nCols;
	}

	inline int getRows() const {
		return _nRows;
	}

	void set(const int row, const int col, const double value) {
		_matriz[row][col] = value;
	}

	ColumnVector operator*(const ColumnVector & b) const;

	void print() const;

private:
	int _nRows;
	int _nCols;
	std::vector<std::vector<double>> _matriz;
};

#endif // SPARSE_MATRIX_HPP
