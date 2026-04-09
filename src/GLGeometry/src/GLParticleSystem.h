#ifndef GLPARTICLESYSTEM_H
#define GLPARTICLESYSTEM_H

#include "GLGeometry.h"
#include "GLElemObject.h"
#include "utils.h"

namespace GLGeometry {
struct GLParticle {
    // Constructor
    GLParticle(glm::vec3 posVal, glm::vec3 velVal, glm::vec3 scaleVal,
               float maxAgeVal, Material *materialVal)
        : position{posVal}, velocity{velVal}, scale{scaleVal},
          material{materialVal}, age{0.f}, maxAge{maxAgeVal} {
        computeModelMatrix();
    }

    ~GLParticle() { delete[] material; }

    void computeModelMatrix() {
        modelMatrix = glm::translate(glm::mat4(1.f), position);
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    float invMass;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 scale;

    glm::mat4 modelMatrix;

    Material *material;

    float age;
    float maxAge;
};

class GLParticleSystem : public GLElemObject {
public:
    GLParticleSystem(std::unique_ptr<GLElemObject> geometryObject,
                     Shader *GPassShader);

    std::list<std::unique_ptr<GLParticle>> *getPointerToListOfParticles();

    void addParticle(glm::vec3 position, glm::vec3 velocity, glm::vec3 scale,
                     float maxAge, Material *material);

    void draw();

private:
    std::list<std::unique_ptr<GLParticle>> mParticles;

    Shader *mGPassShader;

    std::unique_ptr<GLElemObject> mGeometryObject;
};
} // namespace GLGeometry

#endif
