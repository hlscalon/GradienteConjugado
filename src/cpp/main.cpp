#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <iohb.h>

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

void calcularBoeing(const std::string & arquivo, const int valorVetor) {
	int nLinhasMatriz = 0;
	std::vector<double> values;
	std::vector<int> colsPtr;
	std::vector<int> rowsIdx;

	if (!carregarVetoresCSC(arquivo, values, colsPtr, rowsIdx, nLinhasMatriz)) {
		return;
	}

	SparseMatrix A(nLinhasMatriz, nLinhasMatriz);
	A.setValues(values);
	A.setRowsIdx(rowsIdx);
	A.setColsPtr(colsPtr);

	#ifdef DEBUG
	A.printCSC();
	#endif

	ColumnVector b(nLinhasMatriz, valorVetor); // qual valor ?
	ColumnVector res = gradienteConjugado(A, b);

	res.print();
}

int main(int argc, char *argv[]) {
	// todo: pre condicionadores do gradiente conjugado
	if (argc != 3) {
		std::cerr << "<$1> = caminho do arquivo\n"
				  << "<$2> = valor do vetor b.\n";
		return -1;
	}

	std::string arquivo = argv[1];
	int valorVetor = std::atoi(argv[2]);

	calcularBoeing(arquivo, valorVetor);

	return 0;
}
