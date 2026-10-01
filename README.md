# Renter

**Renter** is a small, experimental **terminal rendering engine written in C**.

The project explores how terminal-based graphics can be built from the ground up using low-level Linux and C concepts rather than relying on high-level terminal rendering libraries.

The long-term goal is to build a general-purpose rendering system for the terminal capable of rendering static scenes, animations, visualizations, ASCII graphics, and eventually video-like content.

> **Renter is currently in its very early development stage.**

---

## Philosophy

Renter is primarily a learning project.

The goal is not to create the most feature-rich terminal graphics library, but to understand and implement the fundamental pieces involved in a rendering pipeline.

The project focuses on concepts such as:

* Memory management
* Character buffers
* Framebuffers
* Terminal dimensions
* Rendering loops
* Linux system calls
* Terminal I/O
* ANSI escape sequences
* Frame updates
* Terminal resizing
* Animation
* Data representation

The implementation is intentionally built from relatively low-level primitives so that the relationship between the program, the operating system, and the terminal remains visible.

---

## Current Concept

The basic rendering pipeline is currently envisioned as:

```text
Application
     │
     ▼
Character Framebuffer
     │
     ▼
Renderer
     │
     ▼
Linux write()
     │
     ▼
PTY / Terminal
     │
     ▼
Terminal Emulator
```

The framebuffer represents the current frame as a collection of terminal characters.

For example:

```text
+----------------------+
|                      |
|        @             |
|       /|\            |
|       / \            |
|                      |
+----------------------+
```

Internally, this can be represented as a one-dimensional block of memory while being treated as a two-dimensional canvas:

```text
index = y * width + x
```

---

## Goals

The long-term goals of Renter include:

* [ ] Terminal size detection
* [ ] Character framebuffer
* [ ] Basic framebuffer management
* [ ] Rendering directly with `write()`
* [ ] Terminal cursor control
* [ ] Screen clearing
* [ ] Efficient frame updates
* [ ] Rendering loop
* [ ] Terminal resize handling
* [ ] Basic animation
* [ ] Drawing primitives
* [ ] Text rendering
* [ ] ASCII image rendering
* [ ] Frame timing
* [ ] Double buffering
* [ ] Video/frame playback
* [ ] General-purpose terminal visualization

These features will be implemented incrementally.

---

## Architecture

Renter is intended to separate **what should be displayed** from **how it is displayed**.

At a high level:

```text
                 Application
                     │
                     ▼
              ┌──────────────┐
              │  Framebuffer │
              └──────┬───────┘
                     │
                     ▼
              ┌──────────────┐
              │   Renderer   │
              └──────┬───────┘
                     │
                     ▼
                  write()
                     │
                     ▼
                  Linux
                     │
                     ▼
              Terminal Emulator
```

The framebuffer should contain the representation of a frame.

The renderer is responsible for converting that representation into terminal output.

This separation allows the rendering mechanism to evolve without fundamentally changing how a frame is represented.

---

## Why C?

Renter is intentionally written in C.

C provides direct access to concepts that are important to the project:

* Memory allocation
* Pointers
* Arrays
* Structs
* File descriptors
* System calls
* Signals
* Process behavior
* Linux APIs

Using C also makes it possible to explore the boundary between a user-space program and the Linux operating system directly.

---

## Why the Terminal?

The terminal provides a constrained but interesting rendering environment.

Instead of dealing with millions of individual pixels, Renter initially works with a grid of character cells:

```text
Terminal
┌──────────────────────────────┐
│                              │
│                              │
│                              │
│                              │
│                              │
└──────────────────────────────┘
        columns × rows
```

This makes it possible to build a rendering system while keeping the underlying representation simple.

The terminal can then become the output device for experiments involving:

* Mathematical visualization
* Algorithms
* ASCII graphics
* Simulations
* Animations
* Data visualization
* Procedurally generated scenes

---

## Current Development Stage

Renter is currently **experimental and incomplete**.

The first implementation is intentionally simple:

```text
Get terminal size
       │
       ▼
Create framebuffer
       │
       ▼
Modify framebuffer
       │
       ▼
Render framebuffer
       │
       ▼
write()
       │
       ▼
Terminal
```

The project will be expanded gradually rather than attempting to implement the entire rendering engine at once.

---

## Project Structure

The initial project structure is intentionally minimal:

```text
renter/
├── src/
│   ├── main.c
│   └── framebuffer.c
│
├── include/
│   └── framebuffer.h
│
├── examples/
│   └── basic.c
│
├── build/
│
├── docs/
│   └── architecture.md
│
├── README.md
```

The structure may change as the project develops.

---

## Building

Renter is currently built using a simple `Makefile`.

```bash
make
```

The resulting binaries and object files are placed in the `build/` directory.

---

## Status

🚧 **Early Development**

Renter is currently a personal experimental project.

The API, architecture, and internal representation are expected to change substantially during development.

---

## License

License information will be added later.
