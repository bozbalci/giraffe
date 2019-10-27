#!/usr/bin/env bash

# $1: file to watch changes for (the .xml)
# $2: the output ppm file
# $3: the name of the output png
nodemon -x "./reopen_image.sh $1 $2 $3" $1
