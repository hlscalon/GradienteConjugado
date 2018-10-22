#include <mpi.h>
#include <set>
#include <vector>
#include <iostream>

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

   const int lentag=0;
   const int datatag=1;
   if (rank == 0) {
        int nums[] = {1,4,9,16};
        std::set<int> send_set(nums, nums+4);

        std::cout << "Rank " << rank << " sending set: ";
        for (std::set<int>::iterator i=send_set.begin(); i!=send_set.end(); i++)
            std::cout << *i << " ";
        std::cout << std::endl;

        // Send length, then data
        int len = send_set.size();
        MPI_Send( &len, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD );

        std::vector<int> send_vec;
        for (std::set<int>::iterator i=send_set.begin(); i!=send_set.end(); i++) 
            send_vec.push_back(*i);

        MPI_Send( send_vec.data(), len, MPI_INT, 1, datatag, MPI_COMM_WORLD );

    } else if (rank == 1) {

        int len;
        MPI_Recv( &len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // int recv_data[len];
        std::vector<int> recv_data(len);
        MPI_Recv( recv_data.data(), len, MPI_INT, 0, datatag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::set<int> recv_set;
        for (int i=0; i<len; i++)
            recv_set.insert(recv_data[i]);

        std::cout << "Rank " << rank << " got set: ";
        for (std::set<int>::iterator i=recv_set.begin(); i!=recv_set.end(); i++)
            std::cout << *i << " ";
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
