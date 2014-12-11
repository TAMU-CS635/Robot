all: Robot

Robot: Robot.o Create.o SearchBehavior.o
	g++ `pkg-config --cflags opencv` Robot.o Create.o SearchBehavior.o -o Robot `pkg-config --libs opencv` -L ~/local/lib -rdynamic -lplayerc -lm -lz -lplayerinterface -lplayerwkb -lplayercommon -Wl,-rpath,~/local/lib
	rm *.o

Robot.o: Robot.cc Create.h
	g++ -o Robot.o -I /usr/include/player-3.0 -c Robot.cc

Create.o: Create.cc Create.h
	g++ -o Create.o -I /usr/include/player-3.0 -c Create.cc

SearchBehavior.o: SearchBehavior.cc SearchBehavior.h
	g++ -o SearchBehavior.o -I /usr/include/player-3.0 -c SearchBehavior.cc

clean:
	rm -f *.o Robot
.PHONY: clean
