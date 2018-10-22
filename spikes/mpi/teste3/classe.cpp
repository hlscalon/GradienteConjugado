#include <mpi.h>
#include <vector>
#include <iostream>
#include <numeric>

struct Matrix {
    Matrix(int rank, int nprocs) : _rank(rank), _nprocs(nprocs) {
        if (rank == 0) {
            _nums = std::vector<int>{1, 4, 9, 16, 17, 24, 30, 35};
        }
    }

    std::vector<int> operator*(int valor) const {
        int size = 0;
        if (_rank == 0) {
            size = _nums.size() / _nprocs;
        }

        std::vector<int> res;

        MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        res.resize(size);

        // dividir matriz
        MPI_Scatter(const_cast<int*>(_nums.data()), size, MPI_INT, res.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

        // std::cout << "rank = " << _rank << " res = ";
        // for (const auto & r : res) std::cout << r << " ";
        // std::cout << "\n";

        for (int i = 0; i < size; ++i) {
            res[i] *= valor;
        }

        // reunir no 0
        std::vector<int> res_total;
        if (_rank == 0) {
            res_total.resize(_nums.size());
        }

        MPI_Gather(res.data(), size, MPI_INT, res_total.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

        return res_total;
    }

    void printar() const {
        std::cout << "rank = " << _rank << "\n";
        for (const auto & n : _nums) std::cout << n << " ";
        std::cout << "\n";
    }
private:
    int _rank;
    int _nprocs;
    std::vector<int> _nums;
};

void run(const Matrix & t, int rank) {
    int sum = 0, i = 0;
    std::vector<int> res1;
    while (sum < 1000) {
        res1 = t * i++;

        if (rank == 0) {
            sum = std::accumulate(res1.begin(), res1.end(), 0);
        }

        MPI_Bcast(&sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        t.printar();
        std::cout << "sum = " << sum << " res1 = "; for (auto r : res1) std::cout << r << " "; std::cout << "\n";
    }
}

int main(int argc, char** argv) {
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        if (rank == 0)
            std::cerr << "Require at least 2 tasks" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    Matrix t(rank, size);
    run(t, rank);

    MPI_Finalize();

    return 0;
}
