CC=g++
LIBS= -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_contrib
FLAGS= -std=c++14 -Wall -pedantic -DDEBUG -g -Og
PROJECT_NAME=local-texture-descriptors.out

objects=main.o textureDescriptor.o descriptor.o

.PHONY: all
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(objects)
	$(CC) -o $(PROJECT_NAME) $^ $(LIBS)

%.o: %.cpp %.h
	$(CC) -c $< $(FLAGS)

main.o: main.cpp
	$(CC) -c $< $(FLAGS)

.PHONY: clean gdb valgrind
.NOTPARALLEL: clean gdb valgrind

clean:
	rm -rf *.o
	rm -rf $(PROJECT_NAME)

gdb: all
	gdb ./$(PROJECT_NAME)  2>&1 | tee logs/gdb-`date -Ins`.log

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(PROJECT_NAME) img/damier.png 2>&1 | tee logs/valgrind-`date -Ins`.log
