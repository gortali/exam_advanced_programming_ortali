all: bst

CC = g++
OPT_LEV = -O3
CPPL = -std=c++11
#PROF =  -g

$bst: bst.cpp Makefile
	$(CC) bst.cpp -o bst.o $(CPPL) -lm $(OPT_LEV) $(PROF) -Wall Wextra
