cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)

project(project)

# Create a variable with all the include directories
set(INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src

    # ${FREETYPE_INCLUDE_DIRS}
)

include_directories(${INCLUDE})

# Define a variable with all the libraries
set(LIBS PUBLIC 
)

# Create a variable with a link to all cpp files to compile
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Create the library
add_library(Utils ${SOURCES})

# Link the other libraries to this one
target_link_libraries(Utils ${LIBS})

# Configure the include directories defined above.
# Setting the second argument to INTERFACE (or PUBLIC) allows other programs that
# link to this library to use these same include directories.
target_include_directories(Utils INTERFACE ${INCLUDE})
