src = *.cpp

all:
	g++ $(src) -std=c++17 -O3 -o raytracer -pthread -flto -march=native

clean:
	rm -f raytracer *.ppm
