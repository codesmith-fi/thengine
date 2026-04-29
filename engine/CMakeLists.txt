# Create the shared library target
add_library(thengine SHARED "")

# Define your subsystems folders
target_sources(thengine PRIVATE
    src/core/Game.cpp
)

# Export headers so the sandbox can #include them
target_include_directories(thengine PUBLIC include)

# Link SDL3 to the engine
target_link_libraries(thengine PRIVATE SDL3::SDL3)
