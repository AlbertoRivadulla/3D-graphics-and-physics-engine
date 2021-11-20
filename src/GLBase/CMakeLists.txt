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

# Create a variable with all the include directories
set(INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty

    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

include_directories(${INCLUDE})

# Add GLAD as a library. It's code is located inside the src/thirdparty folder
# of the project
add_library(glad thirdparty/glad.c)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inputHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deferredRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp

    # Auxiliary source file needed for stb_image.h to work
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stb_image.cpp
)

# Create the library
add_library(GLBase ${SOURCES})

# Link the other libraries to this one
target_link_libraries(GLBase ${LIBS})

# Configure the include directories defined above.
# Setting the second argument to INTERFACE (or PUBLIC) allows other programs that
# link to this library to use these same include directories.
target_include_directories(GLBase INTERFACE ${INCLUDE})

# Set the path to the folder where libraries are located.
# This is needed for the library Assimp (for loading 3D models)
target_link_directories(GLBase INTERFACE /usr/local/lib)
