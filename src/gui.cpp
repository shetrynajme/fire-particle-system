#include "gui.hpp"

#include <iostream>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "particle_system.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

ImGuiModule::ImGuiModule(GLFWwindow* window,  ParticleSystem& particleSystem)
    : fps(0.0f), frameTime(0.0f), particleSystem(particleSystem) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
    std::cerr << "Failed to initialize ImGui GLFW" << std::endl;
    return;
  }
  if (!ImGui_ImplOpenGL3_Init()) {
    std::cerr << "Failed to initialize ImGui OpenGL3" << std::endl;
    return;
  }
}

ImGuiModule::~ImGuiModule() { cleanup(); }

void ImGuiModule::beginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiModule::render() {
    ImGui::Begin("Debug Information");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.3f ms", frameTime);

    ImGui::Separator();
    ImGui::Text("Particle System Controls");

    static float pps = particleSystem.getPPS();
    if (ImGui::SliderFloat("Particles Per Second", &pps, 0.0f, 5000.0f)) {
        particleSystem.setPPS(pps);
    }

    static float avgSpeed = particleSystem.getAverageSpeed();
    if (ImGui::SliderFloat("Average Speed", &avgSpeed, 0.0f, 20.0f)) {
        particleSystem.setAverageSpeed(avgSpeed);
    }

    static float gravityEffect = particleSystem.getGravityEffect();
    if (ImGui::SliderFloat("Gravity Effect", &gravityEffect, -10.0f, 10.0f)) {
        particleSystem.setGravityEffect(gravityEffect);
    }

    // Average life length
    static float avgLifeLength = particleSystem.getAverageLifeLength();
    if (ImGui::SliderFloat("Average Life Length", &avgLifeLength, 0.1f, 10.0f)) {
        particleSystem.setAverageLifeLength(avgLifeLength);
    }

    static float avgScale = particleSystem.getAverageScale();
    if (ImGui::SliderFloat("Average Scale", &avgScale, 0.1f, 5.0f)) {
        particleSystem.setAverageScale(avgScale);
    }

    static float speedError = particleSystem.getSpeedError();
    if (ImGui::SliderFloat("Speed Error", &speedError, 0.0f, 1.0f)) {
        particleSystem.setSpeedError(speedError);
    }

    static float lifeError = particleSystem.getLifeError();
    if (ImGui::SliderFloat("Life Error", &lifeError, 0.0f, 1.0f)) {
        particleSystem.setLifeError(lifeError);
    }

    static float scaleError = particleSystem.getScaleError();
    if (ImGui::SliderFloat("Scale Error", &scaleError, 0.0f, 1.0f)) {
        particleSystem.setScaleError(scaleError);
    }

    static bool randomRotation = particleSystem.isRandomRotation();
    if (ImGui::Checkbox("Random Rotation", &randomRotation)) {
        if (randomRotation) {
            particleSystem.randomizeRotation();
        } else {
            particleSystem.disableRandomRotation();
        }
    }

    static float direction[3] = { particleSystem.getDirection().x, particleSystem.getDirection().y, particleSystem.getDirection().z };
    if (ImGui::InputFloat3("Direction", direction)) {
        glm::vec3 dir = glm::normalize(glm::vec3(direction[0], direction[1], direction[2]));
        particleSystem.setDirection(dir, particleSystem.getDirectionDeviation());
    }

    static float directionDeviation = particleSystem.getDirectionDeviation();
    if (ImGui::SliderAngle("Direction Deviation", &directionDeviation, 0.0f, 180.0f)) {
        particleSystem.setDirection(glm::normalize(glm::vec3(direction[0], direction[1], direction[2])), directionDeviation);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiModule::endFrame() {}

void ImGuiModule::cleanup() {
  if (ImGui::GetCurrentContext()) {  // Check if the ImGui context exists
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext();
  }
}
