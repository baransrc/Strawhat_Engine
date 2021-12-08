# Strawhat Engine
Strawhat Engine is a game engine under construction that has model loading and camera movement features along with an editor.

[**Repository**](https://github.com/baransrc/Strawhat_Engine) [**License**](https://github.com/baransrc/Strawhat_Engine/blob/main/LICENSE)


![image](https://user-images.githubusercontent.com/42971567/145306298-143724d2-572f-48f8-a243-df8fe5611332.png)


## Team for the Assignment 1
- [**I. Baran Surucu**](https://github.com/baransrc)

## How It Works
Strawhat Engine currently supports drag and dropping of Model files. If the dropped file is not a model file, it will reload it's default model, a cute lovely Baker House.
In Strawhat Engine, you can move around using Unity-like controls such as:
- **Movement and Rotation:** RMB + WASD and RMB + Mouse movement
- **Orbit:** LALT + LMB 
- **Smooth Focus:** F 
- **Smooth Zoom (Change fov):** Mouse Wheel

## Notes
Some features such as Camera Movement, Zoom and Focus were crafted with extra care to achieve a satisfying experience while using the engine. Although the engine uses Quaternions to form Rotations, there is a Gimbal Lock occurring when X and Z axises align at 90 and -90 degrees. Overall, it is a pleasure to create this engine, and I'm looking forward to the next releases.

Models are created by Clàudia Escuder.
