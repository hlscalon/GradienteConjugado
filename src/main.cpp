#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

void calcularMTX(std::ifstream & infile, int valorVetor) {
	std::string nop;
	std::getline(infile, nop); // pula 1 linha

	int rows, cols, lines;
	infile >> rows >> cols >> lines;

	if (rows != cols) {
		std::cerr << "O nro de linhas deve ser igual ao de colunas.\n";
		return;
	}

	SparseMatrix A(rows, cols, lines);

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

void calcularBoeing(std::ifstream & infile, int valorVetor) {
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

	// funciona para impar apenas
	int metade = nColunasMatriz / 2; // trunca para + 1
	SparseMatrix A(nLinhasMatriz, nColunasMatriz, nElementos, metade + 1);

	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::string linha; int i = 0; int idx = 0; std::istringstream iss;

	int ultIdxCol = 0;
	int colPtr;
	while (i++ < nLinhasColsPtr && std::getline(infile, linha)) {
		iss.str(linha);
		iss.clear();
		while (iss >> colPtr) {
			if (idx++ <= metade + 1) {
				A.setColPtr(colPtr - 1);
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
				A.setRowIdx(rowIdx - 1);
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
				A.setValue(value);
			}
		}
	}

	#ifdef DEBUG
	A.printCSC();
	#endif

	ColumnVector b(nLinhasMatriz, valorVetor); // qual valor ?
	ColumnVector res = gradienteConjugado(A, b);
	res.print();
}

int main(int argc, char *argv[]) {
	// todo: pre condicionadores do gradiente conjugado

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

	int valorVetor = std::atoi(argv[3]);

	int formato = std::atoi(argv[2]);
	if (formato == 2) {
		calcularMTX(infile, valorVetor);
	} else if (formato == 1) {
		calcularBoeing(infile, valorVetor);
	} else {
		std::cerr << "Formato de arquivo inválido.\n";
		return -1;
	}

	return 0;
}
