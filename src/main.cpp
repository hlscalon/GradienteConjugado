#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include "mpi.h"

struct DadosMPI {
	std::vector<double> values;
	std::vector<int> rowsIdx;
	std::vector<int> colsPtr;
	std::vector<int> colunas;
};

void calcularMTX(std::ifstream & infile, const int valorVetor) {
	std::string nop;
	std::getline(infile, nop); // pula 1 linha

	int rows, cols, lines;
	infile >> rows >> cols >> lines;

	if (rows != cols) {
		std::cerr << "O nro de linhas deve ser igual ao de colunas.\n";
		return;
	}

	SparseMatrix A(Tipo::MTX, 0, 0, rows, cols, lines);

	int row, col; double value;
	while (infile >> row >> col >> value) {
		A.set(row - 1, col - 1, value);
	}
	A.updateColsPtr();

	#ifdef DEBUG
	A.printCSC();
	#endif

	ColumnVector b(rows, valorVetor);

	ColumnVector res = gradienteConjugado(A, b);
	res.print();
}

bool carregarVetoresCSC(std::ifstream & infile, std::vector<double> & values, std::vector<int> & colsPtr, std::vector<int> & rowsIdx, int & nLinhasMatriz) {
	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // pula cabeçalho

	int nLinhasColsPtr, nLinhasRowsIdx, nLinhasValues;
	std::string nop;
	infile >> nop >> nLinhasColsPtr >> nLinhasRowsIdx >> nLinhasValues;

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignora ate o final da linha

	int nColunasMatriz, nElementos;
	infile >> nop >> nLinhasMatriz >> nColunasMatriz >> nElementos;

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignora ate o final da linha

	if (nLinhasMatriz != nColunasMatriz) {
		std::cerr << "O nro de linhas deve ser igual ao de colunas.\n";
		return false;
	}

	// nElementos maximo
	values.reserve(nElementos);
	colsPtr.reserve(nElementos);
	rowsIdx.reserve(nElementos);

	// funciona para impar apenas
	int metade = nColunasMatriz / 2; // trunca para + 1

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::string linha; int i = 0; int idx = 0; std::istringstream iss;

	int ultIdxCol = 0;
	int colPtr;
	while (i++ < nLinhasColsPtr && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> colPtr) {
			if (idx++ <= metade + 1) {
				colsPtr.push_back(colPtr - 1);
				ultIdxCol = colPtr - 1;
			}
		}
	}

	ultIdxCol--; // ultima coluna é 1 + do que o ultimo
	i = 0; idx = 0;
	int rowIdx;
	while (i++ < nLinhasRowsIdx && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> rowIdx) {
			if (idx++ <= ultIdxCol) {
				rowsIdx.push_back(rowIdx - 1);
			}
		}
	}

	i = 0; idx = 0;
	double value;
	while (i++ < nLinhasValues && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> value) {
			if (idx++ <= ultIdxCol) {
				values.push_back(value);
			}
		}
	}

	return true;
}

template<typename T>
void sendVectorMPI(int proc, std::vector<T> & v, MPI_Datatype type) {
	const int lentag = 0;
	const int datatag = 1;
	int len = v.size();
	MPI_Send(&len, 1, MPI_INT, proc, lentag, MPI_COMM_WORLD);
	MPI_Send(v.data(), len, type, proc, datatag, MPI_COMM_WORLD);
}

