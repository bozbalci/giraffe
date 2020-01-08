CXX=g++
CXXFLAGS=-std=c++17 -Wall -O3

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS=-lGL -lglfw -lGLEW -ljpeg
endif
ifeq ($(UNAME_S),Darwin)
	LDFLAGS=-framework OpenGL -lglfw -ljpeg
	CXXFLAGS += -DGL_SILENCE_DEPRECATION
endif
UBUNTU_S := $(shell $(CXX) --version | grep -i ubuntu >/dev/null; echo $$?)
ifeq ($(UBUNTU_S),0)
    LDFLAGS=-lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -ljpeg
endif

all: hw3_flat

hw3_flat: flat.cpp
	$(CXX) flat.cpp -o hw3_flat $(CXXFLAGS) $(LDFLAGS)

.PHONY += codegen
codegen:
	cd code_generation; python3 ./generate.py flat.cpp > ../flat.cpp
#	cd code_generation; python3 ./generate.py sphere.cpp > ../sphere.cpp

codegen-build: codegen hw3_flat

clean:
	rm -f *.o hw3_flat
