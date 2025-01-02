cmake_minimum_required(VERSION 3.10)
project(playground C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

find_package(glfw3 3.4 REQUIRED)

# Add your executable
add_executable(playground 
    src/main.c
    external/glad/src/glad.c
)

# Include directories
target_include_directories(playground PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/cglm/include
)

# Link against GLFW
target_link_libraries(playground glfw m)

# Create a directory for shaders in the build tree
set(SHADER_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_DIR})

# Copy shader files to the build directory
file(GLOB SHADER_FILES "shaders/*.glsl")
foreach(shader_file ${SHADER_FILES})
    file(COPY ${shader_file} DESTINATION ${SHADER_DIR})
endforeach()

add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
)

add_dependencies(playground copy_shaders)