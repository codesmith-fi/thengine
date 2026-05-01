# Create the executable
add_executable(thesandbox src/main.cpp src/SandboxGame.cpp)

# Link to your custom engine
target_link_libraries(thesandbox PRIVATE thengine)

# Copy assets to build directory on every build
add_custom_command(TARGET thesandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:thesandbox>/assets)
