all:	cache-sim

cache-sim:	cache.o
	g++ -g -Wall -std=c++11 cache.o -o cache-sim 

cache.o:	cache.cpp
	g++ -c cache.cpp 

clean:
	rm *.o cache-sim 