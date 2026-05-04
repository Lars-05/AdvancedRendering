//
// Created by Lars on 4-5-2026.
//
#pragma once

#include <vector>

#include "Particle.h"


class ParticleEmitter {
    public:
    const core::Model& baseModel;
    core::Texture* texture;
    std::vector<Particle*> particles;
    const GLuint program;
    float emitRate;

    ParticleEmitter(const core::Model& pBaseModel, core::Texture* pTexture, GLint particleProgram);
    ~ParticleEmitter();
    void EmitNewParticle();
    void SetVelocity(glm::vec2 vel);
    void UpdateParticles(float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view);
    void DebugParticles() const;
};
