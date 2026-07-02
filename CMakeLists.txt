cmake_minimum_required(VERSION 3.20)

project(3DGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(glm REQUIRED)

add_executable(main
    src/main.cpp

    src/engine/core/Application.cpp
    src/engine/core/SDLContext.cpp
    src/engine/graphics/Renderer.cpp
    src/engine/graphics/Mesh.cpp
    src/engine/graphics/RenderMesh.cpp
    src/engine/graphics/Shader.cpp
    src/engine/math/Transform.cpp
    src/engine/window/Window.cpp

    src/game/Game.cpp
    src/game/meshes/PyramidMesh.cpp
)

target_link_libraries(main PRIVATE SDL3::SDL3 glm::glm)
