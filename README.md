# 3D graphics and physics engine

A simple engine for 3-dimensional games and physics simulations written from
scratch in C++.
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
- Fog effect added in postprocessing
- Text and simple GUI rendering to the screen

### Physics engine

- Ballistic movement of objects
- Particle system

## Examples

- Sandbox [link](examples/Sandbox)

## References

- [Learn OpenGL](https://learnopengl.com/)
- "Game Physics Engine Development" by Ian Millington
