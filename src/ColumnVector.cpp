#include "ColumnVector.hpp"

#include <iostream>
#include <cassert>

double ColumnVector::operator*(const ColumnVector & b) const {
	double res = 0.0;

	assert(_size == b.getSize());

	for (auto i = 0; i < _size; ++i) {
		res += get(i) * b(i);
	}

	return res;
}

ColumnVector ColumnVector::operator*(const double b) const {
	ColumnVector colVector(*this);
	for (auto i = 0; i < _size; ++i) {
		colVector(i) *= b;
	}

	return colVector;
}

ColumnVector ColumnVector::operator+(const ColumnVector & b) const {
	assert(_size == b.getSize());

	ColumnVector colVector(b);
	for (auto i = 0; i < _size; ++i) {
		colVector(i) += get(i);
	}

	return colVector;
}

ColumnVector ColumnVector::operator-(const ColumnVector & b) const {
	assert(_size == b.getSize());

	ColumnVector colVector(*this);
	for (auto i = 0; i < _size; ++i) {
		colVector(i) -= b(i);
	}

	return colVector;
}

void ColumnVector::print() const {
	for (const auto & x : _matrix) {
		std::cout << x << "\n";
	}
}
