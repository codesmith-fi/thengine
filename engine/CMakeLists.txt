# Create the shared library target
add_library(thengine SHARED "")

# Define your subsystems folders
target_sources(thengine PRIVATE
    src/core/Game.cpp
    src/input/Input.cpp
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
target_link_libraries(thengine PRIVATE SDL3::SDL3)
