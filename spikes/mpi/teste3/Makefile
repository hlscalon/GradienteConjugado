main:
	mpicxx classe.cpp -O3 -Wall -std=c++14 -o classe

run:
	mpirun -np 4 ./classe

completo: main run
