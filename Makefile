all: main

main: main.cpp
	g++ main.cpp -o main -lSDL2

clean:
	rm main

