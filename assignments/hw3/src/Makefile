CXX=g++
CXXFLAGS=-std=c++17 -Wall -g

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS=-lGL -lglfw -lGLEW
endif
ifeq ($(UNAME_S),Darwin)
	LDFLAGS=-framework OpenGL -lglfw
	CXXFLAGS += -DAPPLE -DGL_SILENCE_DEPRECATION
endif
UBUNTU_S := $(shell $(CXX) --version | grep -i ubuntu >/dev/null; echo $$?)
ifeq ($(UBUNTU_S),0)
    LDFLAGS=-lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl
endif

all: hw3_flat

hw3_flat: flat.cpp util.cpp Giraffe.h
	$(CXX) flat.cpp util.cpp -o hw3_flat $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f *.o hw3_flat
