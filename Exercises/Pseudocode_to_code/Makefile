CC=g++
OBJS=textmatch textmatch.o flipflop flipflop.o 
CFLAGS=-c -Wall -O2
LFLAGS=-static
PROGRAM=flipflop textmatch

textmatch: textmatch.cpp
	$(CC) textmatch.cpp -o textmatch

textmatch.o: textmatch.cpp
	$(CC) textmatch.cpp -o textmatch.o $(CFLAGS)

flipflop: flipflop.cpp
	$(CC) flipflop.cpp -o flipflop

flipflop.o: flipflop.cpp
	$(CC) flipflop.cpp -o flipflop.o $(CFLAGS)

clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)
