#include "particle_system.hpp"
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "glad/glad.h"
#include "shader.hpp"
#include "util.hpp"

ParticleSystem::ParticleSystem(float pps, float averageSpeed, float gravityEffect,
                               float averageLifeLength, float averageScale)
    : pps(pps), averageSpeed(averageSpeed), gravityEffect(gravityEffect),
      averageLifeLength(averageLifeLength), averageScale(averageScale),
      randomEngine(std::random_device{}()), randomDist(0.0f, 1.0f),
      turbulenceScale(1.0f), turbulenceStrength(0.5f)
{
   float quadVertices[] = {
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom-left
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom-right
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, // Top-left
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f  // Top-right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    particles.resize(1000);
}

void ParticleSystem::update(float deltaTime, const glm::vec3& cameraPosition)
{
    for (auto& particle : particles) {
        if (particle.isActive()) {
            particle.update(deltaTime, cameraPosition);
        }
    }

    std::sort(particles.begin(), particles.end(), [&cameraPosition](const Particle& a, const Particle& b) {
        return glm::distance(cameraPosition, a.getPosition()) > glm::distance(cameraPosition, b.getPosition());
    });

    emitParticles(glm::vec3(0.0f, 0.1f, 0.0f), deltaTime);
}

void ParticleSystem::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Shader& shader)
{
    shader.use();

    shader.setMat4("projection", projectionMatrix);
    shader.setMat4("view", viewMatrix);

    glBindVertexArray(quadVAO);

    for (const Particle& particle : particles)
    {
        if (!particle.isActive())
            continue;

        glm::vec3 position = particle.getPosition();
        float scale = particle.getScale();
        float rotation = particle.getRotation();
        float lifeFactor = particle.getLifeFactor();

        shader.setVec3("particlePosition", position);
        shader.setFloat("particleScale", scale);
        shader.setFloat("particleRotation", rotation);

        shader.setInt("currentTextureIndex", particle.getCurrentTextureIndex());
        shader.setInt("nextTextureIndex", particle.getNextTextureIndex());
        shader.setFloat("blendFactor", particle.getBlendFactor());
        shader.setInt("textureRows", textureRows);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindVertexArray(0);
    shader.unuse();
}

void ParticleSystem::emitParticles(const glm::vec3& position, float deltaTime)
{
    float particlesToCreate = pps * deltaTime;
    int count = static_cast<int>(std::floor(particlesToCreate));
    float partialParticle = particlesToCreate - count;

    for (int i = 0; i < count; ++i) {
        emitParticle(position);
    }
}

void ParticleSystem::emitParticle(const glm::vec3& position)
{
    glm::vec3 velocity;

    if (glm::length(direction) > 0.0f) {
        velocity = generateRandomUnitVectorWithinCone(direction, directionDeviation);
    } else {
        velocity = generateRandomUnitVector();
    }

    velocity = glm::normalize(velocity);
    float speed = generateValue(averageSpeed, speedError);
    velocity *= speed;

    float scale = generateValue(averageScale, scaleError);
    float lifeLength = generateValue(averageLifeLength, lifeError);
    float rotation = randomRotation ? randomDist(randomEngine) * 360.0f : 0.0f;

    for (auto& particle : particles) {
        if (!particle.isActive()) {
            particle.activate(position, velocity, gravityEffect * util::GRAVITY, lifeLength, rotation, scale, textureRows, turbulenceScale, turbulenceStrength);
            return;
        }
    }

    particles.emplace_back();
    particles.back().activate(position, velocity, gravityEffect * util::GRAVITY, lifeLength, rotation, scale, textureRows, turbulenceScale, turbulenceStrength);
}

float ParticleSystem::generateValue(float average, float errorMargin)
{
    float offset = (randomDist(randomEngine) - 0.5f) * 2.0f * errorMargin;
    return average + offset;
}

glm::vec3 ParticleSystem::generateRandomUnitVector()
{
    float theta = randomDist(randomEngine) * 2.0f * glm::pi<float>();
    float z = (randomDist(randomEngine) * 2.0f) - 1.0f;
    float rootOneMinusZSquared = sqrtf(1 - z * z);
    float x = rootOneMinusZSquared * cosf(theta);
    float y = rootOneMinusZSquared * sinf(theta);
    return glm::vec3(x, y, z);
}

glm::vec3 ParticleSystem::generateRandomUnitVectorWithinCone(const glm::vec3& coneDirection, float angle)
{
    float cosAngle = cosf(angle);
    float theta = randomDist(randomEngine) * 2.0f * glm::pi<float>();
    float z = cosAngle + randomDist(randomEngine) * (1 - cosAngle);
    float rootOneMinusZSquared = sqrtf(1 - z * z);
    float x = rootOneMinusZSquared * cosf(theta);
    float y = rootOneMinusZSquared * sinf(theta);

    glm::vec4 direction(x, y, z, 1.0f);

    if (coneDirection != glm::vec3(0.0f, 0.0f, 1.0f) && coneDirection != glm::vec3(0.0f, 0.0f, -1.0f)) {
        glm::vec3 rotateAxis = glm::cross(coneDirection, glm::vec3(0.0f, 0.0f, 1.0f));
        rotateAxis = glm::normalize(rotateAxis);
        float rotateAngle = glm::acos(glm::dot(coneDirection, glm::vec3(0.0f, 0.0f, 1.0f)));

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -rotateAngle, rotateAxis);
        direction = rotationMatrix * direction;
    } else if (coneDirection == glm::vec3(0.0f, 0.0f, -1.0f)) {
        direction.z *= -1.0f;
    }

    return glm::vec3(direction);
}

void ParticleSystem::setDirection(const glm::vec3& direction, float deviation)
{
    this->direction = glm::normalize(direction);
    this->directionDeviation = deviation;
}

void ParticleSystem::randomizeRotation()
{
    randomRotation = true;
}

void ParticleSystem::setSpeedError(float error)
{
    this->speedError = error * averageSpeed;
}

void ParticleSystem::setLifeError(float error)
{
    this->lifeError = error * averageLifeLength;
}

void ParticleSystem::setScaleError(float error)
{
    this->scaleError = error * averageScale;
}

float ParticleSystem::getPPS() const { return pps; }
float ParticleSystem::getAverageSpeed() const { return averageSpeed; }
float ParticleSystem::getGravityEffect() const { return gravityEffect; }
float ParticleSystem::getAverageLifeLength() const { return averageLifeLength; }
float ParticleSystem::getAverageScale() const { return averageScale; }
float ParticleSystem::getSpeedError() const { return speedError / averageSpeed; } 
float ParticleSystem::getLifeError() const { return lifeError / averageLifeLength; }
float ParticleSystem::getScaleError() const { return scaleError / averageScale; }
bool ParticleSystem::isRandomRotation() const { return randomRotation; }
glm::vec3 ParticleSystem::getDirection() const { return direction; }
float ParticleSystem::getDirectionDeviation() const { return directionDeviation; }

void ParticleSystem::setPPS(float pps) { this->pps = pps; }
void ParticleSystem::setAverageSpeed(float speed) { this->averageSpeed = speed; }
void ParticleSystem::setGravityEffect(float gravityEffect) { this->gravityEffect = gravityEffect; }
void ParticleSystem::setAverageLifeLength(float lifeLength) { this->averageLifeLength = lifeLength; }
void ParticleSystem::setTextureRows(unsigned int rows) { textureRows = rows; }
void ParticleSystem::setAverageScale(float scale) { this->averageScale = scale; }
void ParticleSystem::disableRandomRotation() { randomRotation = false; }
