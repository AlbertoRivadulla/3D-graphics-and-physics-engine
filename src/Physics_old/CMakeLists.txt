cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)

# Find the GLFW library, needed for rendering to the screen with OpenGL
find_package(glfw3 3.3 REQUIRED)
# Find the OpenGL library. The first line is needed, otherwise cmake will complain.
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
# # Find the package assymp, used to load models
# find_package(ASSIMP REQUIRED)
# if(ASSIMP_FOUND)
#     include_directories(${ASSIMP_INCLUDE_DIR})
# endif()
find_package( Freetype REQUIRED )

project(project)

# Create a variable with all the include directories
set(INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty

    ${CMAKE_CURRENT_SOURCE_DIR}/../GLBase
    ${CMAKE_CURRENT_SOURCE_DIR}/../GLBase/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../GLBase/thirdparty

    ${CMAKE_CURRENT_SOURCE_DIR}/../GLGeometry
    ${CMAKE_CURRENT_SOURCE_DIR}/../GLGeometry/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../GLGeometry/thirdparty

    ${FREETYPE_INCLUDE_DIRS}
)

include_directories(${INCLUDE})

# Define a variable with all the libraries
set(LIBS PUBLIC 
    glfw 
    OpenGL::GL 
    glad 
    ${CMAKE_DL_LIBS}
    ${ASSIMP_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

# Create a variable with a link to all cpp files to compile
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicsWorld.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicsBody.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Collider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SphereCollider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlaneCollider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ConvexCollider.cpp
)

# Create the library
add_library(Physics ${SOURCES})

# Link the other libraries to this one
target_link_libraries(Physics ${LIBS})

# Configure the include directories defined above.
# Setting the second argument to INTERFACE (or PUBLIC) allows other programs that
# link to this library to use these same include directories.
target_include_directories(Physics INTERFACE ${INCLUDE})

# Set the path to the folder where libraries are located.
# This is needed for the library Assimp (for loading 3D models)
target_link_directories(Physics INTERFACE /usr/local/lib)
