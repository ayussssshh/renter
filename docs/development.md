# Development notes

## Build

The repository uses a small `Makefile` and a direct C compilation path. From the repository root:

```sh
make
```

The output binary is `build/renter`. Clean generated build output with:

```sh
make clean
```

The equivalent compiler invocation is:

```sh
mkdir -p build
cc -std=c11 -O2 -Wall -Wextra -Iinclude \
  src/main.c src/terminal.c src/framebuffer.c src/image.c \
  -o build/renter -lm
```

`-Iinclude` makes the project and vendored headers available. `-lm` links the math library. There is no CMake project or external package-manager dependency in the supplied code.

## Smoke test

There is no automated test runner at present. After building, run one of the included images in an interactive terminal:

```sh
./build/renter images/mountain-landscape-3840x2160-26973.png
```

A successful run should print rows of ASCII characters and then return to the shell. The process is expected to finish after one frame.

## Troubleshooting

### `make: command not found`

Install `make` using the package manager for the operating system, or use the direct compiler command instead. A C compiler is still required.

### `cc: command not found` or `gcc: command not found`

Install a C compiler, such as GCC or Clang.

### The program does not display an image

Check that the image path is correct and that the file format is supported by the bundled `stb_image` decoder. The current error handling for failed image loads is incomplete, so an invalid path may not produce a friendly message.

### Output is too wide or clipped

Each sampled pixel is emitted as two terminal characters, but the resize limit currently uses the terminal's full column count. That can make the text wider than the terminal. This is a limitation of the current size calculation, not an issue with the image file.

### Terminal-size errors or unexpected dimensions

Renter queries standard output for terminal dimensions. Use an interactive terminal rather than redirecting output to a file or piping it to another process. The current implementation does not provide a configured fallback size if the `ioctl()` call fails.

## Implementation caveats

- `main()` does not check that an argument was supplied before reading `argv[1]`.
- `get_imagePixels()` reports a `stbi_load()` failure but does not return safely from every failure path.
- Some helper functions do not consistently check every allocation before using it.
- Intermediate images and helper allocations are not all freed by the current one-shot CLI path.
- The `resize_image()` argument validation repeats the check for `available_w` rather than checking `available_h` in that condition. Callers should pass positive width and height bounds.
- The returned terminal-size structure is not guaranteed to be initialized if the `ioctl()` call fails.
- No automated unit or integration tests are included.

These points are documented so the implementation's current behavior is clear. The intended project scope remains a straightforward static image-to-ASCII utility, not a general-purpose rendering framework.

## Scope and dependencies

The project uses the bundled `stb_image.h`, `stb_image_resize2.h`, and `stb_image_write.h` headers for image loading, resizing, and PNG output support. The rest of the pipeline is implemented in C with standard/POSIX-style APIs. A 3D engine and a separate maths engine are not part of this project's scope.
