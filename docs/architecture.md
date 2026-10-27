# Architecture

Renter is a small, single-process C program. Its architecture separates image preparation, character-buffer generation, and terminal output into a short linear pipeline.

## End-to-end flow

```text
CLI image path
      |
      v
main.c
      |
      +--> image.c: load RGBA image
      |        |
      |        +--> grayscale RGB values
      |        +--> resize image
      |        +--> one-byte-per-pixel grayscale buffer
      |
      +--> framebuffer.c: grayscale -> ASCII character buffer
      |
      +--> terminal.c: write buffer to stdout
      v
Interactive terminal
```

There is no persistent render loop. `main()` builds one output frame, writes it, releases the final framebuffer and grayscale buffer, and returns.

## Modules

### `src/main.c` — orchestration

`main()` calls the image and terminal helpers in pipeline order:

1. Load the path in `argv[1]`.
2. Transform RGB values to grayscale while keeping the image structure in RGBA form.
3. Query terminal dimensions.
4. Resize the image to the requested bounds.
5. Extract a compact grayscale byte array.
6. Build the character framebuffer.
7. Calculate the exact number of display bytes and write them to standard output.

The current entry point assumes `argv[1]` exists. Callers should supply an image path; argument validation is not yet complete.

### `src/image.c` — image data and pixel transforms

This module implements the `Image_Data` operations declared in `include/image.h`.

- `get_imagePixels()` loads an image through `stbi_load()` and requests four channels (RGBA) regardless of the source's original channel count.
- `TBW_pixels()` allocates a new image and writes the same grayscale value into red, green, and blue while preserving alpha.
- `resize_image()` calculates an aspect-ratio-preserving scale and uses `stbir_resize_uint8_srgb()` from the bundled `stb_image_resize2.h` implementation.
- `get_grayScale_img()` produces a separate one-byte-per-pixel array for ASCII mapping.
- `save_image()` writes an image as PNG through the bundled `stb_image_write.h` implementation.
- `get_IRSDimensions()` calculates an image size from terminal pixel dimensions, but this helper is not used by the current `main()` path.

The `stb` implementation macros are defined in this module, so the headers are compiled as part of the project; no separate image library needs to be linked.

### `src/framebuffer.c` — ASCII framebuffer

`get_framebuffer()` converts a row-major grayscale image into a heap-allocated, null-terminated character buffer. Each grayscale sample produces two identical characters; every row ends in `\n`.

For a logical image of `width` by `height`, the generated text occupies `height * (2 * width + 1)` bytes, not counting the final null terminator. The display code passes this content length to `paintTerminal()`.

### `src/terminal.c` — terminal query and output

- `getTerminalSize()` queries the terminal attached to standard output with `ioctl(STDOUT_FILENO, TIOCGWINSZ, ...)`.
- `paintTerminal()` writes the supplied byte range to file descriptor `1` (`stdout`). It handles partial writes and retries when `write()` is interrupted with `EINTR`.

The renderer does not clear the screen, move the cursor, switch terminal modes, or restore terminal state. It writes at the current cursor location and then exits.

## Data representation

`Image_Data` contains:

```c
typedef struct {
    unsigned char *pixels;
    int width;
    int height;
    int channels;
} Image_Data;
```

Pixel components are stored consecutively in a one-dimensional byte array. For an image with `channels` components per pixel, the address of component zero for pixel `(x, y)` is:

```text
((y * width) + x) * channels
```

The grayscale buffer contains a single byte per pixel, so the source index for `(x, y)` in that buffer is `y * width + x`.

The framebuffer uses a separate one-dimensional character buffer. Its line layout adds two characters per logical pixel plus one newline per row.

## Design boundaries

Renter is a static image-to-character converter. There is no separate renderer abstraction, scene graph, event system, graphics context, 3D engine, or standalone maths engine in the implementation or intended scope.
