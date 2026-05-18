#pragma once
#include "core/model.h"
#include "core/texture.h"

class Particle {
private:
    core::Texture &particleTexture;
    GLint particleTextureUniform{};
    GLint alphaUniform{};
    GLint mvpUL{};
    const GLint program;
    const core::Model &baseModel;
    glm::vec2 velocity;

    glm::vec4 color;
    Transform transform;

    float gravity;
    float startGravity;
    float endGravity;

    float alpha;
    float startAlpha;
    float endAlpha;

    glm::vec2 size;
    glm::vec2 startSize;
    glm::vec2 endSize;

    float rotation;
    float startRotation;
    float endRotation;

    float baseLifeTime;
    float lifeTime;

public:
    bool alive = true;


    Particle(const core::Model &pBaseModel, core::Texture &pParticleTexture, GLint pProgram);

    void Update(float pDeltaTime, const glm::mat4 &projection, const glm::mat4 &view);

    void SetVelocity(glm::vec2);

    void SetLifeTime(float);

    void SetPosition(glm::vec3);

    void SetSize(glm::vec2, glm::vec2);

    void SetGravity(float, float);

    void SetRotation(float, float);

    void SetColor(glm::vec4);

    void SetAlpha(float, float);



    Transform &GetTransform();
}
;
