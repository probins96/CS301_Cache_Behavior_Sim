# makefile
# Name: Palmer Robins
# use to compile and test the assembler ASM for the STACK computer and
# its various components

DEBUG_FLAG= -DDEBUG -g -Wall
CFLAGS= -std=c++17 -DDEBUG -g -Wall

.SUFFIXES: .cpp .o

.cpp.o:
	g++ $(CFLAGS) -c $<


cachetest: driver.o Cache.o 
	g++ -o cachetest driver.o Cache.o 

Cache.o: Cache.h 

clean:
	/bin/rm -f cachetest *.o core