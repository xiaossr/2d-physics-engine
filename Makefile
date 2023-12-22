all: build run

build: compile link

compile:
	g++ -O2 -c main.cpp 

link:
	g++ -O2 main.o -o main -lsfml-graphics -lsfml-window -lsfml-system 

run:
	main.exe