Purpose:
A Game Engine Framework library and a separate sandbox testing application depending on the shared Game Engine Framework library. The game engine will be implemented as shared library usable in game projects. The project uses the open source SDL3 library (Simple DirectMedia Layer) for basic graphics and audio (+ other) features and builds on top of it. 

Folder structure, "thengine" for the shared library and "thesandbox" for the testing sandbox application linking to the shared library:
	thengine/
		src/
		doc/
	thesandbox/
		src
		assets/
		doc/

Design practises:
- We will use object oriented design applying common practical design patterns and idioms. 
- The game engine is to be designed as extendable, reusable and easily maintainable. 
- Separate each sub-system of the game engine into separate subfolders, for example: 
	thengine/
		src/
			primitives/
			core/
			physics/
			particles/
			...

Programming language and environment:
- This project uses modern C++, at least C++17. 
- Stick to modern C++ features but aim for efficiency. Use automatic resource management (e.g. smart pointers) and modern C++ data structures/containers where suitable.
- Create CMake compatible build which checks for required dependencies (e.g. SDL3 libraries)



Purpose:
A modern C++ Game Engine Framework (thengine) implemented as a shared library, and a separate sandbox application (thesandbox) that consumes it. The project leverages SDL3 for core platform abstraction (graphics, audio, input).

Project Structure, folders:
	CLAUDE.md
	build/: All compiled binaries
	thengine/: Core framework (Shared Library).
		include/: Public headers
		src/: Internal headers and sources categorized by subsystem (e.g., core/, physics/, particles/).
		doc/: Technical specs for subsystems.
	thesandbox/: Testing environment (Executable).
		src/: Sandbox logic.
		assets/: Shaders, textures, and data.

Design Practices:
	- Patterns: OO design using common patterns (Factory, Observer, Component, etc.).
	- Modularity: Every engine subsystem must reside in its own subfolder within thengine/src/.
	- C++ Standard: Modern C++17.
	- Resource Management: Prefer RAII and smart pointers (std::unique_ptr, std::shared_ptr) over raw new/delete. 
	- Efficiency: Use standard containers (std::vector) and avoid unnecessary copies. 

Build System (Required):
	- Use CMake. The root CMakeLists.txt should orchestrate both projects.
	- Must check for SDL3 dependencies.
	- Output binaries should be organized so the sandbox can find the engine's shared library. 

## Key Commands
- **Build All**: `cmake -B build && cmake --build build`
- **Build Engine Only**: `cmake --build build --target thengine`
- **Build Sandbox Only**: `cmake --build build --target thesandbox`
- **Clean**: `rm -rf build/`
- **Run Sandbox**: `./build/thesandbox/thesandbox` 
