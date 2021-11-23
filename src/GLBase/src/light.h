#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "GLBase.h"
#include "GLGeometry.h"

namespace GLBase
{
    // Enum for the different types of lights
    enum LightType
    {
        LIGHT_DIRECTIONAL,
        LIGHT_POINT,
        LIGHT_SPOT
    };

    class Light
    {
        protected:
            // Constructor
            Light(glm::vec3 color, glm::vec3 position, float attenConst,
                  float intensity, float attenQuadratic, int shadowRes,
                  LightType lightType);

            // Enum for the type of light
            LightType mLightType;

            // Position of the light
            glm::vec3 mPosition;

            // Color of the light
            glm::vec3 mColor;

            // Attenuation constants
            float mIntensity;
            float mAttenLinear;
            float mAttenQuadratic;

            // Pointer to a shader for shadow mapping
            Shader* mShadowShader;

            // Shadow map framebuffer
            unsigned int mShadowMapFBO;
            // Shadow map texture
            unsigned int mShadowMapTexture;
            // Resolution of the shadow map
            int mShadowMapResolution;
            // Light space matrix
            glm::mat4 mLightSpaceMatrix;

            // Method to configure the shadow map framebuffer and texture
            virtual void setupShadowMap() = 0;

        public:
            // Method to get the position of the light
            inline glm::vec3 getPosition()
            {
                return mPosition;
            }

            // Method to check the type of the light
            inline LightType getLightType()
            {
                return mLightType;
            }

            // // Method to set the pointer to the shader
            // void setShadowShader(Shader* shader)
            // {
            //     mShadowShader = shader;
            // }

            // Method to compute the shadow map
            // This needs a list of objects, which should include their model matrix
            virtual void computeShadowMap(const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow) = 0;

            // Method to pass the light to a shader
            virtual void configureShader(const Shader& lightingShader, 
                                         Shader* shadowShader, 
                                         unsigned int& indexDirectional, 
                                         unsigned int& indexSpot, 
                                         unsigned int& indexPoint,
                                         unsigned int& indexShadow) = 0;

            // Method to pass the lightSpaceMatrix to a shader
            virtual void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int& indexPoint,
                                                   unsigned int& indexShadow) const = 0;
    };

    class DirectionalLight : public Light
    {
        private:
            // Direction of the light
            glm::vec3 mDirection;
            // Number of levels in the shadow cascade
            unsigned int mNrShadowCascadeLevels;

            // Uniform buffer object for the light space matrices of the different subfrustums
            static unsigned int mLightMatricesUBO;
            // Vector of the light space matrices for the differnt subfrustums
            std::vector<glm::mat4> mLightSpaceMatrices;
            // Distances of the near plane of the different levels of the cascade
            std::vector<float> mShadowCascadeDistances;

            // Method to configure the shadow map framebuffer and texture
            void setupShadowMap();

            // Method to compute the light space matrix from a camera
            void computeLightSpaceMatrix(const Camera& camera, int index);

        public:
            // Constructor
            DirectionalLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction,
                             float intensity, float attenLinear, float attenQuadratic,
                             int shadowRes = 2096, unsigned int nrShadowCascadeLevels = 4);

            // Method to compute the shadow map
            void computeShadowMap(const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to pass the light to a shader
            void configureShader(const Shader& lightingShader, 
                                 Shader* shadowShader, 
                                 unsigned int& indexDirectional, 
                                 unsigned int& indexSpot, 
                                 unsigned int& indexPoint,
                                 unsigned int& indexShadow);

            // Method to pass the lightSpaceMatrix to a shader
            void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int& indexPoint,
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

            // Method to configure the shadow map framebuffer and texture
            void setupShadowMap();

            // // Method to compute the light space matrix from a camera
            // void computeLightSpaceMatrix(const Camera& camera);

        public:
            // Constructor
            SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction,
                      float angleInner, float angleOuter,
                      float intensity, float attenLinear, float attenQuadratic,
                      int shadowRes = 1024);

            // Method to compute the shadow map
            void computeShadowMap(const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to pass the light to a shader
            void configureShader(const Shader& lightingShader, 
                                 Shader* shadowShader, 
                                 unsigned int& indexDirectional, 
                                 unsigned int& indexSpot, 
                                 unsigned int& indexPoint,
                                 unsigned int& indexShadow);

            // Method to pass the lightSpaceMatrix to a shader
            void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int& indexPoint,
                                                   unsigned int& indexShadow) const;
    };

    class PointLight : public Light
    {
        private:
            // Maximum distance reached by the light
            float mRadiusMax;

            // Method to configure the shadow map framebuffer and texture
            void setupShadowMap();
            //
            // // Method to compute the light space matrix from a camera
            // void computeLightSpaceMatrix(const Camera& camera);

        public:
            // Constructor
            PointLight(glm::vec3 color, glm::vec3 position,
                       float intensity, float attenLinear, float attenQuadratic,
                       int shadowRes = 1024);

            // Method to compute the shadow map
            void computeShadowMap(const Camera& camera,
                                  const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to pass the light to a shader
            void configureShader(const Shader& lightingShader, 
                                 Shader* shadowShader, 
                                 unsigned int& indexDirectional, 
                                 unsigned int& indexSpot, 
                                 unsigned int& indexPoint,
                                 unsigned int& indexShadow);

            // Method to pass the lightSpaceMatrix to a shader
            void configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                                   unsigned int& indexSpot, unsigned int& indexPoint,
                                                   unsigned int& indexShadow) const;
    };
}

#endif
