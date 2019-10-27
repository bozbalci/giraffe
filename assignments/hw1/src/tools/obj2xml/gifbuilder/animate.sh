#!/usr/bin/env bash

mogrify -format jpg *.ppm
convert -delay 1 -loop 0 *.jpg animation.gif
rm *.jpg
