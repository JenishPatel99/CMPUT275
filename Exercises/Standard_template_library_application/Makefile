CC=g++
OBJS = student calc complex_postfix.o student_hash.o
CFLAGS=-c -Wall -O2
LFLAGS=-static
PROGRAM=student calc

all: student calc

student: student_hash.o
	$(CC) student_hash.o -o student
	
student_hash.o:student_hash.cpp
	$(CC) student_hash.cpp -o student_hash.o $(CFLAGS)

calc: complex_postfix.o
	$(CC) complex_postfix.o -o calc

complex_postfix.o: complex_postfix.cpp
	$(CC) complex_postfix.cpp -o complex_postfix.o $(CFLAGS)	
	
clean:
	@rm -f $(OBJS)
	@rm -f $(PROGRAM)
