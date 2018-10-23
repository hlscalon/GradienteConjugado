CC = mpicxx
CFLAGS = -O3 -Wall -std=c++11
LIBS =
SOURCES = src/*.cpp
OBJECT = gradiente_conjugado
DEBUG = #-DDEBUG #-g
PROFILER = -lprofiler

main:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(OBJECT) $(DEBUG) $(PROFILER)

run:
	mpirun -np $(NPROCS) ./$(OBJECT) $(ARQ) $(TIPO) $(VALORB)

completo: main run
