all: compile link

compile:
	g++ -I src/include -c main.cpp src/include/functions.cpp
link:
	g++ main.o functions.o -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system