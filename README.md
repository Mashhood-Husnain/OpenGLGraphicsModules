# OpenGLGraphicsModules

A small, reusable C++17 library that wraps the repetitive parts of OpenGL: window creation, shaders, meshes, a free-fly camera and keyboard/mouse input. Drop it into a project, link one CMake target, and start drawing.

GLFW, glad and glm are bundled, so there is nothing else to install.

## Features

| Module | Class | What it does |
|--------|-------|--------------|
| `Window` | `OpenGLModWindow` | Creates a GLFW window and OpenGL context, runs the main loop, toggles depth testing |
| `Shader` | `OpenGLModShader` | Loads, compiles and links vertex and fragment shaders from files; typed uniform setters with a location cache |
| `Mesh` | `OpenGLModMesh` | Wraps VAO/VBO/EBO; describe your vertex layout and draw with one call |
| `Camera` | `OpenGLModCamera` | Free-fly (FPS style) camera that produces view and projection matrices |
| `Input` | `OpenGLModInput` | Keyboard and mouse state: held, pressed this frame, released this frame, mouse delta, scroll |

Design notes:

- Modules are independent. The camera does not know about input, and the mesh does not know about shaders, so you can use one without the others.
- `Shader` and `Mesh` own their OpenGL objects. They are freed in the destructor, and the classes are move-only.

## Requirements

- CMake 3.16 or newer
- A C++17 compiler (developed with MSVC 2022 on Windows)
- A GPU and driver with OpenGL 3.3 or newer

On Linux, GLFW needs the usual X11 or Wayland development packages. Linux and macOS builds have not been tested yet.

## Folder layout

```
OpenGLGraphicsModules/
├── CMakeLists.txt
├── Camera/
├── Input/
├── Mesh/
├── Shader/
├── Window/
└── vendors/            GLFW, glad, glm
```

## Getting it into your project

### Option A: git submodule (recommended)

```bash
git submodule add https://github.com/mashhood-husnain/OpenGLGraphicsModules.git OpenGLGraphicsModules
```

To pull in later library changes:

```bash
git submodule update --remote OpenGLGraphicsModules
git add OpenGLGraphicsModules
git commit -m "Update graphics library"
```

When cloning a project that uses it, add `--recurse-submodules`, or run `git submodule update --init --recursive` afterwards.

### Option B: copy the folder

Paste the `OpenGLGraphicsModules` folder into your project. Simple, but your copies can drift apart over time.

## CMake setup

In your project's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(OpenGLGraphicsModules)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE OGM::OGM)

# Copy your shaders folder next to the executable after every build
ogm_copy_folder(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/shaders" "shaders")
```

Linking `OGM::OGM` automatically gives you GLFW, glad and glm, so you can write `#include <glm/glm.hpp>` and `#include <GLFW/glfw3.h>` in your own code.

`ogm_copy_folder(<target> <source folder> <name in build output>)` is a helper function defined by this library. It is optional.

Build:

```bash
cmake -S . -B build
cmake --build build
```

After adding new source files to the library, re-run the first command.

## Quick start

A spinning cube you can fly around. Shaders go in `shaders/`.

`shaders/basic.vert`

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vColor;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vColor = aColor;
}
```

`shaders/basic.frag`

```glsl
#version 330 core
in vec3 vColor;
uniform vec3 uTint;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor * uTint, 1.0);
}
```

`src/main.cpp`

```cpp
#include "Window/OpenGLModWindow.hpp"
#include "Shader/OpenGLModShader.hpp"
#include "Mesh/OpenGLModMesh.hpp"
#include "Input/OpenGLModInput.hpp"
#include "Camera/OpenGLModCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

