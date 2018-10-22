#include <mpi.h>
#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>

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
		std::cout << "rank = " << _rank << "\n";
		// for (const auto & n : _nums) std::cout << n << " ";

		std::cout << "_values = ";
		for (const auto & v : _values) { std::cout << v << " "; }

		std::cout << "\n_rowsIdx = ";
		for (const auto & r : _rowsIdx) { std::cout << r << " "; }

		std::cout << "\n_colsPtr = ";
		for (const auto & c : _colsPtr) { std::cout << c << " "; }

		std::cout << "\n";
	}

	void addValues(std::initializer_list<double> values) {
		_values = std::vector<double>(values);
	}

	void addRowsIdx(std::initializer_list<int> rowsIdx) {
		_rowsIdx = std::vector<int>(rowsIdx);
	}

	void addColsPtr(std::initializer_list<int> colsPtr) {
		_colsPtr = std::vector<int>(colsPtr);
	}

private:
	int _rank;
	int _nprocs;
	std::vector<double> _values;
	std::vector<int> _rowsIdx;
	std::vector<int> _colsPtr;
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

	Matrix t(rank, size);
	// if (rank == 0) {

	// 	for (int i = 1; i < size; ++i) {
	// 		MPI_Send( &len, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD );
	// 		MPI_Send( send_vec.data(), len, MPI_INT, 1, datatag, MPI_COMM_WORLD );
	// 	}

	// 	t.addValues();
	// 	t.addRowsIdx();
	// 	t.addColsPtr();
	// } else {

	// }

	// run(t, rank);

	MPI_Finalize();

	return 0;
}
