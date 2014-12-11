all: Robot

Robot: Robot.o Create.o
	g++ -std=c++11 `pkg-config --cflags opencv` Robot.o Create.o -o Robot `pkg-config --libs opencv` -L ~/local/lib -rdynamic -lplayerc -lm -lz -lplayerinterface -lplayerwkb -lplayercommon -Wl,-rpath,~/local/lib
	rm *.o

Robot.o: Robot.cc Create.h
	g++ -std=c++11 -o Robot.o -I ~/local/include/player-3.0 -c Robot.cc

Create.o: Create.cc Create.h
	g++ -std=c++11 -o Create.o -I ~/local/include/player-3.0 -c Create.cc

clean:
	rm -f *.o Robot
.PHONY: clean
