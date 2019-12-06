CXX=g++
CXXFLAGS=-std=c++17 -O3 -flto

SRC=$(wildcard *.cpp)
HDR=$(wildcard *.h)
OBJ=$(patsubst %.cpp,%.o,$(SRC))

YAGMUR=e2171783
BERK=e2171791
DISTNAME=$(YAGMUR)_$(BERK)_rasterizer.zip

rasterizer_cpp: rasterizer

rasterizer: $(OBJ)
	$(CXX) $(OBJ) $(CXXFLAGS) -o rasterizer

%.o: %.cpp $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test:
	python3 tools/test_suite.py

clean:
	rm -f rasterizer *.o

testclean:
	rm -rf test_outputs *.ppm *.png NULL tests.json

dist:
	zip $(DISTNAME) *.cpp *.h Makefile

distclean:
	rm -f $(DISTNAME)
