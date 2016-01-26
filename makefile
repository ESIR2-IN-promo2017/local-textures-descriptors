CC=g++
LIBS= -lopencv_core -lopencv_highgui
FLAGS= -std=c++14 -Wall -pedantic
PROJECT_NAME=local-texture-descriptors.out

objects=main.o

all: $(objects)
	$(CC) -o $(PROJECT_NAME) $^ $(LIBS)

%.o: %.cpp %.h
	$(CC) -c $< $(FLAGS)

main.o: main.cpp
	$(CC) -c $< $(FLAGS)

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf $(PROJECT_NAME)
