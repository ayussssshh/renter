#if !defined (IMAGE_H)
#define IMAGE_H


typedef struct 
{
    unsigned char *pixels;
    int width;
    int height;
    int channels;

} Image_Data;



Image_Data *get_imagePixels(char imagePath[]);
Image_Data *TBW_pixels(Image_Data *image);

#endif