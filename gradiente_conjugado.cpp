#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

/*
g++ -O3 -Wall -std=c++14 gradiente_conjugado.cpp -o gradiente_conjugado
*/

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Vector2D = Vector<Vector<T>>;

template <typename T>
Vector2D<T> transpose(const Vector2D<T> & data) {
	Vector2D<T> resVec(data[0].size(), Vector<T>(data.size()));

	for (auto i = 0; i < data[0].size(); i++) {
		for (auto j = 0; j < data.size(); j++) {
			resVec[i][j] = data[j][i];
		}
	}

	return resVec;
}

template <typename T>
Vector<T> transpose(const Vector<T> & data) {
	return data;
}

template <typename T>
double multiply(const Vector<T> & a, const Vector<T> & b) {
	double res = 0.0;

	const int aRows = a.size();
	const int bRows = b.size();

	assert(aRows == bRows);

	for (auto i = 0; i < aRows; ++i) {
		res += a[i] * b[i];
	}

	return res;
}

template <typename T>
Vector2D<T> multiply(const Vector2D<T> & a, const Vector2D<T> & b) {
	const int aRows = a.size();
	const int aCols = a[0].size();
	const int bCols = b[0].size();

	Vector2D<T> resVec(aRows, Vector<T>(bCols, 0));
	for (auto j = 0; j < bCols; ++j) {
		for (auto k = 0; k < aCols; ++k) {
			for (auto i = 0; i < aRows; ++i) {
				resVec[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	return resVec;
}

template <typename T>
Vector2D<T> multiply(const Vector2D<T> & a, const double b) {
	const int aRows = a.size();
	const int aCols = a[0].size();

	Vector2D<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		for (auto j = 0; j < aCols; ++j) {
			resVec[i][j] *= b;
		}
	}

	return resVec;
}

template <typename T>
Vector<T> multiply(const Vector<T> & a, const double b) {
	const int aRows = a.size();

	Vector<T> resVec(aRows);
	for (auto i = 0; i < aRows; ++i) {
		resVec[i] = a[i] * b;
	}

	return resVec;
}

template <typename T>
Vector<T> multiply(const Vector2D<T> & a, const Vector<T> & b) {
	const int aRows = a.size();
	const int bRows = b.size();

	assert(aRows == bRows);

	Vector<T> resVec(bRows);
	for (auto i = 0; i < aRows; ++i) {
		for (auto j = 0; j < aRows; ++j) {
			resVec[i] += a[i][j] * b[j];
		}
	}

	return resVec;
}

template <typename T>
Vector<T> add(const Vector<T> & a, const Vector<T> & b) {
	const int aRows = a.size();
	const int bRows = b.size();

	assert(aRows == bRows);

	Vector<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		resVec[i] += b[i];
	}

	return resVec;
}

template <typename T>
Vector<T> add(const Vector<T> & a, const T b) {
	const int aRows = a.size();

	Vector<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		resVec[i] += b;
	}

	return resVec;
}

template <typename T>
Vector2D<T> add(const Vector2D<T> & a, const T b) {
	const int aRows = a.size();
	const int aCols = a[0].size();

	Vector2D<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		for (auto j = 0; j < aCols; ++j) {
			resVec[i][j] += b;
		}
	}

	return resVec;
}

template <typename T>
Vector<T> subtract(const Vector<T> & a, const Vector<T> & b) {
	const int aRows = a.size();
	const int bRows = b.size();

	assert(aRows == bRows);

	Vector<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		resVec[i] -= b[i];
	}

	return resVec;
}

template <typename T>
Vector<T> subtract(const Vector<T> & a, const T b) {
	const int aRows = a.size();

	Vector<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		resVec[i] -= b;
	}

	return resVec;
}

template <typename T>
Vector2D<T> subtract(const Vector2D<T> & a, const T b) {
	const int aRows = a.size();
	const int aCols = a[0].size();

	Vector2D<T> resVec(a);
	for (auto i = 0; i < aRows; ++i) {
		for (auto j = 0; j < aCols; ++j) {
			resVec[i][j] -= b;
		}
	}

	return resVec;
}

template <typename T>
void print(const Vector<T> & vec) {
	for (const auto & x : vec) {
		std::cout << x << "\n";
	}
}

template <typename T>
void print(const Vector2D<T> & vec) {
	for (const auto & v : vec) {
		for (const auto & x : vec) {
			std::cout << x << " ";
		}
		std::cout << "\n";
	}
}

Vector<double> gradiente_conjugado(const Vector2D<double> & A, const Vector<double> & b) {
	int imax = 1000;
	double erro = 0.00001;
	int n = A.size();
	Vector<double> x = Vector<double>(n);
	int i = 1;
	Vector<double> r(b);
	Vector<double> d(b);
	double sigma_novo = multiply(transpose(r), r);
	double sigma0 = sigma_novo;

	while (i < imax && sigma_novo > (std::pow(erro, 2) * sigma0)) {
		Vector<double> q = multiply(A, d);
		double alpha = sigma_novo / multiply(transpose(d), q);
		x = add(x, multiply(d, alpha));

		if (i % 50 == 0) {
			r = subtract(b, multiply(A, x));
		} else {
			r = subtract(r, multiply(q, alpha));
		}

		double sigma_velho = sigma_novo;
		sigma_novo = multiply(transpose(r), r);
		double beta = sigma_novo / sigma_velho;
		d = add(r, multiply(d, beta));
		++i;
	}

	return x;
}

int main() {

	Vector2D<double> A = {
		{5.0, 2.0, 1.0, 0.0, 0.0},
		{2.0, 5.0, 2.0, 1.0, 0.0},
		{1.0, 2.0, 5.0, 2.0, 1.0},
		{0.0, 1.0, 2.0, 5.0, 2.0},
		{0.0, 0.0, 1.0, 2.0, 5.0}
	};

	Vector<double> b = {5, 5, 5, 5, 5};

	std::cout << "gradiente_conjugado\n";
	print(gradiente_conjugado(A, b));

	return 0;
}