template<typename T>
void recvVectorMPI(std::vector<T> & vec, MPI_Datatype type) {
	const int lentag = 0;
	const int datatag = 1;
	int len;
	MPI_Recv(&len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	vec.resize(len);
	MPI_Recv(vec.data(), len, type, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void calcularBoeing(const int rank, const int size, std::ifstream & infile, const int valorVetor) {
	std::vector<DadosMPI> dados;
	dados.reserve(size);

	int nLinhasMatriz = 0;
	if (rank == 0) {
		std::vector<double> values;
		std::vector<int> colsPtr;
		std::vector<int> rowsIdx;

		if (!carregarVetoresCSC(infile, values, colsPtr, rowsIdx, nLinhasMatriz)) {
			return;
		}

		int sizeColsPtr = colsPtr.size();
		int colPorProc = std::floor(colsPtr.size() / size);
		for (int proc = 0; proc < size; ++proc) {
			DadosMPI d;

			int colFinal = (proc + 1) * colPorProc;
			int offsetCol = -1;
			for (int colAtual = proc * colPorProc; colAtual < colFinal && colAtual < sizeColsPtr; ++colAtual) {
				if (offsetCol == -1) {
					offsetCol = colsPtr[colAtual];
				}

				if (colAtual < sizeColsPtr - 1) {
					d.colunas.push_back(colAtual);
				}

				int colProx = colAtual + 1;
				for (int j = colsPtr[colAtual]; j < colsPtr[colProx]; ++j) {
					d.values.push_back(values[j]);
					d.rowsIdx.push_back(rowsIdx[j]);
				}

				d.colsPtr.push_back(colsPtr[colAtual] - offsetCol);
			}

			if (colFinal < sizeColsPtr) {
				d.colsPtr.push_back(colsPtr[colFinal] - offsetCol);
			}

			if (!d.values.empty()) {
				dados.push_back(d); // proc
			}
		}
	}

	MPI_Bcast(&nLinhasMatriz, 1, MPI_INT, 0, MPI_COMM_WORLD);
	SparseMatrix A(Tipo::Boeing, rank, size, nLinhasMatriz, nLinhasMatriz);

	if (rank == 0) {
		for (int proc = 1; proc < size; ++proc) {
			sendVectorMPI(proc, dados[proc].values, MPI_DOUBLE);
			sendVectorMPI(proc, dados[proc].rowsIdx, MPI_INT);
			sendVectorMPI(proc, dados[proc].colsPtr, MPI_INT);
			sendVectorMPI(proc, dados[proc].colunas, MPI_INT);
		}

		A.setValues(dados[0].values);
		A.setRowsIdx(dados[0].rowsIdx);
		A.setColsPtr(dados[0].colsPtr);
		A.setColunas(dados[0].colunas);
	} else {
		std::vector<double> values_rec;
		recvVectorMPI(values_rec, MPI_DOUBLE);

		std::vector<int> rowsIdx_rec;
		recvVectorMPI(rowsIdx_rec, MPI_INT);

		std::vector<int> colsPtr_rec;
		recvVectorMPI(colsPtr_rec, MPI_INT);

		std::vector<int> colunas_rec;
		recvVectorMPI(colunas_rec, MPI_INT);

		A.setValues(values_rec);
		A.setRowsIdx(rowsIdx_rec);
		A.setColsPtr(colsPtr_rec);
		A.setColunas(colunas_rec);
	}

	#ifdef DEBUG
	A.printCSC();
	#endif

	ColumnVector b(nLinhasMatriz, valorVetor); // qual valor ?
	ColumnVector res = gradienteConjugado(A, b);

	if (rank == 0) {
		res.print();
	}
}

int main(int argc, char *argv[]) {
	// todo: pre condicionadores do gradiente conjugado

	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (argc != 4) {
		std::cerr << "<$1> = caminho do arquivo\n"
				  << "<$2> = formato do arquivo (1 = Harwell-Boeing, 2 = MTX)\n"
				  << "<$3> = valor do vetor b.\n";

		MPI_Abort(MPI_COMM_WORLD, 1);
		return -1;
	}

	std::ifstream infile;
	if (rank == 0) {
		infile.open(argv[1], std::ios::binary);
		if (!infile) {
			std::cerr << "Não foi possível abrir o arquivo informado.\n";
			MPI_Abort(MPI_COMM_WORLD, 1);
			return -1;
		}
	}

	int valorVetor = std::atoi(argv[3]);

	int formato = std::atoi(argv[2]);
	if (formato == 2) {
		calcularMTX(infile, valorVetor);
	} else if (formato == 1) {
		calcularBoeing(rank, size, infile, valorVetor);
	} else {
		std::cerr << "Formato de arquivo inválido.\n";
		return -1;
	}

	MPI_Finalize();

	return 0;
}
