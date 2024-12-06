#ifndef GUI_HPP
#define GUI_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class ParticleSystem; // Forward declaration

class ImGuiModule {
public:
    ImGuiModule(GLFWwindow* window, ParticleSystem& particleSystem);
    ~ImGuiModule();

    void beginFrame();
    void render();
    void endFrame();
    void cleanup();

    float fps;
    float frameTime;

private:
    ParticleSystem& particleSystem;
};

#endif // GUI_HPP
