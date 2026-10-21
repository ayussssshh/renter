#if !defined (IMAGE_H)
#define IMAGE_H


#include <sys/ioctl.h>


typedef struct 
{
    unsigned char *pixels;
    int width;
    int height;
    int channels;

} Image_Data;

Image_Data *resize_image(const Image_Data *original, int new_width, int new_height);
Image_Data *get_imagePixels(char imagePath[]);
Image_Data *TBW_pixels(Image_Data *image);
int *get_IRSDimensions(struct winsize ws, Image_Data *image);
int save_image(const Image_Data *image, const char *filename);
unsigned char *get_grayScale_img(Image_Data *image);
unsigned char *AM_grayscale(unsigned char *grayscale_img, int grayscale_w, int grayscale_h, int terminal_h, int terminal_w);
Image_Data *get_downscale_img(Image_Data *src_image, int available_w, int available_h);


#endif // IMAGE_H