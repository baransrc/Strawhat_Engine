# Strawhat Engine
Strawhat Engine is a game engine under construction for the final project of the master’s degree.

[**Repository**](https://github.com/baransrc/Strawhat_Engine) [**License**](https://github.com/baransrc/Strawhat_Engine/blob/main/LICENSE)


![image](https://user-images.githubusercontent.com/42971567/145306298-143724d2-572f-48f8-a243-df8fe5611332.png)


## Team for the Assignment 2
- [**I. Baran Surucu**](https://github.com/baransrc)
- [**I. Mónica Martín**](https://github.com/monikamnc)

## How It Works
Strawhat Engine is composed of a menu bar and 8 windows: Scene, Inspector, Hierarchy, Console, Controls, Modules, Performance and About. 
- **Scene:** A 3D viewer it’s going to be shown on this window where you can interact with the camera and entities through a Gizmo.
- **Inspector:** The properties of the components of the entity selected it’s going to be shown on this window. Through the buttons and sliders you can interact to change the entity position, rotation, scale and more.
- **Hierarchy:** All the entities that are in the scene / viewer 3D are going to be listed in this window. You can change each parent and children of each entity through dragging it with a left mouse click.
- **Console:** All the logs of the engine are displayed in a list on this window.
- **Controls:** A selector to change between Translate, Rotate, Scale and Local or World is going to be shown on this window. If you change it you will be able to see different types of gizmo on the entity selected.
- **Modules:** All render related information will be displayed as changing the color of the background 3D viewer and enable and disable: Smooth Lines, Face Culling, Depth, Scissor and Stencil.
- **Performance :** Checks the hardware of the system and the framerate milliseconds graphs.
- **About :** Shows relevant info about the engine, code and its developers.
In Strawhat Engine, you can move around the 3D viewer using Unity-like controls such as:
- **Movement and Rotation:** RMB + WASD and RMB + Mouse movement
- **Orbit:** LALT + LMB 
- **Smooth Focus:** F 
- **Smooth Zoom (Change fov):** Mouse Wheel
By selecting the entity through the hierarchy you will activate the gizmo that will be shown over the entity. There are 3 types of gizmo: **Translate** will have arrows, **Rotate** will be a sphere and **Scale** will be round arrows. You can change between them with the keys **T, R and S**. You can interact with gizmo by dragging with the left mouse click.
As in the last assignment you can drag and drop fbx files and they will be displayed on the scene window. Automatically they will be listed on the hierarchy with all its nodes as children.
With the left mouse click on the hierarchy window you can **create new empty entities** and then add its components in the inspector window.
You will find these types of components:
- **Camera:** Some important values about the camera will be displayed and editable such as z-near, z-far and horizontal fov.
- **Transform:** The base of each entity, here you can modify all local and world position, rotation and scale of each entity. Also there is a useful button **LookAt 0,0,0** that will make your entity focus its direction to the origin of the coordinate system of the scene.
- **Bounding Box:** You can read the position and the half size of the bounding box of the entity, also on the scene window you will see a green square around the entity.
- **Mesh:** Information about the mesh of the object.
- **Material:** All the different types of textures that have the mesh will be displayed as a little caption like: Diffuse, Specular and Occlusion.
- **Light:** You will be able to select through a dropdown menu the type of the light: **Directional**, **Point**, **Spotlight**.

## Notes
We have enjoyed and learned a lot creating this second assignment of the engine, we put a lot of effort into creating a consistent base to start work from it.
Although it is not visible, an event system has been created to allow the other coders of engine and gameplay programmers to decouple classes while keeping the dialog between them.
At the moment the engine supports one type of each light simultaneously: One Directional, One Point, One Spotlight and the Ambient light that always stays activated.
Each entity that has materials Diffuse and Specular will be sended to the shader to compile lights with them and get the best rendering possible with the physically based formula.

Models are property of Unity 3D from Survival Assets pack.
