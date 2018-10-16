#ifndef GRADIENTE_CONJUGADO_HPP
#define GRADIENTE_CONJUGADO_HPP

#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

ColumnVector gradienteConjugado(int rank, SparseMatrix & A, const ColumnVector & b);

#endif // GRADIENTE_CONJUGADO_HPP