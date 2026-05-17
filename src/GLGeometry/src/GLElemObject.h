#ifndef GLOBJECTELEMENTARY_H
#define GLOBJECTELEMENTARY_H

#include "GLObject.h"
#include "mesh.h"

namespace GLGeometry {
class GLElemObject : public GLObject {
protected:
    // Objects needed for rendering
    unsigned int mVAO;
    unsigned int mVBO;
    // Not all derived classes will have a EBO!
    // unsigned int mEBO;

    // Data of the mesh
    std::vector<GLBase::Vertex> mVertices;

public:
    // Constructor
    GLElemObject()
        : mVAO{0}, mVBO{0} // , mEBO { 0 }
    {
        // Create the Vertex array object
        glGenVertexArrays(1, &mVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mVBO);
        // // Create the Element buffer object
        // glGenBuffers(1, &mEBO);
    }

    virtual ~GLElemObject() = default;

    // Function to get the positions of the vertices
    std::vector<glm::vec3> getVertices() {
        // Initialize the vector
        std::vector<glm::vec3> vertices(mVertices.size());

        // Store all the positions of the vertices
        for (int i = 0; i < mVertices.size(); ++i)
            vertices[i] = mVertices[i].Position;

        return vertices;
    }

    // // Function to render
    // virtual void draw() = 0;
};

class GLObjectPlaceholder : public GLElemObject {
private:
    // // This class will use an element buffer object
    // unsigned int mEBO;

    // // Data of the mesh
    // // std::vector<Vertex> mVertices;
    // std::vector<unsigned int> mIndices;
    //
    // // Number of vertices in each circle
    // int mNrVertices;

public:
    // Constructor
    GLObjectPlaceholder() {}

    // Function to render
    void draw() {}
};

struct GLObjectWithTransform {
    GLElemObject *object = nullptr;
    glm::mat4 modelMatrix = glm::mat4(1.f);

    void setModelMatrix(const glm::vec3 &translation, const float &rotationAngle, const glm::vec3 &rotationAxis,
                        const glm::vec3 &scale) {
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, translation);
        if (rotationAngle != 0.)
            modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::normalize(rotationAxis));
        modelMatrix = glm::scale(modelMatrix, scale);
    }
    void setModelMatrix(const glm::vec3 &translation, const glm::mat4 &rotationMatrix, const glm::vec3 &scale) {
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, translation);
        modelMatrix = modelMatrix * rotationMatrix;
        modelMatrix = glm::scale(modelMatrix, scale);
    }

    void setModelMatrix(const glm::mat4 &modelMatrixVal) { modelMatrix = modelMatrixVal; }

    // Function to read the model matrix
    const glm::mat4 &getModelMatrix() const { return modelMatrix; }
};

} // namespace GLGeometry

#endif
