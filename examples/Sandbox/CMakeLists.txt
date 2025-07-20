cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)

# Obtain a file compile_commands.json used by ccls (through the plugin coc.nvim)
# to provide code completion in Neovim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_FLAGS "-O3 -Wall")

# Set the log level (0=ERROR, 1=WARNING, 2=INFO, 3=DEBUG)
add_compile_definitions(GLOBAL_LOG_LEVEL=3)

# Name of the project
project(Sandbox)

# Root directory of the library source code
set( LIBRARY_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../.. )

# Define base directories of shaders and resources as preprocessor macros
add_compile_definitions(BASE_DIR_RESOURCES="${PROJECT_SOURCE_DIR}/../../resources")
add_compile_definitions(BASE_DIR_SHADERS="${PROJECT_SOURCE_DIR}/../../shaders")

# Create a variable with all the include directories
set(INCLUDE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/thirdparty
    ${LIBRARY_SOURCE_DIR}/src/GLBase
    ${LIBRARY_SOURCE_DIR}/src/GLGeometry
    ${LIBRARY_SOURCE_DIR}/src/GLSandbox
    ${LIBRARY_SOURCE_DIR}/src/Physics
    ${LIBRARY_SOURCE_DIR}/src/Utils

    # ${PROJECT_SOURCE_DIR}/src/GLBase
    # ${PROJECT_SOURCE_DIR}/src/GLGeometry
    # ${PROJECT_SOURCE_DIR}/src/GLSandbox
    # ${PROJECT_SOURCE_DIR}/src/Physics

    # If the main application only imports GLBase, and this itself imports all
    # needed dependencies, the following two lines should not be necessary
    # ${PROJECT_SOURCE_DIR}/src/GLBase/src
    # ${PROJECT_SOURCE_DIR}/src/GLBase/thirdparty
)

include_directories(${INCLUDE})

# Create a variable with a link to all cpp files to compile
set(SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/src/SandboxBase.cpp
    ${PROJECT_SOURCE_DIR}/src/Sandbox.cpp
)

add_executable(main ${SOURCES})

# The subdirectory of the library GLBase, created by me
# add_subdirectory(src/GLBase)
# add_subdirectory(${LIBRARY_SOURCE_DIR}/src/GLBase)
add_subdirectory(${LIBRARY_SOURCE_DIR}/src/GLBase GLBase)
# The subdirectory of the library GLGeometry, created by me
# add_subdirectory(src/GLGeometry)
# add_subdirectory(${LIBRARY_SOURCE_DIR}/src/GLGeometry)
add_subdirectory(${LIBRARY_SOURCE_DIR}/src/GLGeometry GLGeometry)
# The subdirectory of the library Physics, created by me
# add_subdirectory(src/Physics)
# add_subdirectory(${LIBRARY_SOURCE_DIR}/src/Physics)
add_subdirectory(${LIBRARY_SOURCE_DIR}/src/Physics Physics)
# Link to the libraries
target_link_libraries(main GLBase GLGeometry Physics)

# Get rid of the cmake_install.cmake file created
set(CMAKE_SKIP_INSTALL_RULES True)

# Move the file compile_commands.json to the directory of the project.
execute_process(COMMAND cp compile_commands.json ..) 
