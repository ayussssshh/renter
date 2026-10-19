#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "image.h"
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <math.h>
#include "stb_image_resize2.h"
#include "stb_image_write.h"






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




int *get_IRSDimensions(struct winsize ws, Image_Data *image){

    int *new_dimension = malloc(sizeof(int) * 2); // new_dimension[0] is width, new_dimension[1] is height

    if (new_dimension == NULL)
    {
        fprintf(stderr, "get_IRSDimensions(): memory allocation failed");

        return NULL;
    }
    

    float s, width_scale, height_scale;

    width_scale = (float)ws.ws_xpixel / image->width;
    height_scale = (float)ws.ws_ypixel / image->height;

    if (width_scale < height_scale)
    {
        s = width_scale;
    }
    else
    {
        s = height_scale;
    }

    new_dimension[0] = s * image->width;
    new_dimension[1] = s * image->height; 
    
    return new_dimension;
}





Image_Data *resize_image(const Image_Data *original, int available_w, int available_h){

    if (!original || !original->pixels ||
        original->width <= 0 || original->height <= 0 ||
        available_w <= 0 || available_w <= 0 ||
        original->channels < 1 || original->channels > 4) {
        return NULL;
    }

    float W_ratio, H_ratio, s;
    int new_width, new_height;

    W_ratio = (float)available_w/original->width;
    H_ratio = (float)available_h/original->height;

    if (W_ratio<H_ratio)
    {
        s = W_ratio;
    }
    else
    {
        s = H_ratio;
    }
    
    new_width = s * original->width;
    new_height = s * original->height;

    size_t pixels = (size_t)new_width * (size_t)new_height;

    if (pixels > SIZE_MAX / (size_t)original->channels)
        return NULL;

    size_t bytes = pixels * (size_t)original->channels;

    Image_Data *result = malloc(sizeof(*result));
    if (!result)
        return NULL;

    result->pixels = malloc(bytes);
    if (!result->pixels) {
        free(result);
        return NULL;
    }

    result->width = new_width;
    result->height = new_height;
    result->channels = original->channels;

    stbir_pixel_layout layout;

    switch (original->channels) {
        case 1: layout = STBIR_1CHANNEL; break;
        case 2: layout = STBIR_2CHANNEL; break;
        case 3: layout = STBIR_RGB; break;
        case 4: layout = STBIR_RGBA; break;
        default:
            free(result->pixels);
            free(result);
            return NULL;
    }

    unsigned char *output = stbir_resize_uint8_srgb(
        original->pixels,
        original->width,
        original->height,
        0,
        result->pixels,
        new_width,
        new_height,
        0,
        layout
    );

    if (!output) {
        free(result->pixels);
        free(result);
        return NULL;
    }

    return result;
}


int save_image(const Image_Data *image, const char *filename){
        if (image == NULL || image->pixels == NULL)
            return 0;

        int stride = image->width * image->channels;

        return stbi_write_png(
            filename,
            image->width,
            image->height,
            image->channels,
            image->pixels,
            stride);
}






unsigned char *get_grayScale_img(Image_Data *image)
{
    if (image == NULL || image->pixels == NULL ||
        image->width <= 0 || image->height <= 0 ||
        (image->channels != 3 && image->channels != 4))
    {
        fprintf(stderr, "get_grayScale_img(): invalid image\n");
        return NULL;
    }

    size_t total_pixels =
        (size_t)image->width * (size_t)image->height;

    unsigned char *grayScale_img = malloc(total_pixels);

    if (grayScale_img == NULL)
    {
        fprintf(stderr, "get_grayScale_img(): allocation failed\n");
        return NULL;
    }

    for (size_t i = 0; i < total_pixels; i++)
    {
        size_t src = i * (size_t)image->channels;

        unsigned char red   = image->pixels[src];
        unsigned char green = image->pixels[src + 1];
        unsigned char blue  = image->pixels[src + 2];

        grayScale_img[i] = (unsigned char)(
            0.299 * red +
            0.587 * green +
            0.114 * blue
        );
    }

    return grayScale_img;
}

