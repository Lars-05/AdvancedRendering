#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
    const float cameraSpeed = 0.2f;
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);;
    glm::vec3 right = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);;


    float yaw = -90.0f;
    float pitch = 0.0f;
    float speed;
    float sensitivity;

    float lastX, lastY;
    bool firstMouse;

    void handle_input(GLFWwindow* window);

};