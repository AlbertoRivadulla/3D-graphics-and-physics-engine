#ifndef GLCOMPOSED_OBJECT_H
#define GLCOMPOSED_OBJECT_H

// #include "GLElemObject.h"
//
// using namespace GLBase;
//
// namespace GLGeometry {
// class GLComposedObject : public GLElemObject {
//     struct ObjectAndTransform {
//         std::unique_ptr<GLElemObject> object;
//         glm::mat4 transformMatrix;
//
//         void setTransform(const glm::vec3 &relativePos, const glm::vec3 &relativeScale, float rotationAngle,
//                           const glm::vec3 &rotationAxis) {
//             transformMatrix = glm::mat4(1.f);
//             transformMatrix = glm::translate(transformMatrix, relativePos);
//             transformMatrix = glm::rotate(transformMatrix, rotationAngle, rotationAxis);
//             transformMatrix = glm::scale(transformMatrix, relativeScale);
//         }
//     };
//
// public:
//     GLComposedObject();
//
//     template <typename T, typename... Args>
//     ObjectAndTransform *addComponent(const glm::vec3 &relativePos, const glm::vec3 &relativeScale, float rotationAngle,
//                                      const glm::vec3 &rotationAxis, Args &&...args) {
//         static_assert(std::is_base_of<GLGeometry::GLElemObject, T>::value,
//                       "T must derive from GLGeometry::GLElemObject");
//
//         auto component = ObjectAndTransform(std::make_unique<T>(std::forward<Args>(args)...), glm::mat4(1.f));
//         component.setTransform(relativePos, relativeScale, rotationAngle, rotationAxis);
//
//         mComponents.push_back(std::move(component));
//
//         return &mComponents.back();
//     }
//
//     void draw(GLBase::Shader *shader);
//
// private:
//     std::vector<ObjectAndTransform> mComponents;
// };
// } // namespace GLGeometry

#endif
