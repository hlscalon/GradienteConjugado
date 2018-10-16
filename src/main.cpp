#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <mpi.h>

void calcularMTX(int rank, int nProcs, std::ifstream & infile, int valorVetor) {
	int rows, cols, lines;
	if (rank == 0) {
		std::string nop;
		std::getline(infile, nop); // pula 1 linha
		infile >> rows >> cols >> lines;
	}

	// mandar copias para todos os processos
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&lines, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rows != cols) {
		if (rank == 0) {
			std::cerr << "O nro de linhas deve ser igual ao de colunas.\n";
		}

		return;
	}

	SparseMatrix A(rank, nProcs, rows, cols, lines, 0);

	// carrega matriz e calcula somente no 0
	if (rank == 0) {
		int row, col; double value;
		while (infile >> row >> col >> value) {
			A.set(row - 1, col - 1, value);
		}
		A.updateColsPtr();
	}

	ColumnVector b(rows, valorVetor);
	ColumnVector res = gradienteConjugado(rank, A, b);

	if (rank == 0) {
		std::cout << "rank print = " << rank << "\n";
		res.print();
	}
}

void calcularBoeing(int rank, int nProcs, std::ifstream & infile, int valorVetor) {
	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // pula cabeçalho

	std::string nop;
	int nLinhasColsPtr, nLinhasRowsIdx, nLinhasValues;
	infile >> nop >> nLinhasColsPtr >> nLinhasRowsIdx >> nLinhasValues;

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignora ate o final da linha

	int nLinhasMatriz, nColunasMatriz, nElementos;
	infile >> nop >> nLinhasMatriz >> nColunasMatriz >> nElementos;

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ignora ate o final da linha

	if (nLinhasMatriz != nColunasMatriz) {
		std::cerr << "O nro de linhas deve ser igual ao de colunas.\n";
		return;
	}

	SparseMatrix A(0, 0, nLinhasMatriz, nColunasMatriz, nElementos, nLinhasMatriz + 1);

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::string linha; int i = 0; int idx = 0; std::istringstream iss;

	int colPtr;
	while (i++ < nLinhasColsPtr && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> colPtr) {
			A.setColPtr(idx++, colPtr - 1);
		}
	}

	i = 0; idx = 0;
	int rowIdx;
	while (i++ < nLinhasRowsIdx && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> rowIdx) {
			A.setRowIdx(idx++, rowIdx - 1);
		}
	}

	i = 0; idx = 0;
	double value;
	while (i++ < nLinhasValues && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> value) {
			A.setValue(idx++, value - 1);
		}
	}

	ColumnVector b(nLinhasMatriz, valorVetor); // qual valor ?
	ColumnVector res = gradienteConjugado(0, A, b);
	res.print();
}

int main(int argc, char *argv[]) {
	// todos:
	//	- pre condicionadores do gradiente conjugado
	// 	- armazenar metade da matriz apenas
	//	- multiplicao nao funciona para matrizes quadradas

	if (argc != 4) {
		std::cerr << "<$1> = caminho do arquivo\n"
				  << "<$2> = formato do arquivo (1 = Harwell-Boeing, 2 = MTX)\n"
				  << "<$3> = valor do vetor b.\n";
		return -1;
	}

	std::ifstream infile(argv[1], std::ios::binary);
	if (!infile) {
		std::cerr << "Não foi possível abrir o arquivo informado.\n";
		return -1;
	}

    int rank, nProcs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcs);

	int valorVetor = std::atoi(argv[3]);

	int formato = std::atoi(argv[2]);
	if (formato == 2) {
		calcularMTX(rank, nProcs, infile, valorVetor);
	} else if (formato == 1) {
		calcularBoeing(rank, nProcs, infile, valorVetor);
	} else {
		std::cerr << "Formato de arquivo inválido.\n";
		return -1;
	}

    MPI_Finalize();

	return 0;
}
