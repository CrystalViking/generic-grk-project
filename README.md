## Table of contents
- [Table of contents](#table-of-contents)
- [General info](#general-info)
- [Technologies](#technologies)
- [Setup](#setup)
- [Sources & Inspiration](#sources--inspiration)
    - [Skybox](#skybox)
    - [Particles](#particles)

[link text itself]: http://www.reddit.com


## General info
Project made for "Computer graphics" UAM WMI 2020/2021 course.

It features:
* 2 light sources (the Sun and the flashlight from spaceship)
* various 3D objects – spaceship, planets, orbiting comets...
* objects' animations (planets' rotations, etc.)
* normal mapping (on spaceship, Earth and asteroids)
* keyboard & mouse control
* (beautiful[*][skybox credits]) skybox
* particle effect (on spaceship engines and the comet's tail)
* interactiveness (time-attack mode race implementation)
* "bonus" parts: multisample anti-aliasing (MSAA), 60fps optimization lock, hidden mouse cursor with lock inside window, correct window scaling

Unfortunately this project does not include the physics implementation, nor does it intend to do so. Procedural terrain generation may be added in the future.

[skybox credits]: https://forum.kerbalspaceprogram.com/index.php?/topic/177203-17x-darkfighters-skyboxes-red-spiders/


## Technologies
* freeglut
* objload
* GLSL
* GLEW (?)


## Setup
Clone this repo to your computer and open `grk-project.vcxproj` (tested w/ Visual Studio 2019). Run project in 'Release' if 'Debug' takes too much time (skybox textures are in 4k). 


## Sources & Inspiration
List of various sources that proved immensely helpful in our project :yellow_heart:

#### Skybox
[Skybox on learnopengl.com](https://learnopengl.com/Advanced-OpenGL/Cubemaps)

#### Particles
[Particles on opengl-tutorial.org](http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/)\
[Christian Behler on medium.com](https://levelup.gitconnected.com/how-to-create-instanced-particles-in-opengl-24cb089911e2)