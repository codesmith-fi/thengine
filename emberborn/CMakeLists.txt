# Create the executable
add_executable(emberborn src/main.cpp src/EmberbornGame.cpp)

# Link to your custom engine
target_link_libraries(emberborn PRIVATE thengine)

# Copy assets to build directory on every build
add_custom_command(TARGET emberborn POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:emberborn>/assets)
