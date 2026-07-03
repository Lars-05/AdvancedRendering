//
// Created by Lars on 10-5-2026.
//

#include "GPUParticle.h"

GPUParticle::GPUParticle() {
    position = glm::vec4(0.0f);
    velocity = glm::vec4(0.0f);
    color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    data1 = glm::vec4(
        0.0f, // x = life
        5.0f, // y = max life
        1.0f, // z = size
        0.0f); // w = alive?

    data2 = glm::vec4(
        9.81f, // x = gravity
        1.0f, //y = alpha
        1.0f, // z = start size
        1.0f // w = end size
    );
   data3 = glm::vec4(
        9.81f,// x = start gravity
        9.81f,// y = end gravity
        1.0f,// z = start alpha
        1.0f// w = end alpha
   );

    data4 = glm::vec4(
        0,0,0,0
   );
}

GPUParticle::~GPUParticle() = default;

GPUParticle &GPUParticle::operator=(const GPUParticle &other) = default;
