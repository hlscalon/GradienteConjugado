#include "SparseMatrix.hpp"

#include <iostream>
#include <cassert>
#include <cmath>
#include <sstream>
#include "mpi.h"

#ifdef MPE_LOG
#include <mpe.h>
#endif

ColumnVector SparseMatrix::operator*(const ColumnVector & b) const {
	const int bCols = b.getSize();
	ColumnVector res(bCols);

	const int colsSize = _colsPtr.size();
	for (auto i = 0; i < colsSize - 1; ++i) {
		const int coluna = _colunas[i];
		for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
			const int linha = _rowsIdx[k];
			const double valor = _values[k];
			res(linha) += valor * b(coluna);
			if (coluna != linha) {
				res(coluna) += valor * b(linha);
			}
		}
	}

	ColumnVector res_total(bCols);

	#ifdef MPE_LOG
	MPE_Log_event(_evAllReduce1, 0, "Inicio do AllReduce");
	#endif

	MPI_Allreduce(res.data(), res_total.data(), bCols, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	#ifdef MPE_LOG
	MPE_Log_event(_evAllReduce2, 0, "Fim do AllReduce");
	#endif

	return res_total;
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
