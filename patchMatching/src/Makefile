CC=g++
LIBS= -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib
FLAGS= -std=c++11 -Wall -pedantic -DDEBUG -g -O3
PROJECT_NAME=patchmatch.out

objects=AbstractPatchMatch.o PatchMatchDescriptorStatic.o PatchMatchDescriptor.o PatchMatchLab.o PatchMatchY.o main.o descriptor.o textureDescriptor.o

all: $(objects)
	$(CC) -o $(PROJECT_NAME) $^ $(LIBS)

%.o: %.cpp %.h
	$(CC) -c $< $(FLAGS)

main.o: main.cpp
	$(CC) -c $< $(FLAGS)

descriptor.o: ../../descriptor.cpp
	$(CC) -c $< $(FLAGS)

textureDescriptor.o: ../../textureDescriptor.cpp
	$(CC) -c $< $(FLAGS)

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf $(PROJECT_NAME)
