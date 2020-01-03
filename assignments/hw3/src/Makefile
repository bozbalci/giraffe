CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra
LDFLAGS=-lGL -ljpeg

SRC=$(wildcard *.cpp)
HDR=$(wildcard *.h)
OBJ=$(patsubst %.cpp,%.o,$(SRC))

all: hw3_flat hw3_sphere

hw3_flat: flat.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) flat.cpp -o hw3_flat

hw3_sphere: sphere.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) sphere.cpp -o hw3_sphere

clean:
	rm -f *.o hw3_flat hw3_sphere
