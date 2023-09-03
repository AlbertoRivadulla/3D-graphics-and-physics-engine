#ifndef GLPARTICLESYSTEM_H
#define GLPARTICLESYSTEM_H

#include "GLGeometry.h"
#include "GLElemObject.h"
#include "utils.h"

namespace GLGeometry
{
    struct GLParticle
    {
        // Constructor
        GLParticle( glm::vec3 posVal, glm::vec3 velVal, glm::vec3 scaleVal, 
                    float maxAgeVal, Material* materialVal ) :
            position { posVal }, velocity { velVal }, scale { scaleVal }, 
            age { 0.f }, maxAge { maxAgeVal }, material { materialVal }
        {
            computeModelMatrix();
        }
        
        ~GLParticle()
        {
            delete[] material;
        }

        void computeModelMatrix()
        {
            modelMatrix = glm::translate( glm::mat4(1.f), position );
            modelMatrix = glm::scale( modelMatrix, scale );
        }

        float invMass;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 scale;

        glm::mat4 modelMatrix;

        Material* material;

        float age;
        float maxAge;
    };

    class GLParticleSystem : public GLElemObject
    {
        public:
            // Constructor
            GLParticleSystem(GLElemObject* geometryObject, Shader* GPassShader);
            ~GLParticleSystem();

            // Get a pointer to the list of particles
            std::list<GLParticle*>* getPointerToListOfParticles();
            // std::list<GLParticle>* getPointerToListOfParticles();

            // Add a single particle
            void addParticle( glm::vec3 position, glm::vec3 velocity, glm::vec3 scale, 
                              float maxAge, Material* material );

            // Function to render
            void draw();

            // List of particles
            std::list<GLParticle*> mParticles;

        private:
            // List of particles
            // std::list<GLParticle*> mParticles;
            // std::list<GLParticle> mParticles;

            // Pointer to the shader of the geometry pass
            Shader* mGPassShader;

            // Geometrical object
            GLElemObject* mGeometryObject;
    };
}



#endif
