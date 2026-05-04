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
    transform = *pBaseModel.transform;
}

void Particle::SetVelocity(glm::vec2 pVelocity) {

    velocity = pVelocity;
}

void Particle::Update(const float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view) {
    lifeTime -= pDeltaTime;
    transform.Translate(glm::vec3( velocity.x * pDeltaTime,velocity.y * pDeltaTime, 0));

    glUseProgram(program);
    glUniformMatrix4fv(mvpUL, 1, GL_FALSE, glm::value_ptr(projection * view * transform.GetModelMatrix()));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(particleTextureUniform, 0);
    glBindTexture(GL_TEXTURE_2D, particleTexture.getId());
    baseModel.render();
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Particle::SetLifeTime(float pLifeTime) {
    lifeTime = pLifeTime;
}

void Particle::SetColor(glm::vec4 pColor) {
    color = pColor;
}

Transform & Particle::GetTransform() {
    return transform;
}
