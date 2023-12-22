all: build run

static: stbuild run

build: compile link

compile:
	g++ -O2 -c main.cpp

link:
	g++ -O2 main.o -o main -lsfml-graphics-s -lsfml-window-s -lsfml-system-s

run:
	main.exe

stbuild: 
	g++ -O2 -c main.cpp  -static
	g++ -O2 main.o -o main  -DSFML_STATIC -lsfml-graphics-s -lsfml-window-s -lsfml-system-s  -lopengl32 -lfreetype -lwinmm -lgdi32