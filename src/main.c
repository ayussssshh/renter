#include "image.h"
#include "terminal.h"
#include "framebuffer.h"
#include <sys/ioctl.h>


int main(void)
{

    Image_Data *input_image = get_imagePixels("/home/ayush/c_projects/renter/Screenshot from 2026-05-23 18-47-46.png");

    Image_Data *BW_input_image = TBW_pixels(input_image);

    struct winsize ws = getTerminalSize();

    Image_Data *output_image = resize_image(BW_input_image, ws.ws_col, ws.ws_row);

    unsigned char *grayScale_img = get_grayScale_img(output_image);

    char *framebuffer = get_framebuffer(ws.ws_row, ws.ws_col, grayScale_img);

    size_t length = (size_t)ws.ws_col * (size_t)ws.ws_row;

    int x = paintTerminal(framebuffer, length);

    return 0;

}
