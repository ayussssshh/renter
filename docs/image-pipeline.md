# Image pipeline

This document follows one image from file input to terminal text.

## 1. Load pixels

`get_imagePixels()` calls `stbi_load()` with a requested output channel count of `4`. The resulting pixel array therefore contains red, green, blue, and alpha bytes for every pixel, even when the source file has fewer channels.

For an image of width `W` and height `H`, the expected RGBA buffer size is:

```text
W * H * 4 bytes
```

This code path expects the image to load successfully. The current implementation prints a decode error when `stbi_load()` fails but does not return immediately; failure handling should be treated as a limitation of the current code.

## 2. Convert to grayscale

`TBW_pixels()` allocates a second image. For each RGBA source pixel, it calculates a weighted intensity:

```text
gray = 0.299 * red + 0.587 * green + 0.114 * blue
```

The resulting byte is copied into all three color channels:

```text
red   = gray
green = gray
blue  = gray
alpha = original alpha
```

The weights give more influence to green than blue, approximating the perceived brightness of the RGB channels. `get_grayScale_img()` later calculates the grayscale value again from the transformed RGB buffer and stores one byte per pixel.

## 3. Resize with aspect ratio preserved

`resize_image(original, available_w, available_h)` computes two scale ratios:

```text
width_ratio  = available_w  / original_width
height_ratio = available_h / original_height
scale = min(width_ratio, height_ratio)

new_width  = floor(scale * original_width)
new_height = floor(scale * original_height)
```

The implementation uses `stbir_resize_uint8_srgb()` to resize the pixel data. The purpose of choosing the smaller ratio is to keep the full image within both requested bounds without intentionally stretching it.

In the current application, the bounds come from `ws_col` and `ws_row` returned by the terminal-size query. The renderer later doubles each image sample horizontally, so using the entire terminal column count as `available_w` can produce output wider than the terminal. This is a known mismatch in the current implementation.

## 4. Build a compact grayscale buffer

`get_grayScale_img()` allocates `width * height` bytes. It reads RGB components from the resized image and writes one intensity byte per pixel:

```text
gray_index = y * image_width + x
source_index = gray_index * channels
```

For each pixel it reads the red, green, and blue components; alpha does not contribute to the brightness calculation.

## 5. Map brightness to a character

`get_framebuffer()` uses this fixed ramp:

```c
static const char ASCII_RAMP[] = ".:-=+%#@ ";
```

It computes a ramp index using integer arithmetic:

```text
ramp_index = gray * (ramp_length - 1) / 255
character = ASCII_RAMP[ramp_index]
```

The index is clamped naturally by the calculation for byte values from `0` through `255`. The mapping is hard-coded; there is no runtime configuration for the ramp or brightness direction.

Each selected character is emitted twice in succession. For example, a logical row of four samples might become:

```text
Input samples:      0   64  128  255
Mapped characters:  .   -    +   (space)
Rendered row:       ..--++  
```

The exact characters follow the ramp and integer index calculation. A newline is appended after every row, and a null terminator is placed at the end of the allocated buffer.

## 6. Write to the terminal

`main()` calculates the printable length as:

```text
height * (2 * width + 1)
```

This excludes the final null terminator. `paintTerminal()` writes those bytes to standard output and loops until the full buffer is written, handling short writes and `EINTR`.

## Visual quality notes

Terminal character cells are usually taller than they are wide. Duplicating each character horizontally attempts to compensate for that shape, but it also means the display width is twice the logical image width. The current size calculation does not fully account for that duplication, and there is no option to choose another ramp, tune contrast, render ANSI colors, or specify output dimensions manually.
