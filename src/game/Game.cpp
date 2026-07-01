#include "Game.hpp"
#include "meshes/TriangleMesh.hpp"

Game::Game()
    : engine::Application(),
      m_triangleMesh(game::createTriangleMesh()),
      m_triangleRenderMesh(std::make_unique<engine::RenderMesh>(
          renderer().createRenderMesh(m_triangleMesh)))
{
}

void Game::update(float){
}

void Game::render() {
    renderer().draw(*m_triangleRenderMesh);
}
