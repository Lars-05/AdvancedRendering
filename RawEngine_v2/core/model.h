#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>

#include "mesh.h"
#include "../Transform.h"


namespace core {
    class Model {
    private:
        std::vector<core::Mesh> meshes;


    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), transform(new Transform()) {}

        void render() const;
        Transform* transform;
        GLint getVAO();
    };
}
