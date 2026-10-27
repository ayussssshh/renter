# API reference

The public declarations are in `include/image.h`, `include/framebuffer.h`, and `include/terminal.h`. They are plain C functions intended for use by the program's pipeline. The API is not currently packaged as a separate library with an installation target.

## Image API — `include/image.h`

### `Image_Data`

```c
typedef struct {
    unsigned char *pixels;
    int width;
    int height;
    int channels;
} Image_Data;
```

`pixels` points to interleaved pixel components. `width` and `height` are measured in pixels; `channels` is the number of bytes per pixel.

### `Image_Data *get_imagePixels(char imagePath[])`

Loads an image from `imagePath` using the bundled `stb_image` implementation. The function requests four output channels, so a successful result has `channels == 4`.

**Returns:** an allocated `Image_Data *` when loading succeeds. The current implementation does not safely propagate every load or allocation failure; callers should not assume every non-null path yields a valid image.

### `Image_Data *TBW_pixels(Image_Data *image)`

Allocates a grayscale-colored copy of the source image. RGB are set to the same weighted grayscale value, and alpha is copied from the original pixel.

**Returns:** a newly allocated `Image_Data *` on success. The current implementation has incomplete error handling and does not release all of its temporary allocations.

### `Image_Data *resize_image(const Image_Data *original, int new_width, int new_height)`

Resizes an image to fit within the provided width and height bounds while preserving its aspect ratio. The returned image dimensions can be smaller than either bound.

**Returns:** a newly allocated image on success, or `NULL` for several invalid-input/allocation/resizing failures.

### `int *get_IRSDimensions(struct winsize ws, Image_Data *image)`

Calculates dimensions by scaling an image against `ws.ws_xpixel` and `ws.ws_ypixel`.

**Returns:** a heap-allocated two-element integer array, where element `0` is width and element `1` is height. The current `main()` does not call this helper. The caller must free the returned array with `free()`.

### `int save_image(const Image_Data *image, const char *filename)`

Writes image pixels to a PNG file using `stbi_write_png()`.

**Returns:** the underlying `stbi_write_png()` result (non-zero on success, zero on failure). The function does not validate all dimension/stride edge cases.

### `unsigned char *get_grayScale_img(Image_Data *image)`

Converts an RGB or RGBA image into a one-byte-per-pixel grayscale array using the weighted RGB formula.

**Returns:** a heap-allocated buffer of `width * height` bytes, or `NULL` when validation or allocation fails. The caller must free the returned buffer with `free()`.

## Framebuffer API — `include/framebuffer.h`

### `char *get_framebuffer(int height, int width, unsigned char *grayScale_img, int grayScale_img_width)`

Converts grayscale samples into a character framebuffer using the fixed ASCII ramp. `grayScale_img_width` is the row stride in grayscale samples; it should be at least `width`. Each logical pixel becomes two copies of one ASCII character, and a newline follows each row.

**Returns:** a heap-allocated, null-terminated character buffer, or `NULL` for invalid dimensions, invalid arguments, overflow checks, or allocation failure. The caller must free the returned buffer with `free()`.

The printable buffer length can be calculated as:

```c
size_t length = (size_t)height * (2 * (size_t)width + 1);
```

That length excludes the terminating null byte.

## Terminal API — `include/terminal.h`

### `struct winsize getTerminalSize(void)`

Queries the terminal attached to standard output using `ioctl()` and `TIOCGWINSZ`.

**Returns:** a `struct winsize` with fields such as `ws_col` and `ws_row` on success. On failure the function prints an error, but the current implementation does not return a separate status code or guarantee a zero-initialized result. Use from an interactive terminal.

### `int paintTerminal(char *framebuffer, size_t length)`

Writes `length` bytes from `framebuffer` to file descriptor `1` (standard output). It handles partial writes and retries interrupted writes.

**Returns:** `0` when all bytes were written and `-1` if a non-retryable `write()` error occurs. The caller owns the buffer and remains responsible for freeing it.

## Memory ownership summary

The current project does not expose a single destructor for `Image_Data`. In general:

| Allocation | Expected release |
|---|---|
| Struct returned by `get_imagePixels()` | `free(image)` |
| Pixel memory returned by `stbi_load()` inside `get_imagePixels()` | `stbi_image_free(image->pixels)`; include `stb_image.h` for the function declaration |
| Struct and pixel memory returned by `resize_image()` | `free(image->pixels)` then `free(image)` |
| Struct and pixel memory returned by `TBW_pixels()` | `free(image->pixels)` then `free(image)`; note the current function also leaks internal temporary allocations |
| Buffer returned by `get_grayScale_img()` | `free(buffer)` |
| Buffer returned by `get_framebuffer()` | `free(buffer)` |
| Array returned by `get_IRSDimensions()` | `free(array)` |

These ownership notes describe the allocations visible in the current implementation. `main.c` currently frees only the final character framebuffer and compact grayscale buffer, so some intermediate image allocations remain allocated until process exit. This is bounded for a one-shot command-line run but should be improved before the functions are reused in a long-running process.
