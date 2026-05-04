#pragma once
#include "core/model.h"
#include "core/texture.h"

class Particle {
private:
    core::Texture &particleTexture;
    GLint particleTextureUniform{};
    GLint mvpUL{};
    const GLint program;
    const core::Model &baseModel;
    glm::vec2 velocity;
    glm::vec4 color;
    Transform transform;
    float lifeTime{};

public:
    Particle(const core::Model &pBaseModel, core::Texture &pParticleTexture, GLint pProgram);

    void Update(float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view);

    void SetVelocity(glm::vec2);

    void SetLifeTime(float);

    void SetColor(glm::vec4);

    Transform &GetTransform();
}
;
