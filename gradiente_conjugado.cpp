#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <fstream>

/*
g++ -O3 -Wall -std=c++14 gradiente_conjugado.cpp -o gradiente_conjugado
gprof
g++ -O2 -Wall -std=c++14 gradiente_conjugado.cpp -o gradiente_conjugado -pg
*/

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Vector2D = Vector<Vector<T>>;

class ColumnVector {
public:
	ColumnVector(std::initializer_list<double> list) : _matrix(list), _size(_matrix.size()) {}
	ColumnVector(int size) : _matrix(size), _size(size) {}
	ColumnVector(int size, int value) : _matrix(size, value), _size(size) {}

	inline int getSize() const {
		return _size;
	}

	inline double get(int idx) const {
		return _matrix[idx];
	}

	inline void set(int idx, double value) {
		_matrix[idx] = value;
	}

	ColumnVector T() const {
		return ColumnVector(*this);
	}

	double operator*(const ColumnVector & b) const {
		double res = 0.0;

		assert(_size == b.getSize());

		for (auto i = 0; i < _size; ++i) {
			res += _matrix[i] * b.get(i);
		}

		return res;
	}

	ColumnVector operator*(const double b) const {
		ColumnVector colVector(_size);
		for (auto i = 0; i < _size; ++i) {
			colVector.set(i, _matrix[i] * b);
		}

		return colVector;
	}

	ColumnVector operator+(const ColumnVector & b) const {
		assert(_size == b.getSize());

		ColumnVector colVector(_size);
		for (auto i = 0; i < _size; ++i) {
			colVector.set(i, _matrix[i] + b.get(i));
		}

		return colVector;
	}

	ColumnVector operator+(const double b) const {
		ColumnVector colVector(_size);
		for (auto i = 0; i < _size; ++i) {
			colVector.set(i, _matrix[i] + b);
		}

		return colVector;
	}

	ColumnVector operator-(const ColumnVector & b) const {
		assert(_size == b.getSize());

		ColumnVector colVector(_size);
		for (auto i = 0; i < _size; ++i) {
			colVector.set(i, _matrix[i] - b.get(i));
		}

		return colVector;
	}

	ColumnVector operator-(const double b) const {
		ColumnVector colVector(_size);
		for (auto i = 0; i < _size; ++i) {
			colVector.set(i, _matrix[i] - b);
		}

		return colVector;
	}

	void print() const {
		for (const auto & x : _matrix) {
			std::cout << x << "\n";
		}
	}

private:
	Vector<double> _matrix;
	int _size;
};

class SparseMatrix {
public:
	SparseMatrix(int rows, int cols) : _rows(rows), _cols(cols), _matrix(rows, Vector<double>(cols, 0)) {}

	inline void set(int row, int col, double value) {
		_matrix[row][col] = value;
	}

	inline double get(int row, int col) const {
		return _matrix[row][col];
	}

	inline int getCols() const {
		return _cols;
	}

	inline int getRows() const {
		return _rows;
	}

	SparseMatrix T() const {
		SparseMatrix matrix(_rows, _cols);
		for (auto i = 0; i < _rows; i++) {
			for (auto j = 0; j < _cols; j++) {
				matrix.set(i, j, _matrix[j][i]);
			}
		}
		return matrix;
	}

	SparseMatrix operator*(const SparseMatrix & b) const {
		const int bCols = b.getCols();

		SparseMatrix matrix(_rows, bCols);
		for (auto j = 0; j < bCols; ++j) {
			for (auto k = 0; k < _cols; ++k) {
				for (auto i = 0; i < _rows; ++i) {
					matrix.set(i, j, matrix.get(i, j) + (_matrix[i][k] * b.get(k, j)));
				}
			}
		}

		return matrix;
	}

	SparseMatrix operator*(const double b) const {
		SparseMatrix matrix(_rows, _cols);
		for (auto i = 0; i < _rows; ++i) {
			for (auto j = 0; j < _cols; ++j) {
				matrix.set(i, j, _matrix[i][j] * b);
			}
		}

		return matrix;
	}

	ColumnVector operator*(const ColumnVector & b) const {
		const int bRows = b.getSize();

		assert(_rows == bRows);

		ColumnVector colVector(bRows);
		for (auto i = 0; i < _rows; ++i) {
			for (auto j = 0; j < _rows; ++j) {
				colVector.set(i, colVector.get(i) + (_matrix[i][j] * b.get(j)));
			}
		}

		return colVector;
	}

	SparseMatrix operator+(const double b) const {
		SparseMatrix matrix(_rows, _cols);
		for (auto i = 0; i < _rows; ++i) {
			for (auto j = 0; j < _cols; ++j) {
				matrix.set(i, j, _matrix[i][j] + b);
			}
		}

		return matrix;
	}

	SparseMatrix operator-(const double b) const {
		SparseMatrix matrix(_rows, _cols);
		for (auto i = 0; i < _rows; ++i) {
			for (auto j = 0; j < _cols; ++j) {
				matrix.set(i, j, _matrix[i][j] - b);
			}
		}

		return matrix;
	}

	void print() const {
		for (const auto & v : _matrix) {
			for (const auto & x : v) {
				std::cout << x << " ";
			}
			std::cout << "\n";
		}
	}

private:
	int _rows;
	int _cols;
	Vector2D<double> _matrix;
};

ColumnVector gradiente_conjugado(const SparseMatrix & A, const ColumnVector & b) {
	int imax = 1000;
	double erro = 0.00001;
	int n = A.getRows();
	ColumnVector x(n);
	int i = 1;
	ColumnVector r(b);
	ColumnVector d(b);
	double sigma_novo = r * r;
	double sigma0 = sigma_novo;

	while (i < imax && sigma_novo > (std::pow(erro, 2) * sigma0)) {
		ColumnVector q = A * d;
		double alpha = sigma_novo / (d * q);
		x = x + (d * alpha);

		if (i % 50 == 0) {
			r = b - (A * x);
		} else {
			r = r - (q * alpha);
		}

		double sigma_velho = sigma_novo;
		sigma_novo = r * r;
		double beta = sigma_novo / sigma_velho;
		d = r + (d * beta);
		++i;
	}

	return x;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Arquivo .mtx deve ser informado.\n";
		return -1;
	}

	std::ifstream infile(argv[1]);
	if (!infile) {
		std::cerr << "Não foi possível abrir o arquivo informado.\n";
		return -1;
	}

	std::string nop;
	std::getline(infile, nop); // pula 1 linha

	int rows, cols, lines;
	infile >> rows >> cols >> lines;

	if (rows != cols) {
		std::cerr << "O nro de linhas deve ser igual ao de colunas.\n";
		return -1;
	}

	SparseMatrix A(rows, cols);

	int row, col; double value;
	while (infile >> row >> col >> value) {
		A.set(row - 1, col - 1, value);
	}

	ColumnVector b(rows, 5); // qual valor ?

	std::cout << "gradiente_conjugado\n";
	ColumnVector res = gradiente_conjugado(A, b);
	res.print();

	return 0;
}
