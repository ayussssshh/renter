// #include <stdlib.h>
// #include <stddef.h>

// // 

// char *get_framebuffer(int height, int width,
//                       unsigned char *grayScale_img,
//                       int grayScale_img_width)
// {
//     static const char ASCII_RAMP[] = "@%#*+=-:. ";
//     const size_t ramp_len = sizeof(ASCII_RAMP) - 1;

//     if (grayScale_img == NULL ||
//         height <= 0 ||
//         width <= 0 ||
//         grayScale_img_width <= 0)
//     {
//         return NULL;
//     }

//     size_t framebuffer_size = (size_t)height * ((size_t)width + 1);

//     char *framebuffer = malloc(framebuffer_size);

//     if (framebuffer == NULL)
//         return NULL;

//     size_t pixel_index = 0;
//     size_t buffer_index = 0;

//     for (int y = 0; y < height; y++)
//     {
//         for (int x = 0; x < width; x++)
//         {
//             unsigned char gray = grayScale_img[pixel_index++];

//             size_t index = (size_t)gray * (ramp_len - 1) / 255;

//             framebuffer[buffer_index++] = ASCII_RAMP[index];
//         }

//         framebuffer[buffer_index++] = '\n';
//     }

//     framebuffer[buffer_index] = '\0';

//     return framebuffer;
// }



#include <stdlib.h>
#include <stddef.h>

char *get_framebuffer(int height, int width,
                      unsigned char *grayScale_img,
                      int grayScale_img_width)
{
    static const char ASCII_RAMP[] = "@%#*+=-:. ";
    const size_t ramp_len = sizeof(ASCII_RAMP) - 1;

    if (grayScale_img == NULL ||
        height <= 0 || width <= 0 ||
        grayScale_img_width < width)
    {
        return NULL;
    }

    size_t framebuffer_size =
        (size_t)height * ((size_t)width + 1) + 1;

    char *framebuffer = malloc(framebuffer_size);
    if (framebuffer == NULL)
        return NULL;

    size_t buffer_index = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            size_t pixel_index =
                (size_t)y * (size_t)grayScale_img_width
                + (size_t)x;

            unsigned char gray = grayScale_img[pixel_index];

            size_t ramp_index =
                (size_t)gray * (ramp_len - 1) / 255;

            framebuffer[buffer_index++] = ASCII_RAMP[ramp_index];
        }

        framebuffer[buffer_index++] = '\n';
    }

    framebuffer[buffer_index] = '\0';

    return framebuffer;
}
