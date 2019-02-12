all: fpp

fpp: fpp.cpp
	g++ -lncurses fpp.cpp -o fpp

clean:
	rm fpp
