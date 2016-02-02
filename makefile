CC=g++
LIBS= -lopencv_core -lopencv_imgproc -lopencv_highgui
FLAGS= -std=c++11 -Wall -pedantic -DDEBUG -g
PROJECT_NAME=local-texture-descriptors.out

objects=main.o textureDescriptor.o

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
