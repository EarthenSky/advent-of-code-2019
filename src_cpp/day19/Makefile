OPT = -g

all: day19

intcode:
	g++ -Wall -c $(OPT) intcode.cpp -I/usr/local/boost_1_72_0 

droneControlSystem:
	g++ -Wall -c $(OPT) droneControlSystem.cpp -I/usr/local/boost_1_72_0 

day19: intcode droneControlSystem
	g++ -Wall -c $(OPT) day19.cpp -I/usr/local/boost_1_72_0
	g++ -Wall -o day19.out day19.o droneControlSystem.o intcode.o

clean:
	rm *.o *.out
