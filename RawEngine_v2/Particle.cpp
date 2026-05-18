//
// Created by Lars on 30-3-2026.
//

#include "Particle.h"

#include <glm/gtc/type_ptr.hpp>

// Particle::Particle(const core::Model& pBaseModel, core::Texture& pParticleTexture, const GLint pProgram) {
//     // program = pProgram;
//     // mvpUL = glGetUniformLocation(program, "mvpMatrix");
//     // velocity = glm::vec2(0, 0);
//     // color = glm::vec4();
//     // lifeTime = 1;
//     // baseModel = pBaseModel;
//     // transform = pBaseModel->transform;
//     // particleTexture = pParticleTexture;
// }

Particle::Particle(const core::Model& pBaseModel, core::Texture& pParticleTexture, const GLint pProgram)
: baseModel(pBaseModel), particleTexture(pParticleTexture), program(pProgram), velocity(0,0), color(), lifeTime(1){
    mvpUL = glGetUniformLocation(program, "mvpMatrix");
    particleTextureUniform = glGetUniformLocation(program, "text");
    alphaUniform = glGetUniformLocation(program, "alpha");
    transform = *pBaseModel.transform;
}

void Particle::SetVelocity(glm::vec2 pVelocity) {

    velocity = pVelocity;
}

void Particle::Update(const float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view) {

    if (!alive)
        return;

    lifeTime -= pDeltaTime;
    if (lifeTime < 0.0f) {
        alive = false;
    }

    float progress = 1.0f - (lifeTime / baseLifeTime);

    progress = glm::clamp(progress, 0.0f, 1.0f);

    alpha = glm::mix(startAlpha, endAlpha, progress);
    rotation = glm::mix(startRotation,endRotation,progress);
    gravity = glm::mix(startGravity,endGravity,progress);
    size = glm::mix(startSize,endSize,progress);
    transform.SetRotation(glm::vec3(0, 0, rotation));
    transform.SetScale(glm::vec3(size, 0));
    transform.Translate(glm::vec3( velocity.x * pDeltaTime,velocity.y * pDeltaTime, 0));

    velocity.y -= gravity * pDeltaTime;

    glUseProgram(program);
    glUniformMatrix4fv(mvpUL, 1, GL_FALSE, glm::value_ptr(projection * view * transform.GetModelMatrix()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(particleTextureUniform, 0);
    glBindTexture(GL_TEXTURE_2D, particleTexture.getId());
    glUniform1f(alphaUniform, alpha);
    baseModel.render();
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Particle::SetLifeTime(float pLifeTime) {
    baseLifeTime = pLifeTime;
    lifeTime = pLifeTime;
}

void Particle::SetRotation(float value, float endValue) {
    startRotation = value;
    endRotation = endValue;
}

void Particle::SetAlpha(float value, float endValue) {
    startAlpha = value;
    endAlpha = endValue;
}


void Particle::SetSize(glm::vec2 value, glm::vec2 endValue) {
    startSize = value;
    endSize = endValue;
}

void Particle::SetPosition(glm::vec3 pPosition) {
    transform.position = glm::vec3(pPosition.x, pPosition.y, pPosition.z);
}

void Particle::SetColor(glm::vec4 pColor) {
    color = pColor;
}



void Particle::SetGravity(float value, float endValue)
{
    startGravity = value;
    endGravity = endValue;
}



Transform & Particle::GetTransform() {
    return transform;
}
