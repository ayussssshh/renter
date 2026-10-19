#include <stdlib.h>
#include <stddef.h>

char *get_framebuffer(int height, int width,
                      unsigned char *grayScale_img)
{
    static const char ASCII_RAMP[] = "@%#*+=-:. ";

    const size_t ramp_len = sizeof(ASCII_RAMP) - 1;

    if (grayScale_img == NULL || height <= 0 || width <= 0)
    {
        return NULL;
    }

    char *framebuffer = malloc((size_t)height * (width + 1));
    if (framebuffer == NULL)
        return NULL;

    size_t pos = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            size_t i = (size_t)y * width + x;

            unsigned char gray = grayScale_img[i];
            size_t index = (size_t)gray * (ramp_len - 1) / 255;

            framebuffer[pos++] = ASCII_RAMP[index];
        }

        framebuffer[pos++] = '\n';
    }

    return framebuffer;
}