#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "ColumnVector.hpp"

#include <vector>

class SparseMatrix {
public:

	SparseMatrix(const int rank, const int nprocs, const int nRows, const int nCols) :
		_rank(rank), _nprocs(nprocs), _nRows(nRows), _nCols(nCols) {}

	inline int getCols() const {
		return _nCols;
	}

	inline int getRows() const {
		return _nRows;
	}

	inline void updateColsPtr() {
		_colsPtr.push_back(_colPtr++);
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

	void set(const int row, const int col, const double value);

	ColumnVector operator*(const ColumnVector & b) const;

	ColumnVector multiBoeing(const ColumnVector & b) const;

	ColumnVector multiMTX(const ColumnVector & b) const;

	void printCSC() const;

private:
	int _rank;
	int _nprocs;
	int _nRows;
	int _nCols;
	int _colPtr;
	int _lastCol;
	std::vector<double> _values;
	std::vector<int> _rowsIdx;
	std::vector<int> _colsPtr;
	std::vector<int> _colunas;
};

#endif // SPARSE_MATRIX_HPP
