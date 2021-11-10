#include "GLGizmo.h"
#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLGizmo::GLGizmo(glm::vec3 position, int radius, int width, int height, glm::vec3 color) :
        // mPosition { position }, mRadius { radius },
        mShader("../shaders/GLGeometry/gizmoVertex.glsl", "../shaders/GLGeometry/gizmoFragment.glsl",
                "../shaders/GLGeometry/gizmoGeometry.glsl")
    {
        // Set the color, radius and position
        setColor(color);
        setSize(radius);
        setPosition(position);

        // Set the window width and height in the shader
        mShader.setVec2("winSize", glm::vec2(width, height));

        // Create the Vertex array object
        glGenVertexArrays(1, &mVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mVBO);

        // Create the single vertex
        float vertex[] { 0.f, 0.f, 0.f };

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &vertex[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to set the position of the gizmo
    void GLGizmo::setPosition(glm::vec3 position)
    {
        // Compute the model matrix
        mModelMatrix = glm::translate(glm::mat4(1.), position);
        // Pass the model matrix to the shader
        mShader.use();
        mShader.setMat4("model", mModelMatrix);
    }

    // Method to set the color of the gizmo
    void GLGizmo::setColor(glm::vec3 color)
    {
        // Set the color uniform in the shader
        mShader.use();
        mShader.setVec3("color", color);
    }

    // Method to set the size of the gizmo
    void GLGizmo::setSize(int size)
    {
        // Set the radius uniform in the shader
        mShader.use();
        mShader.setInt("radius", size);
    }

    // Draw method
    void GLGizmo::draw(glm::mat4 view, glm::mat4 projection)
    {
        // Setup the shader
        mShader.use();
        mShader.setMat4("view", view);
        mShader.setMat4("projection", projection);

        // Disable face culling for drawing the plane
        glDisable(GL_CULL_FACE);
        // Draw the point
        glBindVertexArray(mVAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);
        // Enable face culling again
        glEnable(GL_CULL_FACE);
    }
}
