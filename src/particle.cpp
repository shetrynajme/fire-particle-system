#include "particle.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

void Particle::activate(const glm::vec3 &position, const glm::vec3 &velocity,
                        float gravityEffect, float lifeLength, float rotation,
                        float scale, unsigned int textureRows,
                        float turbulenceScale, float turbulenceStrength) {
  this->position = position;
  this->velocity = velocity;
  this->gravityEffect = gravityEffect;
  this->lifeLength = lifeLength;
  this->rotation = rotation;
  this->scale = scale;
  this->elapsedTime = 0.0f;
  this->active = true;
  this->textureRows = textureRows;
  this->turbulenceScale = turbulenceScale;
  this->turbulenceStrength = turbulenceStrength;
}

bool Particle::update(float deltaTime, const glm::vec3 &cameraPosition) {
  if (!active)
    return false;

  // bouyancy
  float bouyancyFactor = 1.0f - this->getLifeFactor();
  velocity.y += (gravityEffect + bouyancyFactor * 2.0f) * deltaTime;

  // Add sinusoidal horizontal motion
  float swingAmplitude = 0.1f; // Adjust for how wide the sway is
  float swingFrequency = 2.0f; // Adjust for how fast the sway is
  float swayOffset = sin(elapsedTime * swingFrequency) * swingAmplitude;

  // Modify position based on sinusoidal motion
  position.x += swayOffset * deltaTime; // Sway on X-axis
  position.z += cos(elapsedTime * swingFrequency) * swingAmplitude * deltaTime;

  float noiseX =
      glm::perlin(position * turbulenceScale + glm::vec3(elapsedTime)) *
      turbulenceStrength;
  float noiseY = glm::perlin(position * turbulenceScale +
                             glm::vec3(elapsedTime + 100.0f)) *
                 turbulenceStrength;
  float noiseZ = glm::perlin(position * turbulenceScale +
                             glm::vec3(elapsedTime + 200.0f)) *
                 turbulenceStrength;

  glm::vec3 turbulenceForce(noiseX, noiseY, noiseZ);

  velocity += turbulenceForce * deltaTime;

  position += velocity * deltaTime;
  elapsedTime += deltaTime;

  // flickering
  float flickerScale = 0.98f + (rand() % 5) / 1000.0f;
  float flickerRotation = (rand() % 10 - 5) * deltaTime;
  scale *= flickerScale;
  rotation += flickerRotation;

  if (elapsedTime >= lifeLength) {
    active = false; // Deactivate particle
    return false;
  }

  // textures
  float lifeFactor = this->getLifeFactor();
  float totalStages = textureRows * textureRows;
  float atlasProgression = lifeFactor * totalStages;
  currentTextureIndex = static_cast<unsigned int>(floor(atlasProgression));
  nextTextureIndex = currentTextureIndex < totalStages - 1
                         ? currentTextureIndex + 1
                         : currentTextureIndex;
  blendFactor = atlasProgression - currentTextureIndex;

  return true;
}

const glm::vec3 &Particle::getPosition() const { return position; }
float Particle::getRotation() const { return rotation; }
float Particle::getScale() const { return scale; }
float Particle::getLifeFactor() const { return elapsedTime / lifeLength; }
bool Particle::isActive() const { return active; }
unsigned int Particle::getCurrentTextureIndex() const {
  return currentTextureIndex;
}
unsigned int Particle::getNextTextureIndex() const { return nextTextureIndex; }
float Particle::getBlendFactor() const { return blendFactor; }
