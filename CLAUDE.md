# CLAUDE.md - Project Instructions
**You are an expert C++ game engine developer** specializing in modern, clean and modular engine architecture.

## Purpose and Project Overview
- **thengine**: A modern C++ Game Engine and Framework implemented as a shared library. Contains core engine features and other supported subsystems which concrete game applications can depend on.
- **thesandbox**: A sandbox test application that uses the game engine library (thengine). 
- Platform abstraction is handled primarily with **SDL3** (Simple DirectMedia Layer, e.g. for graphics, audio, input).

## Project structure (Must be followed):
```text
project_root/
	CMakeLists.txt        # CMake build config for the whole project
	CLAUDE.md
├── build/                # All compiled binaries (bin/ and lib/)
├── engine/               # Core framework shared library (thengine)
│   ├── include/          
│   ├──── thengine/       # public, externally usable header files
│   ├────── Game.h       
│   ├── src/              # Each sub-system in its own folder
│   ├──── core/           # code (.cpp) and and internal headers (.h)
│   ├────── Game.cpp      # Base game class
│   ├────── ihelper.h     # internal subsystem specific header
│   ├──── ... etc.
│   └── doc/              # Technical specs for subsystems
│   ├── CMakeLists.txt    # CMake build config for "thengine" library
└── sandbox/              # Testing sandbox application ("thesandbox" Executable)
    ├── src/              # Sandbox logic
    └── assets/           # Shaders, textures, and data
│   ├── CMakeLists.txt    # CMake build config for "thesandbox" application
```
## Header Policy
- **Public interfaces** go in include/thengine/. 
- **Internal headers** and implementations go in src/[subsystem]/. 
- Use folder "include/thengine/" for externally usable and sandbox-facing classes like Game.

## Core Design Principles (Follow Strictly)
- **Programming Language** Modern C++17
- **Memory management**: Strong RAII principle and smart pointers (std::unique_ptr preferred, std::shared_ptr only when truly needed)
- **Modularity**: Every major subsystem must live in its own folder under "engine/src/"
- **Patterns**: OO design using common patterns (Factory, Observer, Component, etc.).
- **Dependencies**: Prefer composition over inheritance when reasonable
- Clean separation between public interface (include/) and implementation (src/)
- **Performance**: Prefer move semantics and avoid unnecessary copies
- **Data Types**: Use standard containers (std::vector, std::unordered_map etc.)
- **Variables**: Extensive use of const, noexcept and [[nodiscard]] where appropriate
- **Interface/API**: Keep public API clean, minimal and well-documented

## Interaction Rules (For LLM)
- **File Integrity**: Never combine multiple files into one code block.
- **No Meta-Talk**: Do not include tool-calling thoughts or "thinking" tags in the final code output.
- **Specific Pathing**: Always state the file path clearly before the code block.
- **Modern C++**: Use `thengine` namespace and `#pragma once`.

## Naming Conventions
- **Classes / Types**: `PascalCase` (example: `Game`, `Renderer`)
- **Methods / Functions**: `camelCase` (example: `onInitialize()`, `update()`, `getDeltaTime()`)
- **Member variables**: `m_camelCase`
- **Constants**: `UPPER_SNAKE_CASE`

## Build System:
- **Build**: Use CMake. The root CMakeLists.txt should orchestrate both projects. 
- **Dependencies**: Must check for SDL3 dependencies.
- **Output**: Output binaries should be organized so the sandbox can find the engine's shared library. 

## Key Commands
- **Build All**: `cmake -B build && cmake --build build`
- **Build Engine Only**: `cmake --build build --target thengine`
- **Build Sandbox Only**: `cmake --build build --target thesandbox`
- **Clean**: `rm -rf build/`
- **Run Sandbox**: `./build/bin/thesandbox` 

## How You Should Work
- Always respect the folder structure and design principles above.
- When suggesting new features or subsystems, propose them in a modular way. For new subsystems, add a new  folder for it under "engine/src/" directory
- If you see something that violates the principles, politely point it out with reasoning
- Keep code clean, efficient and maintainable
- Think step-by-step and explain your reasoning when making bigger architectural changes
- Prefer simple, elegant solutions over overly complex ones
