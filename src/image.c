#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "image.h"
#include <string.h>
#include <stdio.h>

Image_Data *get_imagePixels(char imagePath[])
{

    int height, width, original_channels;

    unsigned char *pixels = stbi_load(
        imagePath,
        &width,
        &height,
        &original_channels,
        4);

    if (pixels == NULL)
    {
        fprintf(stderr, "Failed to load image: %s\n",
                stbi_failure_reason());
    }

    Image_Data *image_data = malloc(sizeof(Image_Data));

    image_data->channels = 4;
    image_data->height = height;
    image_data->width = width;
    image_data->pixels = pixels;

    return image_data;
}

// transforming the pixels array in which has the black & white image array
// TBW_pixels = transform Black & White pixels

Image_Data *TBW_pixels(Image_Data *image)
{
    Image_Data *BW_image = malloc(sizeof(Image_Data));

    if (BW_image == NULL)
    {
        fprintf(stderr, "TBW_pixels(): memory allocation failed");
    }

    BW_image->height = image->height;
    BW_image->width = image->width;
    BW_image->channels = image->channels;

    unsigned char *pixels = image->pixels;
    unsigned char *pixel = malloc(sizeof(unsigned char) * 4);
    unsigned char *BW_pixels = malloc(sizeof(unsigned char) * 4 * image->height * image->width);
    unsigned char *BW_pixel = malloc(sizeof(unsigned char) * 4);

    if (pixel == NULL)
    {
        fprintf(stderr, "TBW_pixels(): memory allocation failed");
    }
    if (BW_pixels == NULL)
    {
        fprintf(stderr, "TBW_pixels(): memory allocation failed");
    }
    if (BW_pixel == NULL)
    {
        fprintf(stderr, "TBW_pixels(): memory allocation failed");
    }
    

    for (size_t i = 0; i < (size_t)image->height * (size_t)image->width * 4; i = i + 4)
    {
        memcpy(pixel, &pixels[i], 4);

        unsigned char red = pixel[0];
        unsigned char green = pixel[1];
        unsigned char blue = pixel[2];

        unsigned char gray = (unsigned char)(0.299 * red + 0.587 * green + 0.114 * blue);

        *(BW_pixel + 0) = gray;
        *(BW_pixel + 1) = gray;
        *(BW_pixel + 2) = gray;
        *(BW_pixel + 3) = pixel[3];

        memcpy(&BW_pixels[i], BW_pixel, 4);
    }

    BW_image->pixels = BW_pixels;

    return BW_image;
}