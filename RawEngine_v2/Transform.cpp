#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

void Transform::Translate(const glm::vec3& delta)
{
    position += delta;
    RecalculateMatrix();
}

void Transform::Rotate(const glm::vec3& deltaDegrees)
{
    rotation += deltaDegrees;
    RecalculateMatrix();
}

void Transform::Scale(const glm::vec3& delta)
{
    size += delta;
    RecalculateMatrix();
}

void Transform::SetPosition(const glm::vec3& p)
{
    position = p;
    RecalculateMatrix();
}

void Transform::SetRotation(const glm::vec3& r)
{
    rotation = r;
    RecalculateMatrix();
}

void Transform::SetScale(const glm::vec3& s)
{
    size = s;
    RecalculateMatrix();
}

glm::mat4 Transform::GetModelMatrix() const
{
    return modelMatrix;
}

glm::vec3 Transform::GetPosition() {
    return position;
}

void Transform::RecalculateMatrix()
{
    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), {1,0,0});
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), {0,1,0});
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), {0,0,1});

    modelMatrix = glm::scale(modelMatrix, size);
}
