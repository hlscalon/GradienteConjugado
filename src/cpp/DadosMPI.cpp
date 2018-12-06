#include "DadosMPI.hpp"

const MPI_Datatype DadosMPI::getMpiType() {
    if (_mpiType == MPI_DATATYPE_NULL) {
        makeMpiType();
    }

    return _mpiType;
}

void DadosMPI::invalidateMpiType() {
    if (_mpiType != MPI_DATATYPE_NULL) {
        MPI_Type_free(&_mpiType);
    }
}

void DadosMPI::makeMpiType() {
    const int nblock = 4; // nro campos

    // tamanho de cada campo
    int blockCount[nblock] = {
        static_cast<int>(_values.size()),
        static_cast<int>(_rowsIdx.size()),
        static_cast<int>(_colsPtr.size()),
        static_cast<int>(_colunas.size())
    };

    // tipos dos campos
    MPI_Datatype blockType[nblock] = {
        MPI_DOUBLE,
        MPI_INT,
        MPI_INT,
        MPI_INT
    };

    MPI_Aint offset[nblock];
    MPI_Get_address(&_values[0], &offset[0]);
    MPI_Get_address(&_rowsIdx[0], &offset[1]);
    MPI_Get_address(&_colsPtr[0], &offset[2]);
    MPI_Get_address(&_colunas[0], &offset[3]);

    MPI_Type_struct(nblock, blockCount, offset, blockType, &_mpiType);
    MPI_Type_commit(&_mpiType);
}
