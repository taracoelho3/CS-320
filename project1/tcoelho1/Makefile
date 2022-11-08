all:	predictors

predictors:	trace_read.o
	g++ -g -Wall -std=c++11 trace_read.o -o predictors 

reace_read.o:	trace_read.cpp
	g++ -c trace_read.cpp 

clean:
	rm *.o predictors 