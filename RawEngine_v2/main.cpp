#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Camera.h"
#include "CSVTools.hpp"
#include "GPUParticleEmitter.h"
#include "ParticleEmitter.h"
#include "Transform.h"
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/texture.h"

//#define MAC_CLION
#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ParticleEmitter.h"
#endif

#ifdef VSTUDIO
// Note: install imgui with:
//     ./vcpkg.exe install imgui[glfw-binding,opengl3-binding]
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

int g_width = 800;
int g_height = 600;

Camera* myCamera;
ParticleEmitter* myParticleEmitter;
GPUParticleEmitter* myGpuParticleEmitter;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow *window,
                             int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

std::string readFileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

GLuint generateShader(const std::string &shaderPath, GLuint shaderType) {
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char *s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint computeShader = generateShader("shaders/particle.comp", GL_COMPUTE_SHADER);
    const GLuint particleRenderShader = generateShader("shaders/particleVert.vs", GL_VERTEX_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);
    const GLuint particleFragmentShader =generateShader("shaders/GPUparticleShader.fs",GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];
    const unsigned int modelShaderProgram = glCreateProgram();
    glAttachShader(modelShaderProgram, modelVertexShader);
    glAttachShader(modelShaderProgram, fragmentShader);
    glLinkProgram(modelShaderProgram);
    glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, modelVertexShader);
    glAttachShader(textureShaderProgram, textureShader);
    glLinkProgram(textureShaderProgram);
    glGetProgramiv(textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textureShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    GLuint particleFragmentProgram = glCreateProgram();
    glAttachShader(particleFragmentProgram,particleRenderShader);
    glAttachShader(particleFragmentProgram,particleFragmentShader);
    glLinkProgram(particleFragmentProgram);
    glGetProgramiv(particleFragmentProgram,GL_LINK_STATUS,&success);
    if (!success)
    {
        glGetProgramInfoLog(particleFragmentProgram,512,NULL,infoLog);
        printf("Particle Program Link Error: %s\n",infoLog
        );
    }
    GLuint computeProgram = glCreateProgram();

    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(computeProgram, 512, NULL, infoLog);
        printf("Compute Program Link Error: %s\n", infoLog);
    }

    GLuint particleVertexProgram = glCreateProgram();

    glAttachShader(particleVertexProgram, particleRenderShader);
    glAttachShader(particleVertexProgram, particleFragmentShader);

    glLinkProgram(particleVertexProgram);

    glGetProgramiv(particleVertexProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(particleVertexProgram, 512, NULL, infoLog);
        printf("Particle Program Link Error: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);

    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({quad});
    quadModel.transform->Translate(glm::vec3(0,0,-2.5));
    quadModel.transform->Scale(glm::vec3(0.01f, 0.01f, 1));

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");
    core::Texture dotTexture("textures/circle_05.png");

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);

    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;


    myCamera = new Camera();
    myParticleEmitter =  new ParticleEmitter(quadModel, &dotTexture, textureShaderProgram);
    myGpuParticleEmitter = new GPUParticleEmitter(computeProgram,particleVertexProgram,quadModel.getVAO(),&dotTexture,1000);
    myGpuParticleEmitter->Initialize();
    myGpuParticleEmitter->EmitParticle();

    std::vector<float> frameTimes;
    std::vector<int> particleAmounts;


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        myCamera->handle_input(window);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(g_width) / static_cast<float>(g_height), 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(myCamera->cameraPos, myCamera->cameraPos + myCamera->cameraFront, myCamera->cameraUp); // from world to camera space

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Particle Engine (Based on Raw Engine V2");

        static int modeIndex = 1; // default GPU
        const char* modes[] = { "CPU", "GPU" };

        ImGui::Combo("Particle Mode", &modeIndex, modes, 2);

        bool useCPU = (modeIndex == 0);
        bool useGPU = (modeIndex == 1);

        ImGui::Separator();
        ImGui::Text("Particle Emitter");


        if (useCPU) {

            static float emissionRate = 0.0f;
            ImGui::SeparatorText("Emission");

            ImGui::SliderFloat("Emission Rate",&emissionRate,1.0f,100000.0f,"%.1f",ImGuiSliderFlags_Logarithmic);
            {
                myParticleEmitter->SetEmissionRate(emissionRate);
            }



            static float startGravity = 0.0f;
            static float endGravity = 0.0f;

            ImGui::SeparatorText("Gravity");

            if (ImGui::SliderFloat("Start Gravity", &startGravity, -20.0f, 20.0f) |ImGui::SliderFloat("End Gravity", &endGravity, -20.0f, 20.0f))
            {
                myParticleEmitter->SetGravity(startGravity, endGravity);
            }




            static float beginAlpha = 1.0f;
            static float endAlpha = 0.0f;

            ImGui::SeparatorText("Alpha");

            if (ImGui::SliderFloat("Begin Alpha", &beginAlpha, 0.0f, 1.0f) |
            ImGui::SliderFloat("End Alpha", &endAlpha, 0.0f, 1.0f))
            {
                myParticleEmitter->SetAlpha(beginAlpha, endAlpha);
            }


            static float lifetime = 2.0f;

            ImGui::SeparatorText("Lifetime");

            if (ImGui::SliderFloat("Lifetime", &lifetime, 0.01f, 100.0f))
            {
                myParticleEmitter->SetLifetime(lifetime);
            }


            static glm::vec2 startSize(0.2f, 0.2f);
            static glm::vec2 endSize(0.05f, 0.05f);

            ImGui::SeparatorText("Size");

            if (ImGui::DragFloat2("Start Size", &startSize.x, 0.01f, 0.0f, 10.0f) |
            ImGui::DragFloat2("End Size", &endSize.x, 0.01f, 0.0f, 10.0f))
            {
                myParticleEmitter->SetSize(startSize, endSize);
            }

            static float startRotation = 0.0f;
            static float endRotation = 360.0f;

            ImGui::SeparatorText("Rotation");

            if (ImGui::DragFloat("Start Rotation", &startRotation, 1.0f, -360.0f, 360.0f) |
            ImGui::DragFloat("End Rotation", &endRotation, 1.0f, -360.0f, 360.0f))
            {
                myParticleEmitter->SetRotation(startRotation, endRotation);
            }




            ImGui::SeparatorText("Stats");

            int aliveCount = 0;

            for (Particle* particle : myParticleEmitter->particles)
            {
                if (particle->alive)
                    aliveCount++;
            }


            ImGui::Text("Particles Alive: %d", aliveCount);
            particleAmounts.push_back(aliveCount); // send to CSV
            ImGui::Text("Max Particles: %d", myParticleEmitter->maxParticleCount);
            myParticleEmitter->UpdateParticles(deltaTime, projection, view);
        }
        else {
            myGpuParticleEmitter->Update(deltaTime);
            myGpuParticleEmitter->Render(projection, view);
            //myGpuParticleEmitter->Debug();

            //ImGui::Text("Particles Alive: %d", aliveCount);
            //particleAmounts.push_back(aliveCount); // send to CSV

            static float emissionRate = 0.0f;
            ImGui::SeparatorText("Emission");

            ImGui::SliderFloat("Emission Rate",&emissionRate,1.0f,100000.0f,"%.1f",ImGuiSliderFlags_Logarithmic);
            {
                myGpuParticleEmitter->SetEmissionRate(emissionRate);
            }

            static float startGravity = 0.0f;
            static float endGravity = 0.0f;

            ImGui::SeparatorText("Gravity");

            if (ImGui::SliderFloat("Start Gravity", &startGravity, -20.0f, 20.0f) |ImGui::SliderFloat("End Gravity", &endGravity, -20.0f, 20.0f))
            {
                myGpuParticleEmitter->SetGravity(startGravity, endGravity);
            }

            static glm::vec2 startSize(0.2f, 0.2f);
            static glm::vec2 endSize(0.05f, 0.05f);

            ImGui::SeparatorText("Size");

            if (ImGui::DragFloat2("Start Size", &startSize.x, 0.01f, 0.0f, 10.0f) |
            ImGui::DragFloat2("End Size", &endSize.x, 0.01f, 0.0f, 10.0f))
            {
                myGpuParticleEmitter->SetSize(startSize, endSize);
            }

            static float lifetime = 2.0f;

            ImGui::SeparatorText("Lifetime");

            if (ImGui::SliderFloat("Lifetime", &lifetime, 0.01f, 100.0f))
            {
                myGpuParticleEmitter->SetLifetime(lifetime);
            }


            ImGui::Text("Alive: %d", myGpuParticleEmitter->aliveCount);
            ImGui::Text("Dead: %d", myGpuParticleEmitter->deadCount);
            ImGui::Text("Max Particles: %d", myGpuParticleEmitter->maxParticlesCount);







        }




        ImGui::End();






        //bill board stuff
        //glm::vec3 directionToParticle = myCamera->cameraPos - quad;




        processInput(window);
        //suzanne.transform->SetRotation(glm::vec3(0.0f, 1.0f, 0.0f));
/*
        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(projection * view * quadModel.transform->GetModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        //quadModel.render();
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
*/

        glUseProgram(modelShaderProgram);
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(projection * view * suzanne.transform->GetModelMatrix()));
        //suzanne.render();
        glBindVertexArray(0);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;

        frameTimes.push_back(deltaTime);

    }


    CSVTools::WriteCSV(frameTimes, particleAmounts, "TestData");
    glDeleteProgram(modelShaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}