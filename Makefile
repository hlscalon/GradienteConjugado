CCPP = mpicxx
CC = gcc
CPPFLAGS = -O3 -Wall -std=c++11
CFLAGS = -Wno-pointer-to-int-cast -Wno-format-security
SOURCES = src/cpp/*.cpp
OBJECTS = iohb.o
OBJECT = gradiente_conjugado
DEBUG = #-DDEBUG #-g
PROFILER = -lprofiler
MPE = -DMPE_LOG -lmpe
IOHB_DIR = src/libs/iohb10
LIBS = -I$(IOHB_DIR)

main:
	$(CCPP) $(CPPFLAGS) $(OBJECTS) $(LIBS) $(SOURCES) $(DEBUG) $(PROFILER) $(MPE) -o $(OBJECT)

iohb:
	$(CC) $(CFLAGS) $(LIBS) -c $(IOHB_DIR)/iohb.c

all: iohb main run

run:
	mpirun -np $(NPROCS) ./$(OBJECT) $(ARQ) $(VALORB) $(RES) $(TEMPO)
