# Simple C OpenGL Engine

## Hey there!

This is a small graphics framework/engine I'm building primarily as a way to learn C and explore OpenGL graphics programming. It's a place for experimentation and learning, not a production-ready engine.

## Conceptual Overview

The project aims to provide basic building blocks for rendering graphics with OpenGL. Here's a breakdown of the core ideas and components involved:

1.  **Windowing and Input (GLFW):** Handles creating the application window, managing the OpenGL context, and processing basic input events.
2.  **OpenGL Function Loading (GLAD):** Loads the necessary OpenGL functions at runtime, making it possible to use modern OpenGL features across different platforms and drivers.
3.  **Graphics Rendering Pipeline (OpenGL 3.3 Core):**
    *   **Shaders:** Reads GLSL shader code (vertex and fragment) from separate files (`shaders/`).
    *   **Compilation & Linking:** Compiles these shaders and links them into a shader program used by the GPU.
    *   **Vertex Data:** Defines and sends simple vertex data (like the position and color for a triangle) to the GPU using Vertex Buffer Objects (VBOs) and Vertex Array Objects (VAOs).
    *   **Drawing:** Issues basic draw commands (`glDrawArrays`) to render the geometry.
4.  **Math Operations (CGLM):** Uses the CGLM library (found in `external/`) for vector and matrix math, essential for transformations in graphics.
5.  **Build System (CMake):** Uses CMake (`CMakeLists.txt`) to manage the build process, making it easier to compile on different systems (though primarily tested on Linux).
6.  **Utilities (Implemented in C):**
    *   Basic OpenGL error checking (`GL_CALL` macros).
    *   Simple file reading function for shaders.
    *   Cross-platform time retrieval (`get_time`).
    *   Basic timestamped console output (`print`).

## How It's Organized

The project follows a fairly standard C project structure:

*   **Source Code (`src/`):** Contains the main C code driving the application (`main.c`).
*   **Shaders (`shaders/`):** Holds the GLSL vertex and fragment shader files.
*   **External Libraries (`external/`):** Includes dependencies like CGLM and GLAD source files.
*   **Build Configuration (`CMakeLists.txt`):** Defines how to build the project using CMake.