int main()
{
    // The window creates the OpenGL context, so it must come first
    OpenGLModWindow window("My Window");

    OpenGLModShader shader("shaders/basic.vert", "shaders/basic.frag");
    if (!shader.isValid()) return -1;

    OpenGLModCamera camera(glm::vec3(0.0f, 0.0f, 4.0f));
    OpenGLModInput input(window.getWindowPtr());

    // Vertex data is interleaved: {3, 3} means 3 floats position + 3 floats colour
    OpenGLModMesh cube(
        { -0.5f,-0.5f, 0.5f,  1,0,0,
           0.5f,-0.5f, 0.5f,  0,1,0,
           0.5f, 0.5f, 0.5f,  0,0,1,
          -0.5f, 0.5f, 0.5f,  1,1,0,
          -0.5f,-0.5f,-0.5f,  1,0,1,
           0.5f,-0.5f,-0.5f,  0,1,1,
           0.5f, 0.5f,-0.5f,  1,1,1,
          -0.5f, 0.5f,-0.5f,  0.3f,0.3f,0.3f },
        { 3, 3 },
        { 0,1,2, 2,3,0,   1,5,6, 6,2,1,   5,4,7, 7,6,5,
          4,0,3, 3,7,4,   3,2,6, 6,7,3,   4,5,1, 1,0,4 });

    window.setDepthTest(true);

    float last_time = (float)glfwGetTime();

    window.run([&]()
    {
        float now = (float)glfwGetTime();
        float dt = now - last_time;
        last_time = now;

        if (input.keyPressed(GLFW_KEY_ESCAPE)) window.terminate();

        if (input.keyDown(GLFW_KEY_W)) camera.move(OpenGLModCamera::Move::Forward,  dt);
        if (input.keyDown(GLFW_KEY_S)) camera.move(OpenGLModCamera::Move::Backward, dt);
        if (input.keyDown(GLFW_KEY_A)) camera.move(OpenGLModCamera::Move::Left,     dt);
        if (input.keyDown(GLFW_KEY_D)) camera.move(OpenGLModCamera::Move::Right,    dt);

        // Hold the right mouse button to look around
        if (input.mousePressed(GLFW_MOUSE_BUTTON_RIGHT))  input.setCursorMode(GLFW_CURSOR_DISABLED);
        if (input.mouseReleased(GLFW_MOUSE_BUTTON_RIGHT)) input.setCursorMode(GLFW_CURSOR_NORMAL);
        if (input.mouseDown(GLFW_MOUSE_BUTTON_RIGHT))
        {
            glm::vec2 d = input.mouseDelta();
            camera.rotate(d.x, -d.y);   // screen y grows downward, so flip it
        }
        camera.zoom(input.scrollDelta());

        int w, h;
        glfwGetFramebufferSize(window.getWindowPtr(), &w, &h);
        if (h == 0) h = 1;

        shader.use();
        shader.setMat4("uModel", glm::rotate(glm::mat4(1.0f), now * 0.5f, glm::vec3(0.5f, 1.0f, 0.0f)));
        shader.setMat4("uView", camera.viewMatrix());
        shader.setMat4("uProjection", camera.projectionMatrix((float)w / (float)h));
        shader.setVec3("uTint", glm::vec3(1.0f));
        cube.draw();

        input.newFrame();   // last: clears the per-frame state after it has been read
    });

    return 0;
}
```

## Module reference

### Window

```cpp
OpenGLModWindow window("Title");
window.getWindowPtr();            // raw GLFWwindow*
window.setDepthTest(true);        // enable or disable depth testing
window.run([&]() { /* per frame */ });
window.terminate();               // ask the main loop to end
```

### Shader

```cpp
OpenGLModShader shader("shaders/a.vert", "shaders/a.frag");
if (!shader.isValid()) { /* compile or link error, already printed to stderr */ }

shader.use();                     // bind before setting uniforms
shader.setInt("uTexture", 0);
shader.setFloat("uTime", t);
shader.setVec3("uColor", glm::vec3(1, 0, 0));
shader.setMat4("uModel", model);
```

Also available: `setBool`, `setVec2`, `setVec4`, `setMat3`. Missing uniforms print a message once and are ignored.

### Mesh

```cpp
// vertices:   interleaved floats
// attributes: floats per attribute, e.g. {3, 3, 2} = position, colour, uv
// indices:    optional; without them the mesh is drawn with glDrawArrays
OpenGLModMesh mesh(vertices, {3, 3}, indices);
mesh.draw();                      // GL_TRIANGLES by default
mesh.draw(GL_LINES);              // any GL primitive mode
```

### Camera

```cpp
OpenGLModCamera camera(glm::vec3(0, 0, 4));

camera.move(OpenGLModCamera::Move::Forward, dt);   // Forward, Backward, Left, Right, Up, Down
camera.rotate(dx, dy);            // positive dx turns right, positive dy looks up
camera.zoom(scroll);              // changes the field of view, clamped to 1..90

camera.viewMatrix();
camera.projectionMatrix(aspect);  // aspect = width / height
```

Public settings: `position`, `speed`, `sensitivity`, `fov`, `nearplane`, `farplane`.

### Input

```cpp
OpenGLModInput input(window.getWindowPtr());

input.keyDown(GLFW_KEY_W);        // held
input.keyPressed(GLFW_KEY_SPACE); // went down this frame
input.keyReleased(GLFW_KEY_E);    // came up this frame

input.mouseDown(GLFW_MOUSE_BUTTON_LEFT);   // also mousePressed / mouseReleased
input.mousePosition();
input.mouseDelta();               // pixels moved this frame, y grows downward
input.scrollDelta();

input.setCursorMode(GLFW_CURSOR_DISABLED); // or GLFW_CURSOR_NORMAL / GLFW_CURSOR_HIDDEN
input.newFrame();                 // call once per frame, after you have read the input
```

## Limitations

- OpenGL 3.3 core style; no compute shaders or modern direct state access.
- No texture, lighting or model loading modules yet.
- One window and one input object per window; no multi-context support.

- I am currently working on this library for my own personal projects as well, so over time this library will grow.
