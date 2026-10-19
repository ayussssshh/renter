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


#endif // IMAGE_H