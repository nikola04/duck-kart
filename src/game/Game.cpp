#include "Game.hpp"
#include "meshes/PyramidMesh.hpp"
#include <iomanip>
#include <iostream>

Game::Game()
    : engine::Application(),
        m_camera(),
        m_pyramideMesh(game::createPyramidMesh()),
        m_pyramideRenderMesh(std::make_unique<engine::RenderMesh>(
            renderer().createRenderMesh(m_pyramideMesh))) {
                m_pyramideTransform.position = {0.0f, 0.0f, 0.0f};
                m_pyramideTransform.rotation = {0.0f, 0.0f, 0.0f};
                m_pyramideTransform.scale = {1.0f, 1.0f, 1.0f};

                m_camera.transform.position = {0.0f, 0.0f, 1.5f};
                m_camera.fov = 70.0f;
                m_camera.near_plane = 0.1f;
                m_camera.far_plane = 100.0f;
            }

void Game::update(float dt){
    const float fps = 1.0f / dt;
    std::cout << "\rFPS: " << std::fixed << std::setprecision(1) << std::setw(6) << fps << std::flush;

    const float speed = 3.0f;
    const float mouseSensitivity = 0.0025f;
    const float maxPitch = glm::radians(89.0f);

    // mouse movement
    m_camera.transform.rotation.y += input().mouseDeltaX() * mouseSensitivity;
    m_camera.transform.rotation.x -= input().mouseDeltaY() * mouseSensitivity;
    m_camera.transform.rotation.x = std::clamp(m_camera.transform.rotation.x, -maxPitch, maxPitch);

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
    renderer().draw(*m_pyramideRenderMesh, m_pyramideTransform, m_camera);
}
