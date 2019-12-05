CXX=g++
CXXFLAGS=-std=c++17 -O3 -flto

SRC=$(wildcard *.cpp)
HDR=$(wildcard *.h)
OBJ=$(patsubst %.cpp,%.o,$(SRC))

rasterizer: $(OBJ)
	$(CXX) $(OBJ) $(CXXFLAGS) -o rasterizer

%.o: %.cpp $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test:
	python3 tools/test_suite.py

clean:
	rm -f rasterizer *.o

testclean:
	rm -f *.ppm *.png NULL tests.json
