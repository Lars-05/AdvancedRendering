#include "GPUParticleEmitter.h"

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GPUParticle.h"
#include "Random.h"

GPUParticleEmitter::GPUParticleEmitter(
    GLuint pComputeProgram,
    GLuint pRenderProgram,
    GLuint pQuadVAO,
    core::Texture* pTexture,
    int pMaxParticles
): computeProgram(pComputeProgram), renderProgram(pRenderProgram), quadVAO(pQuadVAO), particleTexture(pTexture), maxParticlesCount(pMaxParticles), particles(pMaxParticles) {
    printf("Constructor constructed succesfully");
}

GPUParticleEmitter::~GPUParticleEmitter()
{
    if (ssbo != 0)
    {
        glDeleteBuffers(1, &ssbo);
    }

    particles.clear();
    printf("Destructor destructed");
}

void GPUParticleEmitter::SetEmissionRate(float rate) {
    emissionRate = rate;
}

void GPUParticleEmitter::Initialize()
{
    for (auto& p : particles)
    {
        p.position = glm::vec4(0.0f);
        p.velocity = glm::vec4(0.0f);
        p.color    = glm::vec4(1.0f);

        // x = life, y = maxLife, z = size, w = alive
        p.data1 = glm::vec4(
            0.0f,   // life
            5.0f,   // max life
            0.2f,   // size
            0.0f    // dead
        );

        p.data2 = glm::vec4(
            9.81f,  // gravity
            0.0f,
            0.0f,
            0.0f
        );
    }

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(GPUParticle) * maxParticlesCount,particles.data(),GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUParticleEmitter::EmitParticle()
{
    for (auto& p : particles)
    {
        // find dead particle
        if (p.data1.w > 0.5f)
            continue;

        // reset particle
        p.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        p.velocity = glm::vec4(
            random.RandomFloat(-1.0f, 1.0f),
            random.RandomFloat(2.0f, 5.0f),
            0.0f,
            0.0f
        );

        p.color = glm::vec4(1.0f);

        p.data1 = glm::vec4(
            5.0f,  // life
            5.0f,  // max life
            0.2f,  // size
            1.0f   // alive
        );

        p.data2  = glm::vec4(
        1, // gravity
        0,
        0,
        0
        );

        // upload to GPU
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER,0,sizeof(GPUParticle) * maxParticlesCount,particles.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        return;
    }
}

bool emit = true;

void GPUParticleEmitter::Update(float deltaTime)
{
    elapsedTime += deltaTime;

    float interval = 1.0f / emissionRate;

    if (elapsedTime >= interval && emit) {
        EmitParticle();
        emit = false;
    }

    glUseProgram(computeProgram);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glUniform1f(glGetUniformLocation(computeProgram, "deltaTime"),deltaTime);
   // glUniform1f(glGetUniformLocation(computeProgram, "particleIn"),particles[0]);

    glUniform1ui(glGetUniformLocation(computeProgram, "maxParticles"),maxParticlesCount);

    GLuint workGroups = (maxParticlesCount + 255) / 256; //batch of parallel threads on gpu

    // go my compute shader!!
    glDispatchCompute(workGroups, 1, 1);


    // make sure writing to ssbo has finished before read
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    std::vector<GPUParticle> particleS;
    particleS.resize(32);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (ptr) {
        memcpy(particleS.data(), ptr, sizeof(GPUParticle) * particleS.size());
    }


    if (!glIsProgram(renderProgram))
        printf("Render program invalid\n");

    GLuint program = renderProgram;

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cout << "Shader link error:\n" << infoLog << std::endl;
    }

    GLint projLoc  = glGetUniformLocation(program, "projection");
    GLint viewLoc  = glGetUniformLocation(program, "view");
    GLint worldLoc = glGetUniformLocation(program, "worldPos");

    if (projLoc == -1)
        std::cout << "projection uniform not found\n";

    if (viewLoc == -1)
        std::cout << "view uniform not found\n";

    if (worldLoc == -1)
        std::cout << "worldPos uniform not found\n";


    particles.clear();
    for (int i = 0;  i < particleS.size(); i++) {
        particles.push_back(particleS[i]);
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);





    //while (elapsedTime >= interval)
    //{
       // EmitParticle();
        //elapsedTime -= interval;
    //}
}

void GPUParticleEmitter::Render(const glm::mat4& projection,const glm::mat4& view, const glm::vec3& worldPos)
{
    glUseProgram(renderProgram);

    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glUniformMatrix4fv(glGetUniformLocation(renderProgram, "projection"),1,GL_FALSE,&projection[0][0]);

    glUniformMatrix4fv(
        glGetUniformLocation(renderProgram, "view"),
        1,
        GL_FALSE,
        &view[0][0]
    );


    glUniform3f(glGetUniformLocation(renderProgram, "worldPos"), worldPos.x, worldPos.y, worldPos.z);



    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, particleTexture->getId());

    auto f = glGetUniformLocation(renderProgram, "particleTexture");
    glUniform1i(
       f,
        0
    );

    glBindVertexArray(quadVAO);

    // glDrawArraysInstanced(
    //     GL_TRIANGLES,
    //     0,
    //     8,
    //     maxParticlesCount
    // );

    glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}




void GPUParticleEmitter::Debug()
{



    printf("Compute Program : %u\n", computeProgram);
    printf("Render Program  : %u\n", renderProgram);

    if (!glIsProgram(computeProgram))
        printf("[ERROR] Compute program invalid\n");

    if (!glIsProgram(renderProgram))
        printf("[ERROR] Render program invalid\n");


    printf("\nQuad VAO        : %u\n", quadVAO);
    printf("SSBO            : %u\n", ssbo);

    if (!glIsVertexArray(quadVAO))
        printf("[ERROR] Quad VAO invalid\n");

    if (!glIsBuffer(ssbo))
        printf("[ERROR] SSBO invalid\n");


    GLint bufferSize = 0;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    printf("SSBO Size       : %d bytes\n", bufferSize);


    printf("\nMax Particles   : %d\n", maxParticlesCount);
    printf("Emission Rate   : %.2f\n", emissionRate);
    printf("Elapsed Time    : %.2f\n", elapsedTime);

    int currentAliveCount = 0;
    int currentDeadCount  = 0;



    for (const auto& p : particles)
    {
        if (p.data1.w > 0.5f)
            currentAliveCount++;
        else
            currentDeadCount++;
    }

    aliveCount = currentAliveCount;
    deadCount = currentDeadCount;

    currentAliveCount = 0;
    currentDeadCount = 0;
    if (!particles.empty())
    {
        const auto& p = particles[0];

        printf("Position       : %.2f %.2f %.2f\n",p.position.x, p.position.y, p.position.z);

        printf("Velocity       : %.2f %.2f %.2f\n",p.velocity.x, p.velocity.y, p.velocity.z);

        printf("Life           : %.2f\n", p.data1.x);
        printf("Max Life       : %.2f\n", p.data1.y);
        printf("Size           : %.2f\n", p.data1.z);
        printf("Alive          : %.2f\n", p.data1.w);
    }

    GLenum err;
    bool hasError = false;

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("[OpenGL ERROR] %u\n", err);
        hasError = true;
    }

    if (!hasError)
        printf("\nOpenGL: no errors\n");

}