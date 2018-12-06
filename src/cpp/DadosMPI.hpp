#ifndef DADOS_MPI_HPP
#define DADOS_MPI_HPP

#include <vector>
#include <mpi.h>

struct DadosMPI {
    std::vector<double> _values;
    std::vector<int> _rowsIdx;
    std::vector<int> _colsPtr;
    std::vector<int> _colunas;

    DadosMPI() : _mpiType(MPI_DATATYPE_NULL) {}

    DadosMPI(int sizeValues, int sizeRowsIdx, int sizeColsPtr, int sizeColunas) : _mpiType(MPI_DATATYPE_NULL) {
        _values.resize(sizeValues);
        _rowsIdx.resize(sizeRowsIdx);
        _colsPtr.resize(sizeColsPtr);
        _colunas.resize(sizeColunas);
    }

    ~DadosMPI() {
    	if (_mpiType != MPI_DATATYPE_NULL) {
    		MPI_Type_free(&_mpiType);
    	}
    }

    const MPI_Datatype getMpiType();
    void invalidateMpiType();

private:
    MPI_Datatype _mpiType;
    void makeMpiType();
};

#endif // DADOS_MPI_HPP
