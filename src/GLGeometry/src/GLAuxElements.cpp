#include "GLAuxElements.h"

using namespace GLBase;

namespace GLGeometry
{
    //==============================
    // Constructor
    //==============================
    
    // Constructor
    GLAuxElements::GLAuxElements(int width, int height) :
        mPointShader("../shaders/GLGeometry/pointVertex.glsl", "../shaders/GLGeometry/pointFragment.glsl",
                "../shaders/GLGeometry/pointGeometry.glsl"),
        mLineShader("../shaders/GLGeometry/lineVertex.glsl", "../shaders/GLGeometry/lineFragment.glsl")
    {
        // Set the number of vertices in each circle of the cylinder
        mNrVerticesCylinder = 16;
        // Set the number of vertices in each circle of the sphere
        mNrVerticesSphere = 8;
        // Set the number of vertices in the circle of the cone
        mNrVerticesCone = 16;

        // Set the size of the viewport in all the shaders
        setViewportSize(width, height);

        // Configure the point
        setupPoint();
        // Configure the line
        setupLine();
        // Configure the rectangle
        setupRectangle();
        // Configure the box
        setupBox();
        // Configure the cylinder
        setupCylinder();
        // Configure the sphere
        setupSphere();
        // Configure the cone
        setupCone();
    }

    // Method to change the dimensions of the screen
    void GLAuxElements::setViewportSize(int width, int height)
    {
        // Set the size of the viewport in all the shaders
        mPointShader.use();
        mPointShader.setVec2("winSize", glm::vec2(width, height));
    }

    //==============================
    // Methods for drawing points
    //==============================

