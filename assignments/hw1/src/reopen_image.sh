#!/usr/bin/env bash

# Example usage:
#    nodemon -x "./watcher.sh past_examples/dragon_lowres.xml dragon_lowres.ppm dragon_lowres.png" past_examples/dragon_lowres.xml

./raytracer "$1"
pnmtopng "$2" > "$3"
open "$3"
