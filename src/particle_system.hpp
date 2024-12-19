#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

#include "particle.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <random>
#include <vector>

class Shader;

class ParticleSystem {
public:
  ParticleSystem(float pps, float averageSpeed, float gravityEffect,
                 float averageLifeLength, float averageScale);

  void update(float deltaTime, const glm::vec3 &cameraPosition);
  void render(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix,
              Shader &shader);

  void setDirection(const glm::vec3 &direction, float deviation);
  void randomizeRotation();
  void setSpeedError(float error);
  void setLifeError(float error);
  void setScaleError(float error);

  void emitParticles(const glm::vec3 &position, float deltaTime);

private:
  void emitParticle(const glm::vec3 &position);

  float generateValue(float average, float errorMargin);
  glm::vec3 generateRandomUnitVector();
  glm::vec3 generateRandomUnitVectorWithinCone(const glm::vec3 &coneDirection,
                                               float angle);

  std::vector<Particle> particles;

  float pps;
  float averageSpeed;
  float gravityEffect;
  float averageLifeLength;
  float averageScale;
  float turbulenceScale;
  float turbulenceStrength;

  float speedError = 0.0f;
  float lifeError = 0.0f;
  float scaleError = 0.0f;
  bool randomRotation = false;
  glm::vec3 direction = glm::vec3(0.0f);
  float directionDeviation = 0.0f;

  unsigned int textureRows;

  std::mt19937 randomEngine;
  std::uniform_real_distribution<float> randomDist;

  GLuint quadVAO;
  GLuint quadVBO;

public:
  float getPPS() const;
  float getAverageSpeed() const;
  float getGravityEffect() const;
  float getAverageLifeLength() const;
  float getAverageScale() const;
  float getSpeedError() const;
  float getLifeError() const;
  float getScaleError() const;
  bool isRandomRotation() const;
  glm::vec3 getDirection() const;
  float getDirectionDeviation() const;
  float getTurbulenceStrength() const;
  float getTurbulenceScale() const;

  void setPPS(float pps);
  void setAverageSpeed(float speed);
  void setGravityEffect(float gravityEffect);
  void setAverageLifeLength(float lifeLength);
  void setAverageScale(float scale);
  void setTurbulenceStrength(float turbulenceStrength);
  void setTurbulenceScale(float turbulenceScale);
  void disableRandomRotation();
  void setTextureRows(unsigned int rows);
};

#endif // PARTICLE_SYSTEM_HPP
