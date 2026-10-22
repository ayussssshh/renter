#include "image.h"
#include "terminal.h"
#include "framebuffer.h"
#include <sys/ioctl.h>
#include <stdlib.h>

int main(void)
{

    Image_Data *input_image = get_imagePixels("/home/adi/c_projects/renter/images/mountain-landscape-3840x2160-26973.png");

    Image_Data *BW_input_image = TBW_pixels(input_image);

    struct winsize ws = getTerminalSize();

    Image_Data *output_image =
        resize_image(BW_input_image, ws.ws_col, ws.ws_row);

    if (output_image == NULL)
    {
        fprintf(stderr, "Failed to resize image\n");
        return 1;
    }

    unsigned char *grayScale_img = get_grayScale_img(output_image);

    if (grayScale_img == NULL)
    {
        fprintf(stderr, "Failed to create grayscale image\n");
        return 1;
    }

    char *framebuffer = get_framebuffer(
        output_image->height,
        output_image->width,
        grayScale_img,
        output_image->width);

    if (framebuffer == NULL)
    {
        fprintf(stderr, "Failed to create framebuffer\n");
        return 1;
    }

    size_t length =
        (size_t)output_image->height *
        (2*(size_t)output_image->width + 1);

    if (paintTerminal(framebuffer, length) == -1)
    {
        perror("paintTerminal");
    }

    free(framebuffer);
    free(grayScale_img);
}
