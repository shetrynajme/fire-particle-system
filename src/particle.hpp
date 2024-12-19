#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <glm/glm.hpp>

class Particle {
public:
  void activate(const glm::vec3 &position, const glm::vec3 &velocity,
                float gravityEffect, float lifeLength, float rotation,
                float scale, unsigned int textureRows, float turbulenceScale,
                float turbulenceStrength);

  bool update(float deltaTime, const glm::vec3 &cameraPosition);

  bool isActive() const;
  const glm::vec3 &getPosition() const;
  float getRotation() const;
  float getScale() const;
  float getLifeFactor() const;
  unsigned int getCurrentTextureIndex() const;
  unsigned int getNextTextureIndex() const;
  float getBlendFactor() const;

private:
  glm::vec3 position;
  glm::vec3 velocity;
  float gravityEffect;
  float lifeLength;
  float rotation;
  float scale;

  float turbulenceScale;
  float turbulenceStrength;

  float elapsedTime;
  bool active;

  unsigned int textureRows;
  float blendFactor;
  unsigned int currentTextureIndex;
  unsigned int nextTextureIndex;
};

#endif // PARTICLE_HPP
