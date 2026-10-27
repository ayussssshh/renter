# Usage and examples

All commands below are run from the repository root unless stated otherwise.

## 1. Build the executable

```sh
make
```

Equivalent direct build command:

```sh
mkdir -p build
cc -std=c11 -O2 -Wall -Wextra -Iinclude \
  src/main.c src/terminal.c src/framebuffer.c src/image.c \
  -o build/renter -lm
```

## 2. Render the bundled landscape

```sh
./build/renter images/mountain-landscape-3840x2160-26973.png
```

Renter reads the current terminal size, processes the image, and writes the ASCII representation at the current cursor position. The process exits after outputting one image; it does not keep a live window open.

## 3. Render the bundled JPEG with spaces in its filename

```sh
./build/renter "images/WhatsApp Image 2026-10-23 at 2.33.49 AM.jpeg"
```

Quote paths that contain spaces so the shell passes the complete path as one argument.

## 4. Render a different image

```sh
./build/renter "$HOME/Pictures/photo.jpg"
```

The file must be a raster format supported by the bundled `stb_image` version. Common supported formats include JPEG, PNG, BMP, and TGA. Support for any particular file depends on the decoder and the file's encoding.

## 5. Follow the pipeline by function

The main program's sequence is the most useful example of how Renter works:

```c
Image_Data *input_image = get_imagePixels(argv[1]);
Image_Data *bw_image = TBW_pixels(input_image);
struct winsize ws = getTerminalSize();
Image_Data *output_image = resize_image(bw_image, ws.ws_col, ws.ws_row);
unsigned char *gray = get_grayScale_img(output_image);
char *framebuffer = get_framebuffer(
    output_image->height,
    output_image->width,
    gray,
    output_image->width
);
```

This is a shortened illustration of the flow, not a complete standalone program. The current `main.c` contains the output-length calculation and terminal write. For complete function signatures and memory-ownership details, read the [API reference](api.md).

## What the output means

A rendered image is plain terminal text. Each logical pixel is mapped to a character from `".:-=+%#@ "`, duplicated horizontally, with line breaks between rows. The character density and the final appearance depend on the image, terminal font, terminal size, and the fixed mapping.

## Practical notes

- Run inside an interactive terminal. The program queries terminal size through standard output; redirected output and non-terminal contexts are not reliably supported.
- Make the terminal window large enough to see more of the image, but remember that the current width calculation can let output overflow horizontally because each sample prints two characters.
- Pass one image path as the first argument. There are no command-line flags or interactive controls.
- The executable writes directly to stdout and does not clear the screen first.
