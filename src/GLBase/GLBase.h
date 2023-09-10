// Add this file to the project to include all needed libraries and headers
// for a basis OpenGL application.

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <map>
#include <random>
#include <ctime>
#include <list>
#include <iterator>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "utils.h"
#include "application.h"
#include "light.h"
#include "deferredRenderer.h"
#include "camera.h"
#include "inputHandler.h"
#include "model.h"
#include "mesh.h"
#include "shader.h"