    // Method to set the VAO, VBO and properties of the point
    void GLAuxElements::setupPoint()
    {
        // Set the color and radius of the point in the shader
        setPointSize(4);
        setPointColor(glm::vec3(1., 0.7, 0.));

        // Create the Vertex array object
        glGenVertexArrays(1, &mPointVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mPointVBO);

        // Create the single vertex
        float vertex[] { 0.f, 0.f, 0.f };

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(mPointVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mPointVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &vertex[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to change the color of the point
    void GLAuxElements::setPointColor(glm::vec3 color)
    {
        // Set the color uniform in the shader
        mPointShader.use();
        mPointShader.setVec3("color", color);
    }

    // Method to set the size of the point
    void GLAuxElements::setPointSize(int size)
    {
        // Set the radius uniform in the shader
        mPointShader.use();
        mPointShader.setInt("radius", size);
    }

    // Method to draw a point
    void GLAuxElements::drawPoint(const glm::vec3& position, const glm::mat4& view, 
                           const glm::mat4& projection)
    {
        // Set the model, view and projection matrices in the shader
        mPointShader.use();
        mPointShader.setMat4("model", glm::translate(glm::mat4(1.), position));
        mPointShader.setMat4("view", view);
        mPointShader.setMat4("projection", projection);

        // Disable face culling for drawing the plane
        glDisable(GL_CULL_FACE);
        // Draw the point
        glBindVertexArray(mPointVAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);
        // Enable face culling again
        glEnable(GL_CULL_FACE);
    }

    //==============================
    // Methods for drawing lines
    //==============================

    // Method to set the VAO, VBO and properties of the line
    void GLAuxElements::setupLine()
    {
        // Set the color of the line in the shader
        setLineColor(glm::vec3(0., 1., 0.));

        // Create the Vertex array object
        glGenVertexArrays(1, &mLineVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mLineVBO);

        // Create the two vertices of the line
        float vertices[] { 0.f, 0.f, 0.f,
                           1.f, 1.f, 1.f
                         };

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(mLineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mLineVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to change the color of the line
    void GLAuxElements::setLineColor(glm::vec3 color)
    {
        // Set the color uniform in the shader
        mLineShader.use();
        mLineShader.setVec3("color", color);
    }

    // Method to draw a line
    void GLAuxElements::drawLine(const glm::vec3& p1, const glm::vec3& p2, 
                                 const glm::mat4& view, const glm::mat4& projection)
    {
        // Compute the model matrix, following 
        //  https://www.gamedev.net/forums/topic/674736-how-would-i-draw-a-line-between-two-world-coordinate-points-using-opengl/5271106/
        glm::mat4 model { glm::translate(glm::mat4(1.), p1) };
        model = glm::scale(model, p2 - p1);
        
        // Set the model, view and projection matrices in the shader
        mLineShader.use();
        mLineShader.setMat4("model", model);
        mLineShader.setMat4("view", view);
        mLineShader.setMat4("projection", projection);

        // Draw the two vertices of the line
        glBindVertexArray(mLineVAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }

    //==============================
    // Methods for drawing rectangles
    //==============================

    // Method to set the VAO, VBO and properties of the line
    void GLAuxElements::setupRectangle()
    {
        // Create the Vertex array object
        glGenVertexArrays(1, &mRectangleVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mRectangleVBO);
        // Create the Element buffer object
        glGenBuffers(1, &mRectangleEBO);

        // Compute the four vertices by hand
        float vertices[] {  0.5f,  0.5f, 0.0f, // top right
                            0.5f, -0.5f, 0.0f, // bottom right
                           -0.5f, -0.5f, 0.0f, // bottom left
                           -0.5f,  0.5f, 0.0f  // top left
                         };

        // Indices of the vertices for the EBO
        // Each pair of vertices defines a line
        unsigned int indices[] { 0, 1, 1, 2, 2, 3, 3, 0 };

        // Bind the VAO and the VBO (as a verex buffer)
        glBindVertexArray(mRectangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mRectangleVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRectangleEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to draw a line
    void GLAuxElements::drawRectangle(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale,
                                const glm::mat4& view, const glm::mat4& projection)
    {
        // Compute the model matrix 
        glm::mat4 model { glm::translate(glm::mat4(1.), translation) };
        if (rotationAngle != 0.)
            model = glm::rotate(model, glm::radians(rotationAngle), 
                                       glm::normalize(rotationAxis));
        model = glm::scale(model, scale);
        
        // Set the model, view and projection matrices in the shader
        mLineShader.use();
        mLineShader.setMat4("model", model);
        mLineShader.setMat4("view", view);
        mLineShader.setMat4("projection", projection);

        // Draw the two vertices of the line
        glBindVertexArray(mRectangleVAO);
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //==============================
    // Method for drawing parallelepipeds
    //==============================

    // Method to draw a parallelepiped with the origin as one of its vertex, and the rest
    // spanned by the two vector v1 and v2. If these are not orthogonal, it will simply
    // draw a parallelepiped
    void GLAuxElements::drawParallelepiped(const glm::vec3& origin, const glm::vec3& v1, const glm::vec3& v2, 
                                      const glm::mat4& view, glm::mat4 projection)
    {
        // Draw four lines
        drawLine(origin, origin + v1, view, projection);
        drawLine(origin + v1, origin + v1 + v2, view, projection);
        drawLine(origin + v1 + v2, origin + v2, view, projection);
        drawLine(origin + v2, origin, view, projection);
    }

    //==============================
    // Methods for drawing boxes
    //==============================

    // Method to set the VAO, VBO and properties of the line
    void GLAuxElements::setupBox()
    {
        // Create the Vertex array object
        glGenVertexArrays(1, &mBoxVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mBoxVBO);
        // Create the Element buffer object
        glGenBuffers(1, &mBoxEBO);

        // Compute the eight vertices by hand
        float vertices[] {  0.5f,  0.5f,  0.5f, 
                            0.5f, -0.5f,  0.5f, 
                           -0.5f, -0.5f,  0.5f, 
                           -0.5f,  0.5f,  0.5f, 
                            0.5f,  0.5f, -0.5f,
                            0.5f, -0.5f, -0.5f,
                           -0.5f, -0.5f, -0.5f,
                           -0.5f,  0.5f, -0.5f 
                         };

        // Indices of the vertices for the EBO
        // Each pair of vertices defines a line
        unsigned int indices[] { 0, 1, 1, 2, 2, 3, 3, 0,
                                 4, 5, 5, 6, 6, 7, 7, 4,
                                 0, 4, 1, 5, 2, 6, 3, 7
                               };

        // Bind the VAO and the VBO (as a verex buffer)
        glBindVertexArray(mBoxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mBoxVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBoxEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to draw a line
    void GLAuxElements::drawBox(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale,
                                const glm::mat4& view, const glm::mat4& projection)
    {
        // Compute the model matrix 
        glm::mat4 model { glm::translate(glm::mat4(1.), translation) };
        if (rotationAngle != 0.)
            model = glm::rotate(model, glm::radians(rotationAngle), 
                                       glm::normalize(rotationAxis));
        model = glm::scale(model, scale);
        
        // Set the model, view and projection matrices in the shader
        mLineShader.use();
        mLineShader.setMat4("model", model);
        mLineShader.setMat4("view", view);
        mLineShader.setMat4("projection", projection);

        // Draw the two vertices of the line
        glBindVertexArray(mBoxVAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //==============================
    // Methods for drawing cylinders
    //==============================

    // Method to set the VAO, VBO and properties of the line
    void GLAuxElements::setupCylinder()
    {
        // Create the Vertex array object
        glGenVertexArrays(1, &mCylinderVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mCylinderVBO);
        // Create the Element buffer object
        glGenBuffers(1, &mCylinderEBO);

        // Lists of vertices and indices
        std::vector<float> vertices;
        std::vector<int> indices;
        vertices.reserve(2 * mNrVerticesCylinder * 3);
        indices.reserve(2 * 2 * mNrVerticesCylinder);

        // Generate the vertices
        for (int y = 0; y <= 1; ++y)
        {
            for (int angle = 0; angle < mNrVerticesCylinder; ++angle)
            {
                vertices.push_back(0.5 * glm::cos(2. * glm::pi<float>() * (float)angle / mNrVerticesCylinder));
                vertices.push_back((float)y - 0.5);
                vertices.push_back(0.5 * glm::sin(2. * glm::pi<float>() * (float)angle / mNrVerticesCylinder));
            }
        }

        // Generate the indices for the EBO
        for (int i = 0; i < mNrVerticesCylinder; ++i)
        {
            // Join vertice in the upper base
            indices.push_back(i);
            indices.push_back((i + 1) % mNrVerticesCylinder);

            // Join vertices in the lower base
            indices.push_back(i + mNrVerticesCylinder);
            indices.push_back((i + 1) % mNrVerticesCylinder + mNrVerticesCylinder);

            // Join vertices between bases
            indices.push_back(i);
            indices.push_back(i + mNrVerticesCylinder);
        }

        // Bind the VAO and the VBO (as a verex buffer)
        glBindVertexArray(mCylinderVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mCylinderVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCylinderEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to draw a line
    void GLAuxElements::drawCylinder(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale,
                                const glm::mat4& view, const glm::mat4& projection)
    {
        // Compute the model matrix 
        glm::mat4 model { glm::translate(glm::mat4(1.), translation) };
        if (rotationAngle != 0.)
            model = glm::rotate(model, glm::radians(rotationAngle), 
                                       glm::normalize(rotationAxis));
        model = glm::scale(model, scale);
        
        // Set the model, view and projection matrices in the shader
        mLineShader.use();
        mLineShader.setMat4("model", model);
        mLineShader.setMat4("view", view);
        mLineShader.setMat4("projection", projection);

        // Draw the two vertices of the line
        glBindVertexArray(mCylinderVAO);
        glDrawElements(GL_LINES, 3 * 2 * mNrVerticesCylinder, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //==============================
    // Methods for drawing spheres
    //==============================

    // Method to set the VAO, VBO and properties of the line
    void GLAuxElements::setupSphere()
    {
        // Create the Vertex array object
        glGenVertexArrays(1, &mSphereVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mSphereVBO);
        // Create the Element buffer object
        glGenBuffers(1, &mSphereEBO);

        // Lists of vertices and indices
        std::vector<float> vertices;
        std::vector<int> indices;
        vertices.reserve(3 * (2 * (mNrVerticesSphere * mNrVerticesSphere - mNrVerticesSphere + 1)));
        indices.reserve(2 * 2 * mNrVerticesSphere * (2 * mNrVerticesSphere - 1));

        // Generate the vertices
        // Upper cusp
        vertices.push_back(0.);
        vertices.push_back(0.5);
        vertices.push_back(0.);
        // Loop vertically
        for (int t = 1; t < mNrVerticesSphere; ++t)
        {
            // Loop horizontally
            for (int p = 0; p < 2 * mNrVerticesSphere; ++p)
            {
                vertices.push_back(0.5 * glm::sin(glm::pi<float>() * (float)t / mNrVerticesSphere) *
                                         glm::cos(glm::pi<float>() * (float)p / mNrVerticesSphere));
                vertices.push_back(0.5 * glm::cos(glm::pi<float>() * (float)t / mNrVerticesSphere));
                vertices.push_back(0.5 * glm::sin(glm::pi<float>() * (float)t / mNrVerticesSphere) *
                                         glm::sin(glm::pi<float>() * (float)p / mNrVerticesSphere));
            }
        }
        // Lower cusp
        vertices.push_back(0.);
        vertices.push_back(-0.5);
        vertices.push_back(0.);

        // Generate the indices for the EBO
        // Join the upper cusp
        for (int p = 1; p < 2 * mNrVerticesSphere + 1; ++p)
        {
            // Vertical lines
            indices.push_back(0);
            indices.push_back(p);
            // Horizontal lines
            indices.push_back(p);
            indices.push_back(p % (2 * mNrVerticesSphere) + 1);
        }
        // Loop horizontally
        for (int p = 0; p < 2 * mNrVerticesSphere; ++p)
        {
            // Loop vertically
            for (int t = 1; t < mNrVerticesSphere - 1; ++t)
            {
                // Horizontal line
                indices.push_back(p + t * (2 * mNrVerticesSphere) + 1);
                indices.push_back((p + 1) % (2 * mNrVerticesSphere) + t * (2 * mNrVerticesSphere) + 1);
                // Vertical line to the vertex above
                indices.push_back(p + t * (2 * mNrVerticesSphere) + 1);
                indices.push_back(p + (t - 1) * (2 * mNrVerticesSphere) + 1);
            }
        }
        // Join the lower cusp
        for (int p = 0; p < 2 * mNrVerticesSphere; ++p)
        {
            // Vertical lines
            indices.push_back(p + 1 + (mNrVerticesSphere - 2) * 2 * mNrVerticesSphere);
            indices.push_back(2 * (mNrVerticesSphere * mNrVerticesSphere - mNrVerticesSphere + 1) - 1);
        }

        // Bind the VAO and the VBO (as a verex buffer)
        glBindVertexArray(mSphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mSphereVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSphereEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to draw a sphere
    void GLAuxElements::drawSphere(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale,
                                const glm::mat4& view, const glm::mat4& projection)
    {
        // Compute the model matrix 
        glm::mat4 model { glm::translate(glm::mat4(1.), translation) };
        if (rotationAngle != 0.)
            model = glm::rotate(model, glm::radians(rotationAngle), 
                                       glm::normalize(rotationAxis));
        model = glm::scale(model, scale);
        
        // Set the model, view and projection matrices in the shader
        mLineShader.use();
        mLineShader.setMat4("model", model);
        mLineShader.setMat4("view", view);
        mLineShader.setMat4("projection", projection);

        // Draw the two vertices of the line
        glBindVertexArray(mSphereVAO);
        glDrawElements(GL_LINES, 2 * 2 * mNrVerticesSphere * (2 * mNrVerticesSphere - 1), 
                       GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    //==============================
    // Methods for drawing Cones
    //==============================

    // Method to set the VAO, VBO and properties of the line
    void GLAuxElements::setupCone()
    {
        // Create the Vertex array object
        glGenVertexArrays(1, &mConeVAO);
        // Create the Vertex buffer object
        glGenBuffers(1, &mConeVBO);
        // Create the Element buffer object
        glGenBuffers(1, &mConeEBO);

        // Lists of vertices and indices
        std::vector<float> vertices;
        std::vector<int> indices;
        vertices.reserve((mNrVerticesCone + 1) * 3);
        indices.reserve(3 * mNrVerticesCone);

        // Generate the vertices of the base
        for (int angle = 0; angle < mNrVerticesCone; ++angle)
        {
            vertices.push_back(0.5 * glm::cos(2. * glm::pi<float>() * (float)angle / mNrVerticesCone));
            vertices.push_back(-0.5);
            vertices.push_back(0.5 * glm::sin(2. * glm::pi<float>() * (float)angle / mNrVerticesCone));
        }
        // Vertex of the tip
        vertices.push_back(0.f);
        vertices.push_back(0.5f);
        vertices.push_back(0.f);

        // Generate the indices for the EBO
        for (int i = 0; i < mNrVerticesCone; ++i)
        {
            // Join vertices in the base
            indices.push_back(i);
            indices.push_back((i + 1) % mNrVerticesCone);

            // Join the vertices in the base with the tip
            indices.push_back(i);
            indices.push_back(mNrVerticesCone);
        }

        // Bind the VAO and the VBO (as a verex buffer)
        glBindVertexArray(mConeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mConeVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mConeEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Method to draw a line
    void GLAuxElements::drawCone(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale,
                                const glm::mat4& view, const glm::mat4& projection)
    {
        // Compute the model matrix 
        glm::mat4 model { glm::translate(glm::mat4(1.), translation) };
        if (rotationAngle != 0.)
            model = glm::rotate(model, glm::radians(rotationAngle), 
                                       glm::normalize(rotationAxis));
        model = glm::scale(model, scale);
        
        // Set the model, view and projection matrices in the shader
        mLineShader.use();
        mLineShader.setMat4("model", model);
        mLineShader.setMat4("view", view);
        mLineShader.setMat4("projection", projection);

        // Draw the two vertices of the line
        glBindVertexArray(mConeVAO);
        glDrawElements(GL_LINES, 3 * 2 * mNrVerticesCone, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
