#include "Game.hpp"
#include "meshes/PyramidMesh.hpp"

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
    m_pyramideTransform.rotation.y += dt;
}

void Game::render() {
    renderer().draw(*m_pyramideRenderMesh, m_pyramideTransform, m_camera);
}
