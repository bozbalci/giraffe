src = *.cpp

all:
	g++ $(src) -std=c++17 -g -O0 -o raytracer -pthread -flto -fno-strict-aliasing

clean:
	rm -f raytracer *.ppm
