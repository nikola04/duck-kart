#include "Game.hpp"
#include "meshes/PyramidMesh.hpp"
#include "../engine/assets/GLTFLoader.hpp"
#include <SDL3/SDL_scancode.h>
#include <iostream>

Game::Game(): engine::Application(), m_assets(renderer()), m_camera(), m_objects() {
    engine::Mesh pyramidMesh = game::createPyramidMesh();
    // engine::RenderMesh* pyramid = m_assets.createRenderMesh("pyramid", pyramidMesh);

    // m_objects.push_back({
    //     .mesh = pyramid
    // });

    // m_objects.push_back(engine::RenderObject {
    //     .mesh = pyramid,
    //     .transform = { .position = {5.0f, 1.0f, 1.0f} }
    // });

    engine::LoadedModel kartModel = engine::GLTFLoader::loadModel("assets/models/kart.glb");

    for (const auto& loadedTexture : kartModel.textures) {
        auto texture = std::make_unique<engine::Texture>(renderer().createTexture(loadedTexture.pixels.data(), loadedTexture.width, loadedTexture.height));
        m_textures.push_back(std::move(texture));
    }

    for (std::size_t i = 0; i < kartModel.meshes.size(); ++i) {
        const auto& loadedMesh = kartModel.meshes[i];
        engine::Mesh mesh = loadedMesh.mesh;

        engine::RenderMesh* renderMesh = m_assets.createRenderMesh("kart_mesh_" + std::to_string(i), mesh);

        const engine::Texture* texture = nullptr;

        if (loadedMesh.material >= 0) {
            const auto& material = kartModel.materials[loadedMesh.material];

            if (material.baseColorTexture >= 0) {
                texture = m_textures[material.baseColorTexture].get();
            }
        }

        m_objects.push_back(engine::RenderObject{
            .mesh = renderMesh,
            .texture = texture
        });
    }
}

void Game::update(float dt){
    float speed = 3.0f;
    const float mouseSensitivity = 0.0025f;
    const float maxPitch = glm::radians(89.0f);

    // mouse movement
    m_camera.transform.rotation.y += input().mouseDeltaX() * mouseSensitivity;
    m_camera.transform.rotation.x -= input().mouseDeltaY() * mouseSensitivity;
    m_camera.transform.rotation.x = std::clamp(m_camera.transform.rotation.x, -maxPitch, maxPitch);

    if (input().isKeyDown(SDL_SCANCODE_LCTRL))
        speed = 8.0f;
    // keyboard movement
    if (input().isKeyDown(SDL_SCANCODE_W))
        m_camera.transform.position += m_camera.transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_S))
        m_camera.transform.position -= m_camera.transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_A))
        m_camera.transform.position -= m_camera.transform.right() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_D))
        m_camera.transform.position += m_camera.transform.right() * speed * dt;
}

void Game::render() {
    for(const auto& object : m_objects)
        renderer().draw(*object.mesh, object.transform, m_camera, object.texture);
}
