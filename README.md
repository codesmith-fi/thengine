# thengine

`thengine` is a modern cross-platform C++ 2D game engine built on top of the **SDL3** library. 

This repository serves as a practical, highly-constrained experiment in **agentic software engineering**. The entire engine and its test sandbox have been implemented utilizing **Google Antigravity**, driven by architectural patterns inspired by Microsoft XNA and MonoGame.

> [!IMPORTANT]
> **Project Status:** Up to this point, exactly **zero manual code edits** have been made to the core engine files. Every single line is orchestrate-designed by AI under strict architectural guidelines.

**Supports Raycast lighting and shadows**
![Real time raycast lighting](https://i.imgur.com/J0CYK1A.png)

**Prototype realtime Roguelike - Emberborn skeleton**
![Prototype Realtime Roguelike Skeleton](https://i.imgur.com/FTRYvbX.png)

**Without lighting**
![thengine in action, sandbox without lights](https://i.imgur.com/rJbme2n.png)

**With ambient and 3 point lights (RGB)**
![thengine in action, dark grey ambient and 3 moving point lights](https://i.imgur.com/23f7hjz.png)


---

## Strategic Goals & Key Drivers

The development of `thengine` is steered by three major pillars:

1. **Explore Agentic Software Development:** To deeply understand and push the limits of AI-driven coding by executing a concrete, large-scale, and tightly constrained software project.
2. **Maintain Architectural Rigor:** Keep the engine subsystem design clean, modular, highly reusable, and extendable without relying on "vibe-coding".
3. **Familiar & Clean API:** Ensure the public interface remains elegant, type-safe (C++20), intuitive, and easy to grasp for anyone with XNA/MonoGame background.

---

## Core Features

* **Sprite Batching & Rendering:** Full 2D transformations supported (position, rotation, zoom). Powered by an optimized, thread-safe **Asynchronous Dynamic Ring-Buffer (Triple-Buffering)** pipeline that eliminates GPU race conditions while maintaining high-throughput rendering.
* **Deferred Batch Architecture:** Features a robust state-snapshotting system within `Renderer::drawBatched`. This ensures independent lighting and material contexts (e.g., world-space elements vs. screen-space HUD) can be queued sequentially without state-overwrite side effects.
* **Dynamic 2D Lighting MVP:** Built-in ambient light model (color and master intensity controls) alongside support for up to **10 independent dynamic 2D point lights** per scene, complete with smooth distance attenuation math calculated directly in SPIR-V fragment shaders. Supports Raycast lighting and shadows.
* **Decoupled Input System:** Features a custom hardware-agnostic Input Handling layer that abstracts raw SDL3 key events into independent engine commands, allowing clean gameplay code decoupled from the underlying platform.
* **Engine-Level Abstraction API:** Strictly isolates native SDL3 and Vulkan/GPU driver structures behind clean, engine-managed interfaces, protecting core game logic from backend library changes.
* **Modern Text System:** True UTF-8 Unicode rendering with full support for Finnish vowels (ä, ö, å) and text scaling/rotation/positioning via `std::string_view` and `std::format`.
* **Dynamic 2D Camera:** Fully transformable view space supporting independent translation, scaling, and rotation.
* **View Frustum Culling:** An optimized inverse-matrix culling system that automatically skips rendering objects that fall outside the camera's viewport bounds.
* **Resource Management:** A centralized `ContentManager` API for robust, automated loading of bitmaps, textures, fonts, and custom shaders.

---

## Quick Start (Linux)

To generate build files and compile the engine along with the sandbox test application, run:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build