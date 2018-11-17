#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

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

struct DadosMPI {
	std::vector<double> values;
	std::vector<int> rowsIdx;
	std::vector<int> colsPtr;
	std::vector<int> colunas;
};

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

void calcularBoeing(const int rank, const int size, const std::string & arquivo, const int valorVetor) {
	std::vector<DadosMPI> dados;
	dados.reserve(size);

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
					d.colunas.push_back(colAtual);
				}

				int colProx = colAtual + 1;
				if (colProx < sizeColsPtr) {
					for (int j = colsPtr[colAtual]; j < colsPtr[colProx]; ++j) {
						d.values.push_back(values[j]);
						d.rowsIdx.push_back(rowsIdx[j]);
					}
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
			sendVectorMPI(proc, dados[proc].values, MPI_DOUBLE);
			sendVectorMPI(proc, dados[proc].rowsIdx, MPI_INT);
			sendVectorMPI(proc, dados[proc].colsPtr, MPI_INT);
			sendVectorMPI(proc, dados[proc].colunas, MPI_INT);
		}
		#ifdef MPE_LOG
		MPE_Log_event(evSend2, 0, "Fim do Send");
		#endif

		A.setValues(dados[0].values);
		A.setRowsIdx(dados[0].rowsIdx);
		A.setColsPtr(dados[0].colsPtr);
		A.setColunas(dados[0].colunas);
	} else {
		#ifdef MPE_LOG
		MPE_Log_event(evRecv1, 0, "Inicio do Recv");
		#endif

		std::vector<double> values_rec;
		recvVectorMPI(values_rec, MPI_DOUBLE);

		std::vector<int> rowsIdx_rec;
		recvVectorMPI(rowsIdx_rec, MPI_INT);

		std::vector<int> colsPtr_rec;
		recvVectorMPI(colsPtr_rec, MPI_INT);

		std::vector<int> colunas_rec;
		recvVectorMPI(colunas_rec, MPI_INT);

		#ifdef MPE_LOG
		MPE_Log_event(evRecv2, 0, "Fim do Recv");
		#endif

		A.setValues(values_rec);
		A.setRowsIdx(rowsIdx_rec);
		A.setColsPtr(colsPtr_rec);
		A.setColunas(colunas_rec);
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
