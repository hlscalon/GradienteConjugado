#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include "ColumnVector.hpp"

#include <vector>

class SparseMatrix {
public:

	SparseMatrix(const int rank, const int nprocs, const int nRows, const int nCols, const int evAllReduce1, const int evAllReduce2) :
		_rank(rank), _nprocs(nprocs), _nRows(nRows), _nCols(nCols),
		_evAllReduce1(evAllReduce1), _evAllReduce2(evAllReduce2) {}

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
	int _rank;
	int _nprocs;
	int _nRows;
	int _nCols;
	int _evAllReduce1;
	int _evAllReduce2;
	std::vector<double> _values;
	std::vector<int> _rowsIdx;
	std::vector<int> _colsPtr;
	std::vector<int> _colunas;
};

#endif // SPARSE_MATRIX_HPP
