#include "Game.hpp"
#include <SDL3/SDL_scancode.h>
#include <glm/fwd.hpp>

Game::Game(): engine::Application(), m_assets(renderer()), m_scene() {
    // engine::PointLight light{
    //     .position = {0.0f, 5.0f, 5.0f, 1.0f},
    //     .color = {1.0f, 0.9f, 0.7f, 1.0f},
    //     .properties = {5.0f, 15.0f, 0.0f, 0.0f}
    // };
    // m_scene.pointLights.push_back(light);

    // engine::Transform bulbTransform;
    // bulbTransform.position = glm::vec3(light.position);
    // bulbTransform.scale = {0.2f, 0.2f, 0.2f};
    // m_scene.addModel(m_assets.loadModel("assets/models/sphere.glb", bulbTransform));

    // engine::Transform transform{};
    // transform.scale = { 50.0f, 50.0f, 50.0f};
    // m_scene.addModel(m_assets.loadModel("assets/models/sample.glb", transform));


    m_scene.camera.fov = 90;
    m_scene.camera.transform.position.y = 1.0f;
    m_scene.skybox.cubemap = m_assets.loadCubemap("assets/skyboxes/day");
    m_scene.addModel(m_assets.loadModel("assets/models/drift_race_track.glb", {}));
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
        speed = 30.0f;
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
