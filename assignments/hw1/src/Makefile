src = *.cpp

all:
	g++ $(src) -std=c++17 -O3 -o raytracer -pthread -flto

clean:
	rm -f raytracer *.ppm

dist:
	mkdir submission
	cp -r members.txt Makefile bonus_scene *.cpp *.h submission/
	zip -r submission.zip submission
	rm -rf submission

distclean:
	rm -rf submission submission.zip
