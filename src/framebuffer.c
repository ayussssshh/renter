#include <stdlib.h>
#include <stdint.h>

char *get_framebuffer(int height, int width,
                      unsigned char *grayScale_img,
                      int grayScale_img_width)
{
    static const char ASCII_RAMP[] = ".:-=+%#@ ";
    const size_t ramp_len = sizeof(ASCII_RAMP) - 1;

    if (grayScale_img == NULL ||
        height <= 0 || width <= 0 ||
        grayScale_img_width < width)
    {
        return NULL;
    }

    size_t h = (size_t)height;
    size_t w = (size_t)width;

    // Each grayscale pixel produces TWO characters.
    if (w > (SIZE_MAX - 1) / 2)
        return NULL;

    size_t row_size = (w * 2) + 1; // Two chars per pixel + newline

    if (h > (SIZE_MAX - 1) / row_size)
        return NULL;

    size_t framebuffer_size = h * row_size + 1;

    char *framebuffer = malloc(framebuffer_size);
    if (framebuffer == NULL)
        return NULL;

    size_t buffer_index = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            size_t pixel_index =
                (size_t)y * (size_t)grayScale_img_width + (size_t)x;

            unsigned char gray = grayScale_img[pixel_index];

            size_t ramp_index =
                (size_t)gray * (ramp_len - 1) / 255;

            char pixel_char = ASCII_RAMP[ramp_index];

            // One grayscale value -> two character cells
            framebuffer[buffer_index++] = pixel_char;
            framebuffer[buffer_index++] = pixel_char;
        }

        framebuffer[buffer_index++] = '\n';
    }

    framebuffer[buffer_index] = '\0';

    return framebuffer;
}

