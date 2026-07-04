#include "Game.hpp"
#include <SDL3/SDL_scancode.h>
#include <iterator>

Game::Game(): engine::Application(), m_assets(renderer()), m_scene() {
    engine::Transform transform{};
    transform.scale = { 50.0f,50.0f,50.0f};
    engine::RenderModel kartModel = m_assets.loadModel("assets/models/sample.glb", transform);
    m_scene.objects.insert(m_scene.objects.end(), std::make_move_iterator(kartModel.objects.begin()), std::make_move_iterator(kartModel.objects.end()));


    engine::Transform transform2{};
    transform2.scale = { 50.0f,50.0f,50.0f};
    transform2.position.z = 10.0f;
    engine::RenderModel kartModel2 = m_assets.loadModel("assets/models/sample.glb", transform2);
    m_scene.objects.insert(m_scene.objects.end(), std::make_move_iterator(kartModel2.objects.begin()), std::make_move_iterator(kartModel2.objects.end()));
}

void Game::update(float dt){
    float speed = 3.0f;
    const float mouseSensitivity = 0.0025f;
    const float maxPitch = glm::radians(89.0f);

    // mouse movement
    m_scene.camera.transform.rotation.y += input().mouseDeltaX() * mouseSensitivity;
    m_scene.camera.transform.rotation.x -= input().mouseDeltaY() * mouseSensitivity;
    m_scene.camera.transform.rotation.x = std::clamp(m_scene.camera.transform.rotation.x, -maxPitch, maxPitch);

    if (input().isKeyDown(SDL_SCANCODE_LCTRL))
        speed = 8.0f;
    // keyboard movement
    if (input().isKeyDown(SDL_SCANCODE_W))
        m_scene.camera.transform.position += m_scene.camera.transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_S))
        m_scene.camera.transform.position -= m_scene.camera.transform.forward() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_A))
        m_scene.camera.transform.position -= m_scene.camera.transform.right() * speed * dt;
    if (input().isKeyDown(SDL_SCANCODE_D))
        m_scene.camera.transform.position += m_scene.camera.transform.right() * speed * dt;

    if (input().isKeyDown(SDL_SCANCODE_ESCAPE))
        quit();
}

void Game::render() {
    renderer().render(m_scene);
}
