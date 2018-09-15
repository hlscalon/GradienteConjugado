#include "GradienteConjugado.hpp"
#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

#include <iostream>
#include <fstream>

void calcularMTX(std::ifstream & infile) {
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

	ColumnVector b(rows, 5); // qual valor ?

	ColumnVector res = gradienteConjugado(A, b);
	//res.print();
}

void calcularBoeing(const std::ifstream & infile) {

}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "<$1> = caminho do arquivo.\n<$2> = formato do arquivo (1 = Harwell-Boeing, 2 = MTX)\n";
		return -1;
	}

	std::ifstream infile(argv[1]);
	if (!infile) {
		std::cerr << "Não foi possível abrir o arquivo informado.\n";
		return -1;
	}

	int formato = std::atoi(argv[2]);
	if (formato == 2) {
		calcularMTX(infile);
	} else if (formato == 1) {
		calcularBoeing(infile);
	} else {
		std::cerr << "Formato de arquivo inválido.\n";
		return -1;
	}

	return 0;
}
