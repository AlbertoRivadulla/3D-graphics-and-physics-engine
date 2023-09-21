#include "GLParticleSystem.h"
#include "utils.h"


using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLParticleSystem::GLParticleSystem(GLElemObject* geometryObject, Shader* GPassShader) :
        mGPassShader { GPassShader }, mGeometryObject { geometryObject }
    {
    }

    // Destructor
    GLParticleSystem::~GLParticleSystem()
    {
        for ( auto particle : mParticles )
            delete particle;
    }

    // Get a pointer to the list of particles
    std::list<GLParticle*>* GLParticleSystem::getPointerToListOfParticles()
    // std::list<GLParticle>* GLParticleSystem::getPointerToListOfParticles()
    {
        return &mParticles;
    }

    // Add a single particle
    void GLParticleSystem::addParticle( glm::vec3 position, glm::vec3 velocity, 
                                        glm::vec3 scale, float maxAge, Material* material )
    {
        mParticles.push_back( new GLParticle(position, velocity, scale, maxAge, material ) );
    }

    // Function to render
    void GLParticleSystem::draw()
    {
        // Draw all the particles, with their corresponding model matrices
        for ( auto particle : mParticles )
        {
            mGPassShader->setMat4( "model", particle->modelMatrix );
            // Configure the material in the shader
            particle->material->configShader( particle->modelMatrix );

            // Draw the object
            mGeometryObject->draw();
        }
    }
}


