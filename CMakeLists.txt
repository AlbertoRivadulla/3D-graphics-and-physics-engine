cmake_minimum_required(VERSION 3.7)

# Obtain a file compile_commands.json used by ccls (through the plugin coc.nvim)
# to provide code completion in Neovim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Name of the project
project(project)

# Create a variable with all the include directories
set(INCLUDE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/thirdparty
    ${PROJECT_SOURCE_DIR}/src/GLBase
    ${PROJECT_SOURCE_DIR}/src/GLGeometry
    ${PROJECT_SOURCE_DIR}/src/GLSandbox

    # If the main application only inports GLBase, and this itself imports all
    # needed dependencies, the following two lines should not be necessary
    # ${PROJECT_SOURCE_DIR}/src/GLBase/src
    # ${PROJECT_SOURCE_DIR}/src/GLBase/thirdparty
)

include_directories(${INCLUDE})

# Create a variable with a link to all cpp files to compile
set(SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/src/GLSandbox/GLSandboxBase.cpp
    ${PROJECT_SOURCE_DIR}/src/GLSandbox/GLSandbox.cpp
)

add_executable(main ${SOURCES})

# The subdirectory of the library GLBase, created by me
add_subdirectory(src/GLBase)
# The subdirectory of the library GLGeometry, created by me
add_subdirectory(src/GLGeometry)
# Link to the libraries
target_link_libraries(main GLBase GLGeometry)

# Get rid of the cmake_install.cmake file created
set(CMAKE_SKIP_INSTALL_RULES True)

# Move the file compile_commands.json to the directory of the project.
execute_process(COMMAND cp compile_commands.json ..) 

