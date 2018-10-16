CC = mpicxx
CFLAGS = -O3 -Wall -std=c++11
LIBS =
SOURCES = src/*.cpp
OBJECT = gradiente_conjugado
DEBUG = #-g
PROFILER = #-lprofiler

main:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(OBJECT) $(DEBUG) $(PROFILER)

run:
	mpirun -np $(NPROCS) ./$(OBJECT) $(CAMINHO) $(FORMATO) $(VALORB)

completo: main run

# Exemplo para rodar
# make completo NPROCS=4 CAMINHO=arquivos/hnrq1.mtx FORMATO=2 VALORB=5
# ou make
# seguido de
# make run NPROCS=4 CAMINHO=arquivos/hnrq1.mtx FORMATO=2 VALORB=5
# saida
# 0.744681
# 0.478723
# 0.319149
# 0.478723
# 0.744681
