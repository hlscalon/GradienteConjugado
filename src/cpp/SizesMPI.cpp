#include "SizesMPI.hpp"

const MPI_Datatype SizesMPI::getMpiType() {
    if (_mpiType == MPI_DATATYPE_NULL) {
        makeMpiType();
    }

    return _mpiType;
}

void SizesMPI::invalidateMpiType() {
    if (_mpiType != MPI_DATATYPE_NULL) {
        MPI_Type_free(&_mpiType);
    }
}

void SizesMPI::makeMpiType() {
    const int nblock = 1; // nro campos

    // tamanho de cada campo
    int blockCount[nblock] = {
        4
    };

    // tipos dos campos
    MPI_Datatype blockType[nblock] = {
        MPI_INT,
    };

    MPI_Aint offset[nblock];
    MPI_Get_address(&_sizes[0], &offset[0]);

    MPI_Type_struct(nblock, blockCount, offset, blockType, &_mpiType);
    MPI_Type_commit(&_mpiType);
}
