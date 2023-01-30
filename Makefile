C = g++
CFLAGS = -Wall -g

main: genLWEsamples.o ; $(CC) $(CFLAGS) -L /usr/local/include/palisade. -v -o main genLWEsamples.o

genLWEsamples.o: genLWEsamples.cpp ; $(CC) $(CFLAGS) -c genLWEsamples.cpp -I /usr/local/include/palisade/pke/ -I /usr/local/include/palisade/core -I /usr/local/include/palisade


clean: 
	rm *.o 