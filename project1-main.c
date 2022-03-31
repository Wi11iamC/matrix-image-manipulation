#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define CHANNEL_NUM 3

// imatrix struct, wraps a byte array of pixel data for an image
typedef struct imatrix
{
    int width;
    int height;

    uint8_t **r;
    uint8_t **g;
    uint8_t **b;
    struct imatrix *(*add)(struct imatrix *this, struct imatrix *m2);
    struct imatrix *(*subtract)(struct imatrix *this, struct imatrix *m2);
    struct imatrix *(*multiply)(struct imatrix *this, struct imatrix *m2);
    struct imatrix *(*scale)(struct imatrix *this, int width, int height, float alpha);
    void (*write_image_to_rgb)(struct imatrix *this);
    void (*write_rgb_to_image)(struct imatrix *m);
    struct imatrix *(*set_rgb_image)(struct imatrix *this, uint8_t *new_rgb_image, int width, int height, int channels);

    // internal image reference
    uint8_t *rgb_image;
} imatrix;

// initialize an imatrix, allocates all memory dynamically
imatrix *init_from_file(char *image_path, int *width, int *height, int *channels);
imatrix *init_from_rgb_image(uint8_t *rgb_image, int width, int height, int channels);
imatrix *init_blank_rgb_image(int width, int height, int channels);

// frees all allocated memory
void free_imatrix(imatrix *image);

int main(int argc, char *argv[])
{
    int width, height, channels;
    int width2, height2, channels2;
    int i;
    uint8_t *rgb_image;

    for (i = 1; i < argc; ++i)
        printf("%d:\t%s\n", i, argv[1]);

    // Load an image
    imatrix *M = init_from_file("images/test_image1.png", &width, &height, &channels);
    if (M != NULL)
        stbi_write_png("images/output_images/image.png", width, height, CHANNEL_NUM, M->rgb_image, width * CHANNEL_NUM);

    // Load another image
    imatrix *M2 = init_from_file("images/test_image2.png", &width2, &height2, &channels2);
    if (M2 != NULL)
        stbi_write_png("images/output_images/image2.png", width2, height2, CHANNEL_NUM, M2->rgb_image, width2 * CHANNEL_NUM);

    // resize the image to the dimensions of the first image
    uint8_t *output_pixels = (uint8_t *)malloc(width * height * CHANNEL_NUM * sizeof(uint8_t));
    stbir_resize_uint8(M2->rgb_image, width2, height2, 0,
                       output_pixels, width, height, 0,
                       CHANNEL_NUM);
    stbi_write_png("images/output_images/image3.png", width, height, CHANNEL_NUM, output_pixels, width * CHANNEL_NUM);

    // add two images
    imatrix *M2_resized = init_from_rgb_image(output_pixels, width, height, channels);
    imatrix *sum_image = M2_resized->add(M2_resized, M);
    stbi_write_png("images/output_images/sum.png", width, height, CHANNEL_NUM, sum_image->rgb_image, width * CHANNEL_NUM);
    // subtract them
    imatrix *diff_image = sum_image->subtract(sum_image, M);
    stbi_write_png("images/output_images/diff.png", width, height, CHANNEL_NUM, diff_image->rgb_image, width * CHANNEL_NUM);

    // scale then add
    float alpha = 0.4;
    M2_resized->scale(M2_resized, width, height, alpha);
    M->scale(M, width, height, (1 - alpha));
    imatrix *scaled_sum = M2_resized->add(M2_resized, M);
    stbi_write_png("images/output_images/scaled_sum.png", width, height, CHANNEL_NUM, scaled_sum->rgb_image, width * CHANNEL_NUM);

    // scaled and translate (matrix multiply)

    // OPTIONAL: Test code for matrix multiply

    // free memory
    free_imatrix(M);
    free_imatrix(M2);
    free_imatrix(M2_resized);
    free_imatrix(sum_image);
    free_imatrix(diff_image);

    return 0;
}

enum RGB
{
    RED,
    GREEN,
    BLUE
};
void init_funcptrs(imatrix *this);
imatrix *init_rgb(imatrix *this, int width, int height, int channels);

imatrix *init_from_file(char *image_path, int *width, int *height, int *channels)
{
    int i, j;
    uint8_t *rgb_image;
    rgb_image = stbi_load(image_path, width, height, channels, CHANNEL_NUM);
    imatrix *image_matrix = malloc(sizeof(imatrix));

    init_funcptrs(image_matrix);
    image_matrix->rgb_image = rgb_image;
    image_matrix = init_rgb(image_matrix, *width, *height, CHANNEL_NUM);
    image_matrix->write_image_to_rgb(image_matrix);

    return image_matrix;
}

imatrix *init_from_rgb_image(uint8_t *rgb_image, int width, int height, int channels)
{
    int i, j;

    if (rgb_image == NULL)
        return NULL;

    imatrix *image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);
    image_matrix->rgb_image = rgb_image;
    image_matrix = init_rgb(image_matrix, width, height, CHANNEL_NUM);
    image_matrix->write_image_to_rgb(image_matrix);

    return image_matrix;
}

imatrix *init_blank_rgb_image(int width, int height, int channels)
{
    int i, j;
    uint8_t *rgb_image = NULL;

    imatrix *image_matrix = malloc(sizeof(imatrix));
    init_funcptrs(image_matrix);
    image_matrix->rgb_image = (uint8_t *)malloc(sizeof(uint8_t) * width * height * CHANNEL_NUM);
    image_matrix = init_rgb(image_matrix, width, height, CHANNEL_NUM);

    return image_matrix;
}

void init_funcptrs(imatrix *this)
{
    imatrix *add(imatrix * m1, imatrix * m2);
    imatrix *subtract(imatrix * m1, imatrix * m2);
    imatrix *multiply(imatrix * m1, imatrix * m2);
    void write_image_to_rgb(imatrix * this);
    void write_rgb_to_image(imatrix * m);
    imatrix *scale(imatrix * this, int width, int height, float alpha);

    if (this == NULL)
        return;
    this->add = add;
    this->subtract = subtract;
    this->multiply = multiply;
    this->write_image_to_rgb = write_image_to_rgb;
    this->write_rgb_to_image = write_rgb_to_image;
    this->scale = scale;
}

imatrix *init_rgb(imatrix *this, int width, int height, int channels)
{
    int i, j;
    this->width = width;
    this->height = height;

    // allocate the space for the three channel matrices

    // YOUR CODE HERE
    this->r = (u_int8_t **)malloc(height * sizeof(uint8_t **));
    this->g = (u_int8_t **)malloc(height * sizeof(uint8_t **));
    this->b = (u_int8_t **)malloc(height * sizeof(uint8_t **));

    // position allocated memory across `M` pointers
    for (int row = 0; row < height; row++)
    {
        *(this->r + row) = (u_int8_t *)malloc(width * sizeof(uint8_t *));
        *(this->g + row) = (u_int8_t *)malloc(width * sizeof(uint8_t *));
        *(this->b + row) = (u_int8_t *)malloc(width * sizeof(uint8_t *));
    }

    return this;
}

imatrix *set_rgb_image(imatrix *this, uint8_t *new_rgb_image, int height, int width, int channels)
{
    free_imatrix(this);
    return init_from_rgb_image(new_rgb_image, height, width, channels);
}

/*
 *   Add two matrices together.  Creates a new imatrix object and returns a referecne to it.  Returns
 *   NULL if the matrices have different sizes.
 *
 *   @param m1 the imatrix object for the first image
 *   @param m2 the imatrix object for the second image
 *   @returns a reference to a newly allocated imatrix object that is the clipped sum of the two input images or
 *                NULL if the matrices are not the same size (same number of rows and columns).
 *            Note: This memory must be freed when you're done using it.
 */
imatrix *add(imatrix *m1, imatrix *m2)
{

    // YOUR CODE HERE
    if ((m1->height != m2->height) || (m1->width != m2->width))
        return NULL;

    imatrix *this_guy = init_blank_rgb_image(m1->width, m1->height, CHANNEL_NUM);
    int count = 1;
    for (int row = 0; row < m1->height; row++)
    {
        for (int col = 0; col < m1->width; col++)
        {
            count++;
            this_guy->r[row][col] = ((m1->r[row][col] + m2->r[row][col]) <= 255) ? (m1->r[row][col] + m2->r[row][col]) : 255;
            this_guy->g[row][col] = ((m1->g[row][col] + m2->g[row][col]) <= 255) ? (m1->g[row][col] + m2->g[row][col]) : 255;
            this_guy->b[row][col] = ((m1->b[row][col] + m2->b[row][col]) <= 255) ? (m1->b[row][col] + m2->b[row][col]) : 255;
        }
    }

    this_guy->write_rgb_to_image(this_guy);
    return this_guy;
}

/*
 *   Subtract m2 from m1.  Creates a new imatrix object and returns a referecne to it. Returns
 *   NULL if the matrices have different sizes.
 *
 *   @param m1 the imatrix object for the first image
 *   @param m2 the imatrix object for the second image
 *   @returns a reference to a newly allocated imatrix object that is the clipped sum of the two input images or
 *                NULL if the matrices are not the same size (same number of rows and columns).
 *            Note: This memory must be freed when you're done using it.
 */
imatrix *subtract(imatrix *m1, imatrix *m2)
{

    // YOUR CODE HERE
    if ((m1->height != m2->height) || (m1->width != m2->width))
        return NULL;

    imatrix *this_guy = init_blank_rgb_image(m1->width, m1->height, CHANNEL_NUM);
    for (int row = 0; row < m1->height; row++)
    {
        for (int col = 0; col < m1->width; col++)
        {
            this_guy->r[row][col] = ((m1->r[row][col] - m2->r[row][col]) >= 0) ? (m1->r[row][col] - m2->r[row][col]) : 0;
            this_guy->g[row][col] = ((m1->g[row][col] - m2->g[row][col]) >= 0) ? (m1->g[row][col] - m2->g[row][col]) : 0;
            this_guy->b[row][col] = ((m1->b[row][col] - m2->b[row][col]) >= 0) ? (m1->b[row][col] - m2->b[row][col]) : 0;
        }
    }

    this_guy->write_rgb_to_image(this_guy);
    return this_guy;
}

/*
 *   Matrix multiplication.  Multiplies m1*m2 using matrix-matrix multiply.  Returns a new imatrix object with
 *   the output.  If m1 is a (m, n) matrix, then m2 must be a (n, k) matrix for any value of k >=1.
 *
 *   @param m1 the left matrix.
 *   @param m2 the right matrix.
 *   @returns a new matrix with the result or NULL of the dimensions do not match properly for matrix multiplications.
 *               Note: This memory must be freed after use.
 */
imatrix *multiply(imatrix *m1, imatrix *m2)
{

    // YOUR CODE HERE
    if ((m1->height != m2->height) || (m1->width != m2->width))
        return NULL;
    imatrix *M = init_blank_rgb_image(m1->width, m1->height, CHANNEL_NUM);
    for (int row = 0; row < m1->height; row++)
    {
        for (int col = 0; col < m1->width; col++)
        {
            M->r[row][col] = ((m1->r[row][col] * m2->r[row][col]) <= 255) ? (m1->r[row][col] + m2->r[row][col]) : 255;
            M->g[row][col] = ((m1->g[row][col] * m2->g[row][col]) <= 255) ? (m1->g[row][col] + m2->g[row][col]) : 255;
            M->b[row][col] = ((m1->b[row][col] * m2->b[row][col]) <= 255) ? (m1->b[row][col] + m2->b[row][col]) : 255;
        }
    }
    free_imatrix(M);
    return M;
}

/*
 *   Scales all the pixel values by alpha for all alpha values between 0.0 and 1.0.  If the values are invalid, the references
 *   to the input image is returned without modification.
 *
 *   @param this input matrix.
 *   @returns this with all the pixel values scaled by alpha.
 */
imatrix *scale(imatrix *this, int width, int height, float alpha)
{

    // YOUR CODE HERE
    if (alpha > 1 || alpha < 0)
        return this;

    for (int row = 0; row < this->height; row++)
    {
        for (int col = 0; col < this->width; col++)
        {
            this->r[row][col] = ((this->r[row][col] * alpha));
            this->g[row][col] = ((this->g[row][col] * alpha));
            this->b[row][col] = ((this->b[row][col] * alpha));
        }
    }

    return this;
}

void write_rgb_to_image(imatrix *m)
{
    int i, j, height, width;
    height = m->height;
    width = m->width;

    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            *(m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + RED) = m->r[i][j];
            *(m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + GREEN) = m->g[i][j];
            *(m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + BLUE) = m->b[i][j];
        }
    }
}

void write_image_to_rgb(imatrix *m)
{
    int i, j, height, width;
    height = m->height;
    width = m->width;

    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            m->r[i][j] = *(m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + RED);
            m->g[i][j] = *(m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + GREEN);
            m->b[i][j] = *(m->rgb_image + (i * (CHANNEL_NUM * width) + (CHANNEL_NUM * j)) + BLUE);
        }
    }
}

void free_imatrix(imatrix *image_matrix)
{
    int i;
    if (image_matrix == NULL)
        return;

    for (i = 0; i < image_matrix->height; ++i)
    {
        free(*(image_matrix->r + i));
        free(*(image_matrix->g + i));
        free(*(image_matrix->b + i));
    }
    free(image_matrix->r);
    free(image_matrix->g);
    free(image_matrix->b);

    stbi_image_free(image_matrix->rgb_image);
}