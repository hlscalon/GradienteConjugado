#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "ColumnVector.hpp"

#include <vector>

class SparseMatrix {
public:

	SparseMatrix(const int nRows, const int nCols) :
		_nRows(nRows), _nCols(nCols) {}

	inline int getCols() const {
		return _nCols;
	}

	inline int getRows() const {
		return _nRows;
	}

	inline void setValues(const std::vector<double> & values) {
		_values = values;
	}

	inline void setRowsIdx(const std::vector<int> & rowsIdx) {
		_rowsIdx = rowsIdx;
	}

	inline void setColsPtr(const std::vector<int> & colsPtr) {
		_colsPtr = colsPtr;
	}

	inline void setColunas(const std::vector<int> & colunas) {
		_colunas = colunas;
	}

	ColumnVector operator*(const ColumnVector & b) const;

	void printCSC() const;

private:
	int _nRows;
	int _nCols;
	std::vector<double> _values;
	std::vector<int> _rowsIdx;
	std::vector<int> _colsPtr;
	std::vector<int> _colunas;
};

#endif // SPARSE_MATRIX_HPP
