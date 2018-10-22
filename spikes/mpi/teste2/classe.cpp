#include <mpi.h>
#include <set>
#include <vector>
#include <iostream>

struct Teste {
    Teste(int rank) : _rank(rank) {
        if (rank == 0) {
            _nums = std::vector<int>{1, 4, 9, 16, 17, 24, 30};
        }
    }

    void somar() {
       const int lentag = 0;
       const int datatag = 1;
       if (_rank == 0) {
            std::cout << "Rank " << _rank << " sending set: ";
            for (const auto & n : _nums) std::cout << n << " ";
            std::cout << std::endl;

            // Send length, then data
            int len = _nums.size();
            MPI_Send(&len, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD);

            MPI_Send(_nums.data(), len, MPI_INT, 1, datatag, MPI_COMM_WORLD);

        } else if (_rank == 1) {
            int len;
            MPI_Recv(&len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // int recv_data[len];
            std::vector<int> recv_data(len);
            MPI_Recv(recv_data.data(), len, MPI_INT, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::cout << "Rank " << _rank << " got set: ";
            for (const auto & n : recv_data) std::cout << n << " ";
            std::cout << std::endl;
        }
    }

    void printar() {
        std::cout << "rank = " << _rank << "\n";
        for (const auto & n : _nums) std::cout << n << " ";
        std::cout << "\n";
    }
private:
    int _rank;
    std::vector<int> _nums;
};

int main(int argc,char** argv) {
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        if (rank == 0)
            std::cerr << "Require at least 2 tasks" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    Teste t(rank);
    t.printar();
    t.somar();

    MPI_Finalize();

    return 0;
}
