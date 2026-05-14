#include "GLParticleSystem.h"

namespace GLGeometry {

GLParticleSystem::GLParticleSystem(std::unique_ptr<GLElemObject> geometryObject,
                                   GLBase::Shader *GPassShader)
    : mGPassShader{GPassShader}, mGeometryObject{std::move(geometryObject)} {}

std::list<std::unique_ptr<GLParticle>> *
GLParticleSystem::getPointerToListOfParticles() {
    return &mParticles;
}

void GLParticleSystem::addParticle(glm::vec3 position, glm::vec3 velocity,
                                   glm::vec3 scale, float maxAge,
                                   std::unique_ptr<GLBase::Material> material) {
    mParticles.push_back(std::make_unique<GLParticle>(position, velocity, scale,
                                                      maxAge, std::move(material)));
}

void GLParticleSystem::draw(GLBase::Shader *) {
    // Draw all the particles, with their corresponding model matrices
    for (auto &particle : mParticles) {
        mGPassShader->setMat4("model", particle->modelMatrix);

        // Configure the material in the shader
        particle->material->configShader();

        // Draw the object
        particle->material->setShaderModelMatrix(particle->modelMatrix);
        mGeometryObject->draw();
    }
}

} // namespace GLGeometry
