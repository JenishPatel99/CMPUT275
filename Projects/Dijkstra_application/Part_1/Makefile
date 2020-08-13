CC=g++
OBJS = server.o digraph.o dijkstra.o
CFLAGS=-c -Wall -O2
LFLAGS=-static
PROGRAM=server

# Links all object files and creates server executable
server: $(OBJS) 
	$(CC) $(OBJS) -o server $(LFLAGS)

# compiles dijkstra.cpp and creates object file
dijkstra.o: dijkstra.h heap.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

# compiles server.cpp and creates object file
server.o: digraph.cpp wdigraph.h dijkstra.h
	$(CC) server.cpp -o server.o $(CFLAGS)

# compiles digraph.cpp and creates object file
digraph.o: digraph.h 
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)
	
clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)