//
// Created by Lars on 4-5-2026.
//

#include "ParticleEmitter.h"

#include <iostream>
#include <glm/ext/quaternion_common.hpp>

#include "Random.h"

ParticleEmitter::ParticleEmitter(const core::Model& pBaseModel, core::Texture* pTexture, GLint particleProgram) : baseModel(pBaseModel), texture(pTexture), program(particleProgram), emissionRate(1.0)
{
    for (int i = 0; i < maxParticleCount; i++) {
        Particle* particle = new Particle(baseModel, *texture, program);
        particle->alive = false;
        particles.push_back(particle);
    }
}

ParticleEmitter::~ParticleEmitter() {
    for (Particle* particle:  particles) {
        delete particle;
    }
    particles.clear();
}

void  ParticleEmitter::SetGravity(float value, float endValue) {

    startGravity = value;
    endGravity = endValue;
    for (Particle* particle:  particles) {
        particle->SetGravity(startGravity, endGravity);
    }
}
void ParticleEmitter::EmitParticle() {

    for (Particle* particle : particles)
    {
        if (!particle->alive)
        {
            particle->alive = true;

            particle->SetPosition(transform.GetPosition());

            particle->SetVelocity(
                glm::vec2(
                    random.RandomFloat(-1.0f, 1.0f),
                    random.RandomFloat(2.0f, 5.0f)
                )
            );

            particle->SetRotation(startRotation, endRotation);
            particle->SetSize(startSize, endSize);
            particle->SetLifeTime(lifetime);
            particle->SetGravity(startGravity, endGravity);
            particle->SetAlpha(startAlpha, endAlpha);
            return;
        }
    }

}
void ParticleEmitter::SetVelocity(glm::vec2 pVelocity) {

    for (Particle* particle:  particles) {
      particle->SetVelocity(pVelocity);
    }
}

float elapsedTime;

void ParticleEmitter::UpdateParticles(const float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view) {

    for (Particle* particle : particles) {
        if (particle->alive == false)
            continue;

        particle->Update(pDeltaTime, projection, view);
    }
    elapsedTime += pDeltaTime;

    float emissionInterval = 1.0f / emissionRate;

    while (elapsedTime >= emissionInterval)
    {
        EmitParticle();
        elapsedTime -= emissionInterval;
    }
}

void ParticleEmitter::SetEmissionRate(float pEmissionRate) {
    emissionRate = pEmissionRate;
}
void ParticleEmitter::SetAlpha(float value, float endValue)
{
    startAlpha = value;
    endAlpha = endValue;
    for (Particle* particle : particles)
    {
        particle->SetAlpha(startAlpha, endAlpha);
    }
}

void ParticleEmitter::SetRotation(float value, float endValue)
{
    startRotation = value;
    endRotation = endValue;

    for (Particle* particle : particles)
    {
        particle->SetRotation(startRotation, endRotation);
    }
}


void ParticleEmitter::SetSize(glm::vec2 value, glm::vec2 endValue)
{
    startSize= value;
    endSize = endValue;
    for (Particle* particle : particles)
    {
        particle->SetSize(startSize, endSize);
    }
}


void ParticleEmitter::SetLifetime(float pLifetime) {
    for (Particle* particle:  particles) {
        lifetime = pLifetime;
        particle->SetLifeTime( lifetime);
    }
}



