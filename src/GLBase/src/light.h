#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "GLBase.h"
#include "GLGeometry.h"

namespace GLBase
{
    class Light
    {
        protected:
            // Constructor
            Light(glm::vec3 color, glm::vec3 position, float attenConst,
                  float intensity, float attenQuadratic, int shadowRes);

            // Position of the light
            glm::vec3 mPosition;

            // Color of the light
            glm::vec3 mColor;

            // Attenuation constants
            float mIntensity;
            float mAttenLinear;
            float mAttenQuadratic;

            // Shadow map framebuffer
            unsigned int mShadowMapFBO;
            // Shadow map texture
            unsigned int mShadowMapTexture;
            // Resolution of the shadow map
            int mShadowMapResolution;
            // Light space matrix
            glm::mat4 mLightSpaceMatrix;

            // Method to configure the shadow map framebuffer and texture
            void setupShadowMap();

        public:
            // Method to get the position of the light
            inline glm::vec3 getPosition()
            {
                return mPosition;
            }

            // Method to compute the shadow map
            // This needs a list of objects, which should include their model matrix
            virtual void computeShadowMap(const Shader& shader, const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow) = 0;

            // Method to pass the light to a shader
            virtual void configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                         unsigned int& indexSpot, unsigned int indexPoint,
                                         unsigned int& indexShadow) const = 0;

            // Method to pass the lightSpaceMatrix to a shader
            virtual void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int indexPoint,
                                                   unsigned int& indexShadow) const = 0;
    };

    class DirectionalLight : public Light
    {
        private:
            // Direction of the light
            glm::vec3 mDirection;

            // Method to compute the light space matrix from a camera
            void computeLightSpaceMatrix(const Camera& camera);

        public:
            // Constructor
            DirectionalLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction,
                             float intensity, float attenLinear, float attenQuadratic,
                             // int shadowRes = 1024);
                             int shadowRes = 10024);

            // Method to compute the shadow map
            void computeShadowMap(const Shader& shader, const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to pass the light to a shader
            void configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                 unsigned int& indexSpot, unsigned int indexPoint,
                                 unsigned int& indexShadow) const;

            // Method to pass the lightSpaceMatrix to a shader
            void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int indexPoint,
                                                   unsigned int& indexShadow) const;
    };

    class SpotLight : public Light
    {
        private:
            // Direction of the light
            glm::vec3 mDirection;
            // Cosine of the inner and outer angles
            float mCosAngleInner;
            float mCosAngleOuter;
            // Maximum distance reached by the light
            float mRadiusMax;

        public:
            // Constructor
            SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction,
                      float angleInner, float angleOuter,
                      float intensity, float attenLinear, float attenQuadratic,
                      int shadowRes = 1024);

            // Method to compute the shadow map
            void computeShadowMap(const Shader& shader, const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to pass the light to a shader
            void configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                 unsigned int& indexSpot, unsigned int indexPoint,
                                 unsigned int& indexShadow) const;

            // Method to pass the lightSpaceMatrix to a shader
            void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int indexPoint,
                                                   unsigned int& indexShadow) const;
    };

    class PointLight : public Light
    {
        private:
            // Maximum distance reached by the light
            float mRadiusMax;

        public:
            // Constructor
            PointLight(glm::vec3 color, glm::vec3 position,
                       float intensity, float attenLinear, float attenQuadratic,
                       int shadowRes = 1024);

            // Method to compute the shadow map
            void computeShadowMap(const Shader& shader, const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to pass the light to a shader
            void configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                 unsigned int& indexSpot, unsigned int indexPoint,
                                 unsigned int& indexShadow) const;

            // Method to pass the lightSpaceMatrix to a shader
            void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int indexPoint,
                                                   unsigned int& indexShadow) const;
    };
}

#endif
