#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "ColumnVector.hpp"

#include <vector>

class SparseMatrix {
public:
	SparseMatrix(const int nRows, const int nCols, const int nValues) :
				_nRows(nRows), _nCols(nCols), _colPtr(0), _lastCol(-1), _nValues(nValues), _values(nValues), _rowsIdx(nValues) {}

	inline int getCols() const {
		return _nCols;
	}

	inline int getRows() const {
		return _nRows;
	}

	inline void updateColsPtr() {
		_colsPtr.push_back(_colPtr++);
	}

	void set(const int row, const int col, const double value);

	ColumnVector operator*(const ColumnVector & b) const;

	void printCSC() const;

private:
	int _nRows;
	int _nCols;
	int _colPtr;
	int _lastCol;
	int _nValues;
	std::vector<double> _values;
	std::vector<int> _rowsIdx;
	std::vector<int> _colsPtr;
};

#endif // SPARSE_MATRIX_HPP
