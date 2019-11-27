CXX=g++
CXXFLAGS=-std=c++17 -O3

SRC=$(wildcard *.cpp)
HDR=$(wildcard *.h)
OBJ=$(patsubst %.cpp,%.o,$(SRC))

rasterizer: $(OBJ)
	$(CXX) $(OBJ) $(CXXFLAGS) -o rasterizer

%.o: %.cpp $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f rasterizer *.o
