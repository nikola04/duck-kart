cmake_minimum_required(VERSION 3.20)

project(DuckKart)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
find_package(glm REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

add_executable(main
    src/main.cpp

    src/engine/core/Application.cpp
    src/engine/core/SDLContext.cpp
    src/engine/core/Input.cpp
    src/engine/graphics/Renderer.cpp
    src/engine/graphics/Mesh.cpp
    src/engine/graphics/renderer/RenderMesh.cpp
    src/engine/graphics/renderer/RendererGeometry.cpp
    src/engine/graphics/renderer/RendererResources.cpp
    src/engine/graphics/renderer/SceneRenderer.cpp
    src/engine/graphics/renderer/ShadowRenderer.cpp
    src/engine/graphics/renderer/UIRenderer.cpp
    src/engine/graphics/Shader.cpp
    src/engine/graphics/GPUTexture.cpp
    src/engine/graphics/Texture.cpp
    src/engine/graphics/TextTexture.cpp
    src/engine/graphics/GraphicsPipeline.cpp
    src/engine/graphics/ShadowMap.cpp
    src/engine/graphics/ShadowCamera.cpp
    src/engine/graphics/Font.cpp
    src/engine/math/Transform.cpp
    src/engine/math/Math.cpp
    src/engine/math/Frustum.cpp
    src/engine/scene/Camera.cpp
    src/engine/window/Window.cpp
    src/engine/assets/AssetsManager.cpp
    src/engine/assets/GLTFLoader.cpp

    src/game/Game.cpp
    src/game/meshes/PyramidMesh.cpp
)

target_link_libraries(main PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf glm::glm)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
target_include_directories(main PRIVATE ${TINYGLTF_INCLUDE_DIRS})
