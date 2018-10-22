main:
	mpicxx teste_classes.cpp -O3 -Wall -std=c++14 -o teste_classes

run:
	mpirun -np $(NPROCS) ./teste_classes

completo: main run
