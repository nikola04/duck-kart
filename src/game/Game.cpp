#include "Game.hpp"
#include "meshes/PyramidMesh.hpp"

Game::Game()
    : engine::Application(),
      m_pyramideMesh(game::createPyramidMesh()),
      m_pyramideRenderMesh(std::make_unique<engine::RenderMesh>(
          renderer().createRenderMesh(m_pyramideMesh))) {
              m_pyramideTransform.position = {0.0f, 0.0f, 0.0f};
              m_pyramideTransform.rotation = {0.0f, 0.0f, 0.0f};
              m_pyramideTransform.scale = {1.0f, 1.0f, 1.0f};
          }

void Game::update(float){
}

void Game::render() {
    // renderer().draw(*m_render_mesh);
    renderer().draw(*m_pyramideRenderMesh, m_pyramideTransform);
}
