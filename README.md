# Renter

**Renter is a small C program for rendering a still image as grayscale ASCII art directly in a terminal.** It is built around Linux terminal I/O and the image-processing primitives needed to turn an ordinary raster image into a grid of characters.

Renter's defined scope is intentionally focused: **one input image in, one static ASCII rendering out**. The project is documented as a complete, narrow-scope utility rather than a framework with a future feature roadmap. A 3D engine and a separate maths engine are **not part of the project scope or planned features**.

## What it does

Given an image file, Renter:

1. Loads the image as RGBA pixels using the bundled `stb_image` library.
2. Converts the image to grayscale using a weighted RGB formula.
3. Resizes the image while preserving its aspect ratio using bounds derived from the terminal size.
4. Maps each grayscale sample to a character from a fixed ASCII ramp.
5. Writes two copies of each selected character per sample, then a newline per image row. The horizontal duplication partly compensates for the non-square shape of terminal character cells.
6. Writes the generated buffer to standard output with `write(2)`.

This is a static image renderer, not a terminal emulator, graphical windowing system, animation player, or general-purpose graphics API.

## Quick start

### Requirements

- Linux or a compatible POSIX-like environment with `ioctl(2)`, `TIOCGWINSZ`, and `write(2)`.
- A C compiler such as GCC or Clang.
- `make` (optional; a direct compiler command is also provided below).
- An interactive terminal whose dimensions can be queried through standard output.

The image-loading, image-resizing, and PNG-writing code is vendored in `include/`; there is no separate image-library package to install.

### Build

From the repository root:

```sh
make
```

The executable is written to `build/renter`.

To build without `make`:

```sh
mkdir -p build
cc -std=c11 -O2 -Wall -Wextra -Iinclude \
  src/main.c src/terminal.c src/framebuffer.c src/image.c \
  -o build/renter -lm
```

The `-lm` flag links the standard math library used by the resizing implementation.

### Render an image

The repository includes a sample landscape image:

```sh
./build/renter images/mountain-landscape-3840x2160-26973.png
```

A sample JPEG whose filename contains spaces can be used as well:

```sh
./build/renter "images/WhatsApp Image 2026-10-23 at 2.33.49 AM.jpeg"
```

You can also provide a path to another supported image:

```sh
./build/renter /path/to/photo.jpg
```

Run Renter in a normal interactive terminal. It writes the rendered image at the terminal's current cursor position and exits after that single rendering. The current command-line interface accepts the image path as its first argument; it does not have options for choosing an ASCII ramp, output size, color mode, or output file.

## Example output

The exact characters depend on the input image and terminal dimensions, but the output is a grid of character cells resembling this schematic example:

```text
..............::::::----====++++####@@@@
............:::::-----====+++++####@@@@@@
..........:::::-----=====++++####@@@@@    
........:::::-----=====++++####@@@@       
......:::::-----=====++++####@@@          
```

This is illustrative, not the exact output of the bundled sample image.

More command examples and an explanation of the processing pipeline are in [Usage and examples](docs/examples.md).

## How the renderer works

```text
Input image
    |
    v
stbi_load()              Load RGBA pixels
    |
    v
TBW_pixels()             Create grayscale RGB values; preserve alpha
    |
    v
resize_image()           Resize while preserving aspect ratio
    |
    v
get_grayScale_img()      Store one grayscale byte per pixel
    |
    v
get_framebuffer()        Map values to ASCII; duplicate each character
    |
    v
paintTerminal()          Write the frame to standard output
    |
    v
Terminal
```

Grayscale intensity is calculated as:

```text
gray = 0.299 * red + 0.587 * green + 0.114 * blue
```

Each input sample is mapped through the fixed ramp in `src/framebuffer.c`:

```c
".:-=+%#@ "
```

The ramp and mapping formula are hard-coded. Each chosen character is output twice horizontally, and each row ends with a newline. See [Architecture](docs/architecture.md) and [Image pipeline](docs/image-pipeline.md) for details.

## Project layout

```text
renter/
├── include/
│   ├── framebuffer.h
│   ├── image.h
│   ├── terminal.h
│   ├── stb_image.h
│   ├── stb_image_resize2.h
│   └── stb_image_write.h
├── src/
│   ├── main.c
│   ├── image.c
│   ├── framebuffer.c
│   └── terminal.c
├── images/                 Sample images
├── docs/
│   ├── architecture.md
│   ├── api.md
│   ├── examples.md
│   ├── image-pipeline.md
│   └── development.md
├── Makefile
└── README.md
```

## Scope and limitations

Renter's implemented purpose is image-to-ASCII rendering in a terminal. The current code does **not** implement terminal animations, video playback, true-color output, a graphical window, drawing primitives, or an interactive rendering loop. A 3D engine and a standalone maths engine are outside the scope; they are not planned components of this project.

The current implementation is small and has known limitations:

- It uses one fixed ASCII ramp and one grayscale output mode.
- The terminal's current size is queried at runtime; there is no command-line override or fallback size.
- The framebuffer duplicates each pixel character horizontally, but the resize limit currently uses the full terminal column count as the image width. As a result, an image can exceed the terminal's width. The output dimensions are not yet perfectly matched to the visible character grid.
- The command-line entry point assumes an image path was supplied. Omitting the path does not produce a polished usage message.
- Image-load and terminal-size failure handling is not fully hardened for all error cases.
- There is no automated test suite in the repository; building and trying the sample images is the current smoke test.
- No project-level `LICENSE` file is included. The bundled `stb` headers carry their own notices; the project's licensing terms have not been declared in this repository.

These limitations describe the supplied implementation; they do not imply a commitment to add features outside the project's defined scope.

## Documentation

- [Usage and examples](docs/examples.md) — build/run examples and expected behavior.
- [Architecture](docs/architecture.md) — responsibility of each source file and the end-to-end data flow.
- [API reference](docs/api.md) — public headers, function signatures, and allocation/ownership notes.
- [Image pipeline](docs/image-pipeline.md) — image representation, grayscale conversion, resizing, and ASCII mapping.
- [Development notes](docs/development.md) — build commands, troubleshooting, validation, and known implementation caveats.
