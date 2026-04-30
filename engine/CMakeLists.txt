# Create the shared library target
find_package(SDL3_image REQUIRED)
add_library(thengine SHARED "")

# Define your subsystems folders
target_sources(thengine PRIVATE
    src/core/Game.cpp
    src/core/Entity.cpp
    src/core/ContentManager.cpp
    src/input/Input.cpp
    src/graphics/Renderer.cpp
    src/graphics/Sprite.cpp
    src/graphics/Texture.cpp
)

# Export headers so the sandbox can #include them
target_include_directories(thengine 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE 
        src  # Allows engine files to #include "core/InternalHelper.hpp"
)
# Link SDL3 to the engine
target_link_libraries(thengine PRIVATE SDL3::SDL3 SDL3_image::SDL3_image-shared)
