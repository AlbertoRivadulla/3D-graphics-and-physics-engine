cmake_minimum_required(VERSION 3.7)

# Find the GLFW library, needed for rendering to the screen with OpenGL
find_package(glfw3 3.3 REQUIRED)
# Find the OpenGL library. The first line is needed, otherwise cmake will complain.
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
# Find the package assymp, used to load models
find_package(ASSIMP REQUIRED)
# if(ASSIMP_FOUND)
#     include_directories(${ASSIMP_INCLUDE_DIR})
# endif()

project(project)

# Create a variable with all the include directories
set(INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty

    ${CMAKE_CURRENT_SOURCE_DIR}/../GLBase
    ${CMAKE_CURRENT_SOURCE_DIR}/../GLBase/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../GLBase/thirdparty
)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${INCLUDE})

# Define a variable with all the libraries
set(LIBS PUBLIC 
    glfw 
    OpenGL::GL 
    glad 
    ${CMAKE_DL_LIBS}
    ${ASSIMP_LIBRARIES}
)

# Create a variable with a link to all cpp files to compile
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLQuad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLCube.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLSphere.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLCylinder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLCone.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLPolyhedron.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLCubemap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLAuxElements.cpp
)

# Create the library
add_library(GLGeometry ${SOURCES})

# Link the other libraries to this one
target_link_libraries(GLGeometry ${LIBS})

# Configure the include directories defined above.
# Setting the second argument to INTERFACE (or PUBLIC) allows other programs that
# link to this library to use these same include directories.
target_include_directories(GLGeometry INTERFACE ${INCLUDE})

# Set the path to the folder where libraries are located.
# This is needed for the library Assimp (for loading 3D models)
target_link_directories(GLGeometry INTERFACE /usr/local/lib)
