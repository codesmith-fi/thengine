# thengine

`thengine` is a modern cross-platform C++ 2D game engine built on top of the **SDL3** library. 

This repository serves as a practical, highly-constrained experiment in **agentic software engineering**. The entire engine and its test sandbox have been implemented utilizing **Google Antigravity**, driven by architectural patterns inspired by Microsoft XNA and MonoGame.

> [!IMPORTANT]
> **Project Status:** Up to this point, exactly **zero manual code edits** have been made to the core engine files. Every single line is orchestrate-designed by AI under strict architectural guidelines.

**Without lighting**
![thengine in action, sandbox without lights](https://i.imgur.com/rJbme2n.png)

**With ambient and 3 point lights (RGB)**
![thengine in action, dark grey ambient and 3 moving point lights](https://i.imgur.com/23f7hjz.png)

---

## Core Architecture & Engine Features

* **Optimized Sprite Batching:** Implements an XNA-style `SpriteBatch` pipeline (`begin()` / `end()`) to minimize draw calls and optimize GPU vertex buffer allocation.
* **Fully Transformable 2D Sprites:** Supports standard 2D sprites with full, dynamic transformation matrices (independent position, scaling, origin-point offset, and free rotation).
* **Programmable Pipeline (GLSL Shaders):** Full support for dynamic loading and binding of custom OpenGL GLSL shaders to control runtime rendering effects.
* **Centralized Content Pipeline:** A unified `ContentManager` system for robust, centralized resource management and seamless loading of assets (bitmaps, textures, fonts, and shaders).
* **C++20 Type-Safety:** Built on top of the modern C++20 dialect featuring strict RAII memory management (`std::unique_ptr` enforcement), `noexcept` specifications, and `[[nodiscard]]` constraints.
* **Modern Text & Unicode:** True UTF-8 rendering supporting Finnish vowels (`ä`, `ö`, `å`) using zero-allocation `std::string_view` interfaces and `std::format`.
* **Dynamic 2D Camera:** Fully transformable camera tracking space supporting layout translation, zooming, and rotation.
* **View Frustum Culling:** An optimized inverse-matrix view-space culling routine that completely bypasses off-screen objects to save hardware overhead.

---

## Quick Start (Linux)

To generate build files and compile the engine along with the sandbox test application, run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build