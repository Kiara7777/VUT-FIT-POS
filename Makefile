#
#Autor: Sara Skutova
#Projekt: 1. projekt do POS - 3 procesy + exit status
#Rok: 2017/2018
#

CC = gcc
CFLAGS = -Wall -g

all: fork 

fork: fork.c
	$(CC) $(CFLAGS) fork.c -o fork
	
clean:
	rm -f *.o fork core*