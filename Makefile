CC = g++
CFLAGS = -O3 -Wall -std=c++14
LIBS = -lprofiler
SOURCES = src/*.cpp
OBJECT = gradiente_conjugado

main:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o $(OBJECT) -g
