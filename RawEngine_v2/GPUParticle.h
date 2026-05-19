#pragma once

#include <glm/glm.hpp>

class GPUParticle
{
    public:
        glm::vec3 position;
        glm::vec4 velocity;
        glm::vec4 color;

        glm::vec4 data1;
        //x = life
        //y = max life
        //z = size
        //w = alive
        glm::vec4 data2;
        // x == gravity
public:
    GPUParticle();
    ~GPUParticle();

};