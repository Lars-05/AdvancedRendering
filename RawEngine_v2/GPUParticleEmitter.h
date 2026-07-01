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
    void SetSize(glm::vec2, glm::vec2);
    void SetAlpha(float, float);
    void SetLifetime(float);
    void Update(float dt);
    void Render(const glm::mat4& projection, const glm::mat4& view);
    void Debug();

private:


    float startGravity;
    float startAlpha;
    glm::vec2 startSize = glm::vec2(1,1);
    glm::vec2 endSize = glm::vec2(1,1);
    float endGravity;
    float endAlpha;
    float lifetime;

    GLuint ssbo = 0;

    GLuint computeProgram = 0;
    GLuint renderProgram = 0;

    GLuint quadVAO = 0;

    core::Texture* particleTexture = nullptr;



    std::vector<GPUParticle> particles;



    float emissionRate = 20.0f;
    float elapsedTime = 0.0f;
};