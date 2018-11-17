#ifndef COLUMN_VECTOR_HPP
#define COLUMN_VECTOR_HPP

#include <vector>

class ColumnVector {
public:
	ColumnVector(const std::initializer_list<double> list) : _matrix(list), _size(_matrix.size()) {}
	ColumnVector(const int size) : _matrix(size), _size(size) {}
	ColumnVector(const int size, const int value) : _matrix(size, value), _size(size) {}
	ColumnVector(const std::vector<double> & matrix) : _matrix(matrix), _size(matrix.size()) {}

	inline double * data() {
		return _matrix.data();
	}

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

	double operator*(const ColumnVector & b) const;

	ColumnVector operator*(const double b) const;

	ColumnVector operator+(const ColumnVector & b) const;

	ColumnVector operator-(const ColumnVector & b) const;

	void print() const;

private:
	std::vector<double> _matrix;
	int _size;
};

#endif // COLUMN_VECTOR_HPP
