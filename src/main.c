#include "image.h"

int main(){

    Image_Data *image = get_imagePixels("/home/ayush/c_projects/renter/the-punisher-one-3840x2160-26460.jpg");

    Image_Data *image1 = TBW_pixels(image);

    return 0;
}