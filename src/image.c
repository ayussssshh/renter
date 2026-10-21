#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "image.h"
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <math.h>
#include <stdint.h>
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





// unsigned char *AM_grayscale(unsigned char *grayscale_img, int grayscale_w, int grayscale_h, int terminal_h, int terminal_w){

//     size_t tw = (size_t)terminal_w;
//     size_t th = (size_t)terminal_h;
//     size_t gw = (size_t)grayscale_w;
//     size_t gh = (size_t)grayscale_h;

//     unsigned char *AM_grayscale =
//         malloc(sizeof(unsigned char) * tw * th);

//     if (AM_grayscale == NULL)
//     {
//         fprintf(stderr, "AM_grayscale(): allocation failed\n");
//         return NULL;
//     }

//     size_t left_margin = (tw - gw) / 2;
//     size_t right_margin = tw - gw - left_margin;

//     for (size_t i = 0; i < tw * th; i++)
//     {
//         AM_grayscale[i] = 0;

//         if (i % tw == (tw - gw))
//         {
//             i += (tw - gw);
//         }
//         else
//         {
//                 i++;
//         }
//     }

//     for (size_t i = 0; i < tw * th; i++)
//     {
//         if(i % tw < (tw - gw)){
//             i++;
//         }
//         else{
//             AM_grayscale[i] = grayscale_img[i];
//         }
//     }
    
//     return AM_grayscale;
// }




unsigned char *AM_grayscale(
    unsigned char *grayscale_img,
    int grayscale_w,
    int grayscale_h,
    int terminal_h,
    int terminal_w
) {
    if (grayscale_img == NULL ||
        grayscale_w <= 0 || grayscale_h <= 0 ||
        terminal_w <= 0 || terminal_h <= 0) {
        fprintf(stderr, "AM_grayscale(): invalid arguments\n");
        return NULL;
    }

    size_t tw = (size_t)terminal_w;
    size_t th = (size_t)terminal_h;
    size_t gw = (size_t)grayscale_w;
    size_t gh = (size_t)grayscale_h;

    unsigned char *AM_grayscale = malloc(sizeof(unsigned char) * tw * th);

    if (AM_grayscale == NULL) {
        fprintf(stderr, "AM_grayscale(): allocation failed\n");
        return NULL;
    }

    // Copy only the image area that fits inside the terminal.
    size_t copy_w = (gw < tw) ? gw : tw;
    size_t copy_h = (gh < th) ? gh : th;

    // Align the image to the right.
    size_t left_margin = tw - copy_w;

    for (size_t y = 0; y < copy_h; y++) {
        for (size_t x = 0; x < copy_w; x++) {
            size_t src_index = y * gw + x;
            size_t dst_index = y * tw + left_margin + x;

            AM_grayscale[dst_index] = grayscale_img[src_index];
        }
    }

    return AM_grayscale;
}





// Image_Data *get_downscale_img(Image_Data *src_image, int available_w, int available_h){

//     // check the all input of this function

//     float W_ratio, H_ratio, s;
//     int new_width, new_height;

//     W_ratio = (float)available_w/src_image->width;
//     H_ratio = (float)available_h/src_image->height;

//     if (W_ratio<H_ratio)
//     {
//         s = W_ratio;
//     }
//     else
//     {
//         s = H_ratio;
//     }
    
//     new_width = s * src_image->width;
//     new_height = s * src_image->height;

//     Image_Data *downscaled_img = malloc(sizeof(Image_Data));

//     if (downscaled_img == NULL)
//     {
//         fprintf(stderr, "get_downscale_img(): allocation failed\n");
//         return NULL;
//     }

//     downscaled_img->pixels = malloc(sizeof(unsigned char) * new_height * new_width);

//     if (downscaled_img->pixels == NULL)
//     {
//         fprintf(stderr, "get_downscale_img(): pixel allocation failed\n");
//         free(downscaled_img);
//         return NULL;
//     }

//     size_t scale_factor_y = (size_t)src_image->height/(size_t)new_height;
//     size_t scale_factor_x = (size_t)src_image->width/(size_t)new_width;
    
//     for (size_t downscaled_pixel_y = 0; downscaled_pixel_y < (size_t)new_height; downscaled_pixel_y++)
//     {

//         for (size_t downscaled_pixel_x = 0; downscaled_pixel_x < (size_t)new_width; downscaled_pixel_y++)
//         {
        
//             size_t src_block_x = scale_factor_x * downscaled_pixel_x;
//             size_t src_block_y = scale_factor_y * downscaled_pixel_y;

//             size_t src_block_start_index = src_block_y * src_image->width + src_block_x;

//             uint64_t src_block_sum = 0;

//             for (size_t src_block_y = 0; src_block_y < scale_factor_y; src_block_y++)
//             {
                
//                 for (size_t src_block_x = 0; src_block_x < scale_factor_x; src_block_x++)
//                 {
                    
//                     size_t src_block_index = src_block_start_index + src_block_y*(size_t)src_image->width + src_block_x;

//                     src_block_sum += src_image->pixels[src_block_index];

//                 }

//             }


//             unsigned char downscaled_pixel_value = src_block_sum/(scale_factor_x * scale_factor_y);

//             size_t downscaled_pixel_index = downscaled_pixel_y * new_width + downscaled_pixel_x;

//             downscaled_img->pixels[downscaled_pixel_index] = downscaled_pixel_value;
            
//         }

        
//     }

//     downscaled_img->height = new_height;
//     downscaled_img->width = new_width;
//     downscaled_img->channels = src_image->channels;
    
//     return downscaled_img;

// }




// ai code

Image_Data *get_downscale_img(
    Image_Data *src_image,
    int available_w,
    int available_h
) {
    // 1. Validate input.
    if (src_image == NULL ||
        src_image->pixels == NULL ||
        src_image->width <= 0 ||
        src_image->height <= 0 ||
        available_w <= 0 ||
        available_h <= 0) {
        return NULL;
    }

    // 2. Calculate a scale that fits inside the available area.
    double width_ratio =
        (double)available_w / src_image->width;

    double height_ratio =
        (double)available_h / src_image->height;

    double scale = width_ratio < height_ratio
                 ? width_ratio
                 : height_ratio;

    // Do not enlarge the source image.
    if (scale > 1.0) {
        scale = 1.0;
    }

    int new_width = (int)(src_image->width * scale);
    int new_height = (int)(src_image->height * scale);

    // Keep dimensions at least one pixel.
    if (new_width < 1) new_width = 1;
    if (new_height < 1) new_height = 1;

    size_t output_width = (size_t)new_width;
    size_t output_height = (size_t)new_height;

    // 3. Check allocation size for overflow.
    if (output_width > SIZE_MAX / output_height) {
        return NULL;
    }

    size_t output_pixel_count = output_width * output_height;

    // 4. Allocate the output image.
    Image_Data *downscaled_img = malloc(sizeof(*downscaled_img));

    if (downscaled_img == NULL) {
        fprintf(stderr, "get_downscale_img(): allocation failed\n");
        return NULL;
    }

    downscaled_img->pixels =
        malloc(output_pixel_count * sizeof(*downscaled_img->pixels));

    if (downscaled_img->pixels == NULL) {
        fprintf(stderr, "get_downscale_img(): pixel allocation failed\n");
        free(downscaled_img);
        return NULL;
    }

    downscaled_img->width = new_width;
    downscaled_img->height = new_height;
    downscaled_img->channels = src_image->channels;


    // 5. Visit every output pixel.
    for (size_t output_y = 0; output_y < output_height; output_y++) {
        for (size_t output_x = 0; output_x < output_width; output_x++) {

            // Find the source rectangle assigned to this output pixel.
            size_t source_x_start =
                output_x * (size_t)src_image->width / output_width;

            size_t source_x_end =
                (output_x + 1) * (size_t)src_image->width / output_width;

            size_t source_y_start =
                output_y * (size_t)src_image->height / output_height;

            size_t source_y_end =
                (output_y + 1) * (size_t)src_image->height / output_height;

            uint64_t pixel_sum = 0;
            size_t pixel_count = 0;

            // Sum all source pixels in this rectangle.
            for (size_t source_y = source_y_start;
                 source_y < source_y_end;
                 source_y++) {

                for (size_t source_x = source_x_start;
                     source_x < source_x_end;
                     source_x++) {

                    size_t source_index =
                        source_y * (size_t)src_image->width + source_x;

                    pixel_sum += src_image->pixels[source_index];
                    pixel_count++;
                }
            }

            // Calculate the average and store the output pixel.
            size_t output_index =
                output_y * output_width + output_x;

            downscaled_img->pixels[output_index] =
                (unsigned char)(pixel_sum / pixel_count);
        }
    }

    return downscaled_img;
}

