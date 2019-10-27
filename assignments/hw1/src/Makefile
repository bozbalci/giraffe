src = *.cpp

all:
	g++ $(src) -std=c++17 -O3 -o raytracer -pthread -flto -fno-strict-aliasing

clean:
	rm -f raytracer *.ppm
