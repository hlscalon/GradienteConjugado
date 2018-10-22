#include <mpi.h>
#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>
#include <sstream>

struct Matrix {
	Matrix(int rank, int nprocs) : _rank(rank), _nprocs(nprocs) {}

	std::vector<int> operator*(int valor) const {
		// int size = 0;
		// if (_rank == 0) {
		//	 size = _nums.size() / _nprocs;
		// }

		// std::vector<int> res;

		// MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// res.resize(size);

		// // dividir matriz
		// MPI_Scatter(const_cast<int*>(_nums.data()), size, MPI_INT, res.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

		// for (int i = 0; i < size; ++i) {
		//	 res[i] *= valor;
		// }

		// // reunir no 0
		// std::vector<int> res_total;
		// if (_rank == 0) {
		//	 res_total.resize(_nums.size());
		// }

		// MPI_Gather(res.data(), size, MPI_INT, res_total.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

		// return res_total;
		return {};
	}

	void printar() const {
		std::stringstream iss;

		iss << "rank = " << _rank << "\n";
		// for (const auto & n : _nums) iss << n << " ";

		iss << "_values = ";
		for (const auto & v : _values) { iss << v << " "; }

		iss << "\n_rowsIdx = ";
		for (const auto & r : _rowsIdx) { iss << r << " "; }

		iss << "\n_colsPtr = ";
		for (const auto & c : _colsPtr) { iss << c << " "; }

		iss << "\n_colunas = ";
		for (const auto & c : _colunas) { iss << c << " "; }

		iss << "\n";

		std::cout << iss.str();
	}

	void addValues(const std::vector<double> & values) {
		_values = values;
	}

	void addRowsIdx(const std::vector<int> & rowsIdx) {
		_rowsIdx = rowsIdx;
	}

	void addColsPtr(const std::vector<int> & colsPtr) {
		_colsPtr = colsPtr;
	}

	void addColunas(const std::vector<int> & colunas) {
		_colunas = colunas;
	}

private:
	int _rank;
	int _nprocs;
	std::vector<double> _values;
	std::vector<int> _rowsIdx;
	std::vector<int> _colsPtr;
	std::vector<int> _colunas;
};

void run(const Matrix & t, int rank) {
	if (rank == 0) {
		t.printar();
	}
}

struct Dados {
	std::vector<double> values;
	std::vector<int> rowsIdx;
	std::vector<int> colsPtr;
	std::vector<int> colunas; // 0, 1, 2 ...
};

int main(int argc, char** argv) {
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// if (size < 2) {
	// 	if (rank == 0)
	// 		std::cerr << "Require at least 2 tasks" << std::endl;
	// 	MPI_Abort(MPI_COMM_WORLD, 1);
	// }

	std::vector<Dados> dados;
	dados.reserve(size);

	if (rank == 0) {
		std::vector<double> values{5.0, 2.0, 1.0, 2.0, 5.0, 2.0, 1.0, 1.0, 2.0, 5.0, 2.0, 1.0};
		std::vector<int> rowsIdx{0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 4};
		std::vector<int> colsPtr{0, 3, 7, 12};

		int sizeColsPtr = colsPtr.size();
		int colPorProc = std::floor(colsPtr.size() / size);
		for (int proc = 0; proc < size; ++proc) {
			Dados d;

			int colFinal = (proc + 1) * colPorProc;
			for (int colAtual = proc * colPorProc; colAtual < colFinal && colAtual < sizeColsPtr; ++colAtual) {

				if (colAtual < sizeColsPtr - 1) {
					d.colunas.push_back(colAtual);
				}

				int colProx = colAtual + 1;
				for (int j = colsPtr[colAtual]; j < colsPtr[colProx]; ++j) {
					d.values.push_back(values[j]);
					d.rowsIdx.push_back(rowsIdx[j]);
				}

				d.colsPtr.push_back(colsPtr[colAtual]);
			}

			if (colFinal < sizeColsPtr) {
				d.colsPtr.push_back(colsPtr[colFinal]);
			}

			if (!d.values.empty())
				dados.push_back(d); // proc
		}
	}

	if (rank == 0) {
		for (int i = 0; i < static_cast<int>(dados.size()); ++i) {
			std::cout << "i = " << i << "\n";
			std::cout << "values = ";
			for (const auto & v : dados[i].values) { std::cout << v << " "; }

			std::cout << "\nrowsIdx = ";
			for (const auto & r : dados[i].rowsIdx) { std::cout << r << " "; }

			std::cout << "\ncolsPtr = ";
			for (const auto & c : dados[i].colsPtr) { std::cout << c << " "; }

			std::cout << "\ncolunas = ";
			for (const auto & c : dados[i].colunas) { std::cout << c << " "; }

			std::cout << "\n------------------------------------\n";
		}
	}

	const int lentag = 0;
	const int datatag = 1;
	Matrix t(rank, size);
	if (rank == 0) {
		for (int proc = 1; proc < size; ++proc) {

			int lenValues = dados[proc].values.size();
			MPI_Send(&lenValues, 1, MPI_INT, proc, lentag, MPI_COMM_WORLD);
			MPI_Send(dados[proc].values.data(), lenValues, MPI_DOUBLE, proc, datatag, MPI_COMM_WORLD);

			int lenRowsIdx = dados[proc].rowsIdx.size();
			MPI_Send(&lenRowsIdx, 1, MPI_INT, proc, lentag, MPI_COMM_WORLD);
			MPI_Send(dados[proc].rowsIdx.data(), lenRowsIdx, MPI_INT, proc, datatag, MPI_COMM_WORLD);

			int lenColsPtr = dados[proc].colsPtr.size();
			MPI_Send(&lenColsPtr, 1, MPI_INT, proc, lentag, MPI_COMM_WORLD);
			MPI_Send(dados[proc].colsPtr.data(), lenColsPtr, MPI_INT, proc, datatag, MPI_COMM_WORLD);

			int lenColunas = dados[proc].colunas.size();
			MPI_Send(&lenColunas, 1, MPI_INT, proc, lentag, MPI_COMM_WORLD);
			MPI_Send(dados[proc].colunas.data(), lenColunas, MPI_INT, proc, datatag, MPI_COMM_WORLD);
		}

		t.addValues(dados[0].values);
		t.addRowsIdx(dados[0].rowsIdx);
		t.addColsPtr(dados[0].colsPtr);
		t.addColunas(dados[0].colunas);
	} else {
		int lenValues;
		MPI_Recv(&lenValues, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::vector<double> values_rec(lenValues);
		MPI_Recv(values_rec.data(), lenValues, MPI_DOUBLE, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int lenRowsIdx;
		MPI_Recv(&lenRowsIdx, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::vector<int> rowsIdx_rec(lenRowsIdx);
		MPI_Recv(rowsIdx_rec.data(), lenRowsIdx, MPI_INT, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int lenColsPtr;
		MPI_Recv(&lenColsPtr, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::vector<int> colsPtr_rec(lenColsPtr);
		MPI_Recv(colsPtr_rec.data(), lenColsPtr, MPI_INT, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int lenColunas;
		MPI_Recv(&lenColunas, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::vector<int> colunas_rec(lenColunas);
		MPI_Recv(colunas_rec.data(), lenColunas, MPI_INT, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		t.addValues(values_rec);
		t.addRowsIdx(rowsIdx_rec);
		t.addColsPtr(colsPtr_rec);
		t.addColunas(colunas_rec);
	}

	t.printar();

	// run(t, rank);

	MPI_Finalize();

	return 0;
}
