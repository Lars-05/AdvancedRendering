#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
private:
    glm::mat4 modelMatrix{};

public:
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 size{};

    Transform()
        : position(0.0f),
          rotation(0.0f),
          size(1.0f),
          modelMatrix(1.0f)
    {}

    Transform(const Transform &other) {
        if (&other != this) {
            position = other.position;
            rotation = other.rotation;
            size = other.size;
        }
    }

    void Translate(const glm::vec3& delta);
    void Rotate(const glm::vec3& deltaDegrees);
    void Scale(const glm::vec3& delta);

    glm::vec3 GetPosition();

    void SetPosition(const glm::vec3& p);
    void SetRotation(const glm::vec3& r);
    void SetScale(const glm::vec3& s);


    glm::mat4 GetModelMatrix() const;

private:
    void RecalculateMatrix();
};
