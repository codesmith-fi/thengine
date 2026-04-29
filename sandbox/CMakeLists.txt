# Create the executable
add_executable(thesandbox src/main.cpp)

# Link to your custom engine
target_link_libraries(thesandbox PRIVATE thengine)

# Copy assets to build directory on every build
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
