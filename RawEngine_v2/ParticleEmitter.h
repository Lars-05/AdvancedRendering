//
// Created by Lars on 4-5-2026.
//
#pragma once

#include <vector>

#include "Particle.h"
#include "Random.h"


class ParticleEmitter {
    public:
    const unsigned int maxParticleCount = 1000000;
    const core::Model& baseModel;
    core::Texture* texture;
    std::vector<Particle*> particles;
    const GLuint program;
    float emissionRate;
    Transform transform;
    Random random;


    float  startRotation = 0;
    float  endRotation = 360;

    glm::vec2  startSize = glm::vec2(0.2,0.2);
    glm::vec2 endSize = glm::vec2(0.2,0.2);

    glm::vec2 maxVelocity = glm::vec2(0.2,0.2);
    glm::vec2 minVelocity = glm::vec2(0.2,0.2);
    float startAlpha = 1.0f;
    float endAlpha = 1.0f;

    float startGravity = 2.0f;
    float endGravity = 2.0f;
    float lifetime = 2;

    glm::vec2 finalVelocity;



    ParticleEmitter(const core::Model& pBaseModel, core::Texture* pTexture, GLint particleProgram);
    ~ParticleEmitter();
    void SetGravity(float beginGravity, float endGravity);
    void EmitParticle();
    void SetVelocity(glm::vec2);
    void SetLifetime(float lifetime);
    void UpdateParticles(float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view);
    void SetAlpha(float beginAlpha, float endAlpha);
    void SetSize(glm::vec2 beginSize, glm::vec2 endSize);
    void SetRotation(float, float);
    void SetVelocityMinMax(glm::vec2 min, glm::vec2 max);
    void SetEmissionRate(float pEmissionRate);


};
