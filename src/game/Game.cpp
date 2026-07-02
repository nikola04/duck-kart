#include "Game.hpp"
#include "meshes/PyramidMesh.hpp"
#include "../engine/assets/GLTFLoader.hpp"

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

    engine::Mesh kartMesh = engine::GLTFLoader::loadMesh("assets/models/kart.glb");
    engine::RenderMesh* kart = m_assets.createRenderMesh("kart", kartMesh);

    m_objects.push_back(engine::RenderObject {
        .mesh = kart,
    });
}

void Game::update(float dt){
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
    for(const auto& object : m_objects)
        renderer().draw(*object.mesh, object.transform, m_camera);
}
