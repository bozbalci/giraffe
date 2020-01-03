CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS=-lGL -lglfw -lGLEW
endif
ifeq ($(UNAME_S),Darwin)
	LDFLAGS=-framework OpenGL
	CXXFLAGS += -DNO_GLEW
endif

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
