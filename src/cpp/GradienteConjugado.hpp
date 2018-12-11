#ifndef GRADIENTE_CONJUGADO_HPP
#define GRADIENTE_CONJUGADO_HPP

#include "SparseMatrix.hpp"
#include "ColumnVector.hpp"

ColumnVector gradienteConjugado(const SparseMatrix & A, const ColumnVector & b, int & iteracoes);

#endif // GRADIENTE_CONJUGADO_HPP