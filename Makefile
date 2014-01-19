all: main

main: main.cpp
	g++ main.cpp -o main -lSDL2 -lGLEW -lGL

clean:
	rm main

