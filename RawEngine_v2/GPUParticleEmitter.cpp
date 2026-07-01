#include "GPUParticleEmitter.h"

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

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
    // for (auto& p : particles)
    // {
    //     p.position = glm::vec4(0.0f);
    //     //p.velocity = glm::vec4(0.0f);
    //     p.color    = glm::vec4(1.0f);
    //
    //     // x = life, y = maxLife, z = size, w = alive
    //     p.data1 = glm::vec4(
    //         0.0f,   // life
    //         5.0f,   // max life
    //         0.2f,   // size
    //         0.0f    // dead
    //     );
    //
    //     p.data2 = glm::vec4(
    //         9.81f,  // gravity
    //         0.0f,
    //         0.0f,
    //         3.0f
    //     );
    // }

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

        p.color = glm::vec4(1.0f);

        p.data1 = glm::vec4(
            lifetime,  // life
            lifetime,  // max life
            2.2f,  // size
            1.0f   // alive
        );

        p.data2 = glm::vec4(
        0.0f, // x = gravity
        1.0f, //y = alpha
        0,
        0
        );

         p.data3 = glm::vec4(
             startGravity,// x = start gravity
             endGravity,// y = end gravity
             startAlpha,// z = start alpha
             endAlpha// w = end alpha
        );


        p.startSize = startSize;
        p.endSize = endSize;
        p.size = startSize;

        // upload to GPU
        // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        // glBufferSubData(
        //     GL_SHADER_STORAGE_BUFFER,
        //     0,
        //     sizeof(GPUParticle) * maxParticlesCount,
        //     particles.data()
        // );
        //
        // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        //
        // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //
        // void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        //
        // if (ptr)
        // {
        //     memcpy(
        //         particles.data(),
        //         ptr,
        //         sizeof(GPUParticle) * maxParticlesCount
        //     );
        // }
        //
        // glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        //
        return;
    }
}

bool emit = true;

void GPUParticleEmitter::Update(float deltaTime)
{

    float interval = 1.0f / emissionRate;

    while (elapsedTime >= interval)
    {
        EmitParticle();
        elapsedTime -= interval;
    }

    int currentAliveCount = 0;
    int currentDeadCount = 0;;
    for (const auto& p : particles)
    {
        if (p.data1.w > 0.5f)
            currentAliveCount++;
        else
            currentDeadCount++;
    }

    aliveCount = currentAliveCount;
    deadCount = currentDeadCount;

    elapsedTime += deltaTime;

    glUseProgram(computeProgram);

    //upload to GPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    glBufferSubData(
        GL_SHADER_STORAGE_BUFFER,
        0,
        sizeof(GPUParticle) * maxParticlesCount,
        particles.data()
    );

    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glUniform1f(
        glGetUniformLocation(computeProgram, "deltaTime"),
        deltaTime
    );

    glUniform1ui(
        glGetUniformLocation(computeProgram, "maxParticles"),
        maxParticlesCount
    );

    GLuint workGroups = (maxParticlesCount + 255) / 256;

    glDispatchCompute(workGroups, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // READ GPU DATA BACK TO CPU
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    //particles.clear();

    if (ptr)
    {
        memcpy(
            particles.data(),
            ptr,
            sizeof(GPUParticle) * maxParticlesCount
        );

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //printf("hi\n");
}

void GPUParticleEmitter::SetGravity(float pStartGravity, float pEndGravity) {

    startGravity = pStartGravity;
    endGravity = pEndGravity;
}
void GPUParticleEmitter::SetSize(glm::vec2 pStartSize, glm::vec2 pEndSize)
{
    startSize = pStartSize;
    endSize = pEndSize;
}
void GPUParticleEmitter::SetAlpha(float pStartAlpha, float pEndAlpha) {
    startAlpha = pStartAlpha;
    endAlpha = pEndAlpha;
}

void GPUParticleEmitter::SetLifetime(float pLifetime)
{
    lifetime = pLifetime;
}



std::vector<GPUParticle> GPUParticleEmitter::GetAliveParticles(const std::vector<GPUParticle>& particleVector){
    std::vector<GPUParticle> aliveParticles;

    for (const auto& particle : particleVector)
    {
        if (particle.data1.w == 1)
        {
            aliveParticles.push_back(particle);
        }
    }

    return aliveParticles;
}
void GPUParticleEmitter::Render(const glm::mat4& projection,const glm::mat4& view)
{
    // std::vector<GPUParticle> aliveParticles = GetAliveParticles(particles);

    for (auto& part : particles) {
        if (part.data1.w < 0.5) {
            continue;
        }

        glm::vec3 size = glm::vec3(part.size, 0.0f);

        glm::mat4 model =
            glm::translate(glm::mat4(1.0f), glm::vec3(part.position)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(size));

        glm::mat4 MVPmatrix = projection * view * model;
        glUseProgram(renderProgram);

        //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);


        glUniform3f(glGetUniformLocation(renderProgram, "worldPos"), part.position.x, part.position.y, part.position.z);
        glUniformMatrix4fv(glGetUniformLocation(renderProgram, "mvpMatrix"),1,GL_FALSE,&MVPmatrix[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particleTexture->getId());


        glUniform1i(glGetUniformLocation(renderProgram, "particleTexture"),0);

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