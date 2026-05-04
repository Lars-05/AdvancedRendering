//
// Created by Lars on 4-5-2026.
//

#include "ParticleEmitter.h"

#include <iostream>

ParticleEmitter::ParticleEmitter(const core::Model& pBaseModel, core::Texture* pTexture, GLint particleProgram) : baseModel(
    pBaseModel), texture(pTexture), program(particleProgram), emitRate(1.0) {
}

ParticleEmitter::~ParticleEmitter() {
    for (Particle* particle:  particles) {
        delete particle;
    }
    particles.clear();
}

void ParticleEmitter::EmitNewParticle() {
    particles.push_back(new Particle(baseModel, *texture, static_cast<GLint>(program)));
    printf("Emitted A new Particle");
}
void ParticleEmitter::SetVelocity(glm::vec2 pVelocity) {
    for (Particle* particle:  particles) {
      particle->SetVelocity(pVelocity);
    }
}

void ParticleEmitter::UpdateParticles(const float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view) {
    //emission and stuff

    for (Particle* particle:  particles) {
        particle->Update(pDeltaTime, projection, view);
    }
}

void ParticleEmitter::DebugParticles() const {
    for (Particle* particle:  particles) {
        const float x = particle->GetTransform().GetPosition().x;
        const float y = particle->GetTransform().GetPosition().y;
        std::cout << "(" << x << ", " << y << ")" << std::endl;

    }
}
