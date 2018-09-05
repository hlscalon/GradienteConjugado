#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <fstream>

//#define MULT_TUDO 1

/*
g++ -O3 -Wall -std=c++14 gradiente_conjugado.cpp -o gradiente_conjugado
gprof
g++ -O2 -Wall -std=c++14 gradiente_conjugado.cpp -o gradiente_conjugado -pg
g++ -O2 -Wall -std=c++14 gradiente_conjugado.cpp -o gradiente_conjugado -lprofiler
./gradiente_conjugado arquivos/bcsstk13.mtx
LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=/tmp/gradiente_conjugado.prof ./gradiente_conjugado arquivos/bcsstk13.mtx
pprof --web gradiente_conjugado /tmp/gradiente_conjugado.prof
*/

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Vector2D = Vector<Vector<T>>;

class ColumnVector {
public:
	ColumnVector(const std::initializer_list<double> list) : _matrix(list), _size(_matrix.size()) {}
	ColumnVector(const int size) : _matrix(size), _size(size) {}
	ColumnVector(const int size, const int value) : _matrix(size, value), _size(size) {}

	inline int getSize() const {
		return _size;
	}

	inline double get(const int idx) const {
		return _matrix[idx];
	}

	inline double & get(const int idx) {
		return _matrix[idx];
	}

	inline void set(const int idx, const double value) {
		_matrix[idx] = value;
	}

	inline double & operator()(const int idx) {
		return get(idx);
	}

	inline double operator()(const int idx) const {
		return get(idx);
	}

	double operator*(const ColumnVector & b) const {
		double res = 0.0;

		assert(_size == b.getSize());

		for (auto i = 0; i < _size; ++i) {
			res += get(i) * b(i);
		}

		return res;
	}

	ColumnVector operator*(const double b) const {
		ColumnVector colVector(*this);
		for (auto i = 0; i < _size; ++i) {
			colVector(i) *= b;
		}

		return colVector;
	}

	ColumnVector operator+(const ColumnVector & b) const {
		assert(_size == b.getSize());

		ColumnVector colVector(b);
		for (auto i = 0; i < _size; ++i) {
			colVector(i) += get(i);
		}

		return colVector;
	}

	ColumnVector operator-(const ColumnVector & b) const {
		assert(_size == b.getSize());

		ColumnVector colVector(*this);
		for (auto i = 0; i < _size; ++i) {
			colVector(i) -= b(i);
		}

		return colVector;
	}

	void print() const {
		for (const auto & x : _matrix) {
			std::cout << x << "\n";
		}
	}

	void printT() const {
		for (const auto & x : _matrix) {
			std::cout << x << " ";
		}
		std::cout << "\n";
	}

private:
	Vector<double> _matrix;
	int _size;
};

class SparseMatrix {
public:
	SparseMatrix(const int nRows, const int nCols, const int nValues) : _nRows(nRows), _nCols(nCols), _lastCol(-1), _nValues(nValues), _values(nValues), _rowsIdx(nValues) {}

	inline void set(const int row, const int col, const double value) {
		_values[_colPtr] = value;
		_rowsIdx[_colPtr] = row;

		if (col != _lastCol) {
			_colsPtr.push_back(_colPtr);
			_lastCol = col;
		}

		_colPtr++;
	}

	inline void updateColsPtr() {
		_colsPtr.push_back(_colPtr++);
	}

	inline int getCols() const {
		return _nCols;
	}

	inline int getRows() const {
		return _nRows;
	}

	ColumnVector operator*(const ColumnVector & b) const {
		const int bCols = b.getSize();

		assert(_nCols == bCols);

		ColumnVector colVector(bCols);

#ifdef MULT_TUDO
		for (auto i = 0; i < bCols; ++i) {
			for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
				colVector(_rowsIdx[k]) += _values[k] * b(i);
			}
		}
#else
		// mais lento que o de baixo
		/*
		int metade = std::ceil(bCols / 2); // pega a linha do meio
		for (auto i = 0; i < metade; ++i) {
			for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
				const double tmp = _values[k] * b(i);

				colVector(_rowsIdx[k]) += tmp;
				colVector(bCols - 1 - _rowsIdx[k]) += tmp;
			}
		}

		for (auto k = _colsPtr[metade]; k < _colsPtr[metade + 1]; ++k) {
			colVector(_rowsIdx[k]) += _values[k] * b(metade);
		}
		*/

		int metade = std::ceil(bCols / 2); // pega a linha do meio
		for (auto i = 0; i < metade; ++i) { // bCols
			for (auto k = _colsPtr[i]; k < _colsPtr[i + 1]; ++k) {
				colVector(_rowsIdx[k]) += _values[k] * b(i);
			}
		}

		const ColumnVector aux(colVector);
		for (auto i = 0; i < bCols; ++i) {
			colVector(i) += aux(bCols - 1 - i);
		}

		for (auto k = _colsPtr[metade]; k < _colsPtr[metade + 1]; ++k) {
			colVector(_rowsIdx[k]) += _values[k] * b(metade);
		}
#endif

		return colVector;
	}

	void printCSC() const {
		std::cout << "_values: " << "\n";
		for (const auto & v : _values) {
			std::cout << v << " ";
		}
		std::cout << "\n_rowsIdx: " << "\n";
		for (const auto & v : _rowsIdx) {
			std::cout << v << " ";
		}
		std::cout << "\n_colsPtr: " << "\n";
		for (const auto & v : _colsPtr) {
			std::cout << v << " ";
		}
		std::cout << "\n";
	}

private:
	int _nRows;
	int _nCols;
	int _colPtr;
	int _lastCol;
	int _nValues;
	Vector<double> _values;
	Vector<int> _rowsIdx;
	Vector<int> _colsPtr;
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

	SparseMatrix A(rows, cols, lines);

	int row, col; double value;
	while (infile >> row >> col >> value) {
		A.set(row - 1, col - 1, value);
	}
	A.updateColsPtr();

	ColumnVector b(rows, 5); // qual valor ?

	ColumnVector res = gradiente_conjugado(A, b);
	//res.print();

	return 0;
}
