# Image Processing Project

## Build Setup

```bash
# build first before running the program
$ ./project1-main
```

This project is an image processing library that can process images using matrix operations. All images can be represented as a matrix of single-byte values, where a pixel at matrix location (i, j) is represented by a number between 0 and 255. A color imageconsists of three channels: a red channel (R), a green channel (G), and a blue channel (B), and the corresponding matrix for an image includes three matrices, each with the same number of rows (height) and columns (width), one for each of the three channels. Together,these are passed to an image-writing library for outputting to a file and displaying.

## Special Directories

You can create the following extra directories, some of which have special behaviors. Only `pages` is required; you can delete them if you don't want to use their functionality.

### `output_images`

The output_images directory contains your image result of runnning the program.
