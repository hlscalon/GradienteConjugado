#ifndef SIZES_MPI_HPP
#define SIZES_MPI_HPP

#include <mpi.h>

struct SizesMPI {
	int _sizes[4];

    SizesMPI() : _mpiType(MPI_DATATYPE_NULL) {}
    SizesMPI(int sizeValues, int sizeRowsIdx, int sizeColsPtr, int sizeColunas) :
        _mpiType(MPI_DATATYPE_NULL) {
            _sizes[0] = sizeValues;
            _sizes[1] = sizeRowsIdx;
            _sizes[2] = sizeColsPtr;
            _sizes[3] = sizeColunas;
    }

    ~SizesMPI() {
    	if (_mpiType != MPI_DATATYPE_NULL) {
    		MPI_Type_free(&_mpiType);
    	}
    }

    const int getSizeValues() const {
        return _sizes[0];
    }

    const int getSizeRowsIdx() const {
        return _sizes[1];
    }

    const int getSizeColsPtr() const {
        return _sizes[2];
    }

    const int getSizeColunas() const {
        return _sizes[3];
    }

    const MPI_Datatype getMpiType();

    void invalidateMpiType();

private:
    MPI_Datatype _mpiType;
    void makeMpiType();
};

#endif // SIZES_MPI_HPP
