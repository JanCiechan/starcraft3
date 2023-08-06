
CC=g++

target: player.cpp
	$(CC) player.cpp -o player.o

clean:
	rm -rf *.o
