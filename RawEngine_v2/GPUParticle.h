#pragma once

#include <glm/glm.hpp>

class GPUParticle
{
    public:
        glm::vec4 position{};
        glm::vec4 velocity{};
        glm::vec4 color{};

        glm::vec4 data1{};
        //x = life
        //y = max life
        //z = size
        //w = alive
        glm::vec4 data2{};
        // x = gravity
        // y = alpha
        // w = start rotation
        // w = end rotation
        glm::vec4 data3{};
        // x = start gravity
        // y = end gravity
        // z = start alpha
        // w = end alpha

        glm::vec4 data4{};
        // x = rotation

    public:

    GPUParticle();
    ~GPUParticle();
    GPUParticle& operator=(const GPUParticle& other);
};