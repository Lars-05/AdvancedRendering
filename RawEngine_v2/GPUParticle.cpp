//
// Created by Lars on 10-5-2026.
//

#include "GPUParticle.h"

GPUParticle::GPUParticle() {
    position = glm::vec4(0.0f);
    velocity = glm::vec4(0.0f);
    color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    data1 = glm::vec4(
        0.0f, // life
        5.0f, // max life
        0.2f, // size
        0.0f);

    data2 = glm::vec4(
        9.81f, // gravity
        0.0f,
        0.0f,
        3.0f
    );
}

GPUParticle::~GPUParticle() = default;

GPUParticle &GPUParticle::operator=(const GPUParticle &other) = default;
