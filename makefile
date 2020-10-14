CC = gcc
CFLAGS = -O3 -Wall

all : Battleship

Battleship : Battleship.c DoubleLinkedList.c
	$(CC) $(CFLAGS) DoubleLinkedList.c Battleship.c -o Battleship

run : Battleship
	./Battleship

.PHONY : clean
clean :
	rm -f Battleship DoubleLinkedList
