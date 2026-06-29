# thengine

`thengine` is a modern cross-platform C++ 2D game engine built on top of the **SDL3** library. 

This repository serves as a practical, highly-constrained experiment in **agentic software engineering**. The entire engine and its test sandbox have been implemented utilizing **Google Antigravity**, driven by architectural patterns inspired by Microsoft XNA and MonoGame.

> [!IMPORTANT]
> **Project Status:** Up to this point, exactly **zero manual code edits** have been made to the core engine files. Every single line is orchestrate-designed by AI under strict architectural guidelines.

---

## Core Architecture

* **C++20 Pipeline:** Rigorous memory management (`std::unique_ptr` preference), `noexcept` safety, and extensive compile-time constraints using C++20 concepts.
* **Modern Text & Unicode:** True UTF-8 rendering supporting Finnish vowels (`ä`, `ö`, `å`) using type-safe `std::string_view` interfaces and `std::format`.
* **Transformable 2D Camera:** View space supporting independent position, zoom, and free rotation.
* **Optimized Frustum Culling:** An inverse-matrix view-space culling pipeline to guarantee maximum rendering performance.
* **Resource System:** Explicit `ContentManager` API for centralized loading of assets and custom shaders.

---

## Quick Start (Linux)

To generate build files and compile the engine along with the sandbox test application, run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build