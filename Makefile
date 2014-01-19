all: main

main:
	g++ main.cpp -o main -lSDL2

clean:
	rm main

