#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"
#include "DadosMPI.hpp"
#include "SizesMPI.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <mpi.h>
#include <iohb.h>

#ifdef MPE_LOG
#include <mpe.h>
#endif

bool carregarVetoresCSC(const std::string & arquivo, std::vector<double> & values, std::vector<int> & colsPtr, std::vector<int> & rowsIdx, int & nLinhasMatriz) {
	char * _lixo1;
	int nLinhas, nColunas, nValores, _lixo2;
	int retorno = readHB_info(arquivo.c_str(), &nLinhas, &nColunas, &nValores, &_lixo1, &_lixo2);

	if (retorno == 0) {
		std::cerr << "Erro ao ler arquivo.\n";
		return false;
	}

	nLinhasMatriz = nColunas;

	values.resize(nValores);
	rowsIdx.resize(nValores);
	colsPtr.resize(nColunas + 1);

	retorno = readHB_mat_double(arquivo.c_str(), &colsPtr[0], &rowsIdx[0], &values[0]);

	if (retorno == 0){
		std::cerr << "Erro ao ler arquivo.\n";
		return false;
	}

	return true;
}

void calcularBoeing(const int rank, const int size, const std::string & arquivo, const int valorVetor) {
	std::vector<DadosMPI> dados;
	dados.reserve(size);
	std::vector<SizesMPI> sizes;
	sizes.reserve(size);

	const int lentag = 0;
	const int datatag = 1;

	int nLinhasMatriz = 0;
	if (rank == 0) {
		std::vector<double> values;
		std::vector<int> colsPtr;
		std::vector<int> rowsIdx;

		if (!carregarVetoresCSC(arquivo, values, colsPtr, rowsIdx, nLinhasMatriz)) {
			return;
		}

		int sizeColsPtr = colsPtr.size();
		int colPorProc = colsPtr.size() / size;
		int restoColsProc = colsPtr.size() % size;
		for (int proc = 0; proc < size; ++proc) {
			DadosMPI d;

			int resto = proc == size - 1 ? restoColsProc : 0;
			int colFinal = ((proc + 1) * colPorProc) + resto;
			int offsetCol = -1;
			for (int colAtual = proc * colPorProc; colAtual < colFinal && colAtual < sizeColsPtr; ++colAtual) {
				if (offsetCol == -1) {
					offsetCol = colsPtr[colAtual];
				}

				if (colAtual < sizeColsPtr - 1) {
					d._colunas.push_back(colAtual);
				}

				int colProx = colAtual + 1;
				if (colProx < sizeColsPtr) {
					for (int j = colsPtr[colAtual]; j < colsPtr[colProx]; ++j) {
						d._values.push_back(values[j]);
						d._rowsIdx.push_back(rowsIdx[j]);
					}
				}

				d._colsPtr.push_back(colsPtr[colAtual] - offsetCol);
			}

			if (colFinal < sizeColsPtr) {
				d._colsPtr.push_back(colsPtr[colFinal] - offsetCol);
			}

			if (!d._values.empty()) {
				dados.push_back(d); // proc
				sizes.push_back(SizesMPI(d._values.size(), d._rowsIdx.size(), d._colsPtr.size(), d._colunas.size()));
			}
		}
	}

	MPI_Bcast(&nLinhasMatriz, 1, MPI_INT, 0, MPI_COMM_WORLD);
	SparseMatrix A(rank, size, nLinhasMatriz, nLinhasMatriz);

	#ifdef MPE_LOG
	int evSend1, evSend2, evRecv1, evRecv2;

	MPE_Init_log();

	evSend1 = MPE_Log_get_event_number();
	evSend2 = MPE_Log_get_event_number();
	evRecv1 = MPE_Log_get_event_number();
	evRecv2 = MPE_Log_get_event_number();

	if (rank == 0) {
		MPE_Describe_state(evSend1, evSend2, "Send", "gray");
		MPE_Describe_state(evRecv1, evRecv2, "Recv", "red");
	}

	MPE_Start_log();
	#endif

	if (rank == 0) {
		#ifdef MPE_LOG
		MPE_Log_event(evSend1, 0, "Inicio do Send");
		#endif

		for (int proc = 1; proc < size; ++proc) {
			MPI_Send(MPI_BOTTOM, 1, sizes[proc].getMpiType(), proc, lentag, MPI_COMM_WORLD);
			MPI_Send(MPI_BOTTOM, 1, dados[proc].getMpiType(), proc, datatag, MPI_COMM_WORLD);
		}

		#ifdef MPE_LOG
		MPE_Log_event(evSend2, 0, "Fim do Send");
		#endif

		A.setValues(dados[0]._values);
		A.setRowsIdx(dados[0]._rowsIdx);
		A.setColsPtr(dados[0]._colsPtr);
		A.setColunas(dados[0]._colunas);
	} else {
		#ifdef MPE_LOG
		MPE_Log_event(evRecv1, 0, "Inicio do Recv");
		#endif

		SizesMPI s;
		MPI_Recv(MPI_BOTTOM, 1, s.getMpiType(), 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		DadosMPI d(s.getSizeValues(), s.getSizeRowsIdx(), s.getSizeColsPtr(), s.getSizeColunas());
	    MPI_Recv(MPI_BOTTOM, 1, d.getMpiType(), 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		#ifdef MPE_LOG
		MPE_Log_event(evRecv2, 0, "Fim do Recv");
		#endif

		A.setValues(d._values);
		A.setRowsIdx(d._rowsIdx);
		A.setColsPtr(d._colsPtr);
		A.setColunas(d._colunas);
	}

	#ifdef MPE_LOG
	MPE_Finish_log("jumpshot");
	#endif

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

	if (argc != 3) {
		std::cerr << "<$1> = caminho do arquivo\n"
				  << "<$2> = valor do vetor b.\n";

		MPI_Abort(MPI_COMM_WORLD, 1);
		return -1;
	}

	std::string arquivo = argv[1];
	int valorVetor = std::atoi(argv[2]);

	calcularBoeing(rank, size, arquivo, valorVetor);

	MPI_Finalize();

	return 0;
}
