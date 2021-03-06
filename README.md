# Image Processing Project

This project is an image processing library that can process images using matrix operations. All images can be represented as a matrix of single-byte values, where a pixel at matrix location (i, j) is represented by a number between 0 and 255. A color image consists of three channels: a red channel (R), a green channel (G), and a blue channel (B), and the corresponding matrix for an image includes three matrices, each with the same number of rows (height) and columns (width), one for each of the three channels. Together, these are passed to an image-writing library for outputting to a file and displaying.


## Build Setup

Build first before running the program:

```sh
$ ./project1-main
```

## Special Directories

You can create the following extra directories, some of which have special behaviors.

### `output_images`

The output_images directory contains the image result of runnning the program with test_iamge1.png and test_image2.png.

### `output_images2`

The output_images directory contains the image result of runnning the program with test_iamge3.png and test_image4.png.
