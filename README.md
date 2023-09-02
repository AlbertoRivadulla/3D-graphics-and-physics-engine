# Simple 3D game engine in OpenGL

A simple engine for 3-dimensional games and physics simulations written in C++.
The graphics are rendered using OpenGL, by means of the open source GLFW library.

## List of features

### Graphics

- Deferred rendering
    - The properties of each object are given by its material, which specifies
    the albedo color, specular and emissive intensities.
- Different light sources
    - Directional lights
    - Point lights
    - Spot lights
- Cascaded shadow maps
- Text and simple GUI rendering to the screen

### Physics engine

- Work in progress

## Examples

- Sandbox [link](examples/Sandbox)

## References

- [Learn OpenGL](https://learnopengl.com/)
- "Game Physics Engine Development" by Ian Millington
