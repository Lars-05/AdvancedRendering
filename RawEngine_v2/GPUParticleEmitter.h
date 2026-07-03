#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GPUParticle.h"
#include "Random.h"
#include "core/texture.h"

class GPUParticleEmitter
{
public:
    Random* random = new Random();
    int maxParticlesCount = 10000;
    int aliveCount = 0;
    int deadCount = 0;
    GPUParticleEmitter(GLuint computeProgram,GLuint renderProgram,GLuint quadVAO,core::Texture* texture,int maxParticles);
    ~GPUParticleEmitter();
    void EmitParticle();
    std::vector<GPUParticle> GetAliveParticles(const std::vector<GPUParticle>&);
    void SetEmissionRate(float rate);
    void Initialize();
    void SetGravity(float, float);
    void SetSize(float, float);
    void SetLifetime(float);
    void SetAlpha(float, float);
    void Update(float dt);
    void Render(const glm::mat4& projection, const glm::mat4& view);
    void Debug();

private:


    float startGravity;
    float startAlpha;
    float startSize = 1;
    float endSize = 0.2;
    float endGravity;
    float endAlpha;
    float lifetime = 5;

    GLuint ssbo = 0;

    GLuint computeProgram = 0;
    GLuint renderProgram = 0;

    GLuint quadVAO = 0;

    core::Texture* particleTexture = nullptr;



    std::vector<GPUParticle> particles;



    float emissionRate = 20.0f;
    float elapsedTime = 0.0f;
};