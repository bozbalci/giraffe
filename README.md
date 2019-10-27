# Giraffe

Coursework for *CENG447 - Introduction to Computer Graphics (Fall 2019),* by [Yağmur Oymak](https://twitter.com/OymakYagmur) and [Berk Özbalcı](https://unsystem.dev).

## Assignment 1

|Specification|Source|
|--|--|
|[hw1 spec](https://github.com/bozbalci/giraffe/tree/master/assignments/hw1/hw1.pdf)|[hw1](https://github.com/bozbalci/giraffe/tree/master/assignments/hw1/src)|

## Example render

![Dragon](pictures/dragon_hires.png)

<small>Dragon, rendered at 8000x8000 on a 72-core machine in 25 seconds, converted to PNG and downscaled to 1600x1600.</small>

### Performance

|Optimizations|Test file|Time (s)|Speedup|
|--|--|--|--|
|Baseline|`past_examples/dragon_lowres.xml`|495.57|-|
|Threads|`past_examples/dragon_lowres.xml`|107.69|4.59x|
|Better optzn. flags|`past_examples/dragon_lowres.xml`|77.24|6.41x|
|Bounding boxes|`past_examples/dragon_lowres.xml`|45.72|10.8x|
|BVH|`past_examples_dragon_lowres.xml`|1.15|430.9x|

## clang-format
Run

    ./install_hooks.sh

on the project root directory in order to install the `pre-commit` hook that runs `clang-format` on every source file in the repository. Requires `clang-format` and `bash >= 4.0`.

Edit the `.clang-format` to suit your taste.
