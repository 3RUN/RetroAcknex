# RetroAcknex

This shader collection was made with the Conitec GameStudio A8 game engine using HLSL and Lite-C. 
The main goal of this shader collection was to emulate PSone graphics.

This shader collection includes:
- vertex snapping
- affine texture mapping
- polygon cutoff
- surface darken on an angle (don't know how it's called)
- vertex lightning supporting up to 8 lights (pointlights + spotlights + sun)
- turbulence water
- fog support
- animated sprites
- multi-texture terrain (with up to 16 textures support)
- sky with fog and vertex snapping support
- chrome effect (could be used to make metal effect)
- environment mapping
- mirror floors (multiple)
- lens flares (from lights + sun)
- solid object visible through walls (don't know how it's called)
- NES (can be changed in .fx to GAMEBOY or EGA) palette color and dither (postprocessing)

PSone effects, could be toggled ON and OFF by commenting in .fx file for each material.

In order to make tiled surfaces look good, you need to add triangless to the mesh.
For level blocks, this could be done by increasing "Lightmap resolution" while compiling the map.

You can see how it looks like here:
https://youtu.be/UIzDkqpYXgw

CREDITS:

- NES palette color and dither created by Coldberg. Original can be found here: https://www.shadertoy.com/view/MdfXDH
- Prete aka Priest for creating those awesome lens-flare effects and lens flare images (which may not be distributed commercially).
- Txesmi (forum member) thank you for helping me out with shaders! This whole shader pipeline 95% based on his work.
- Kartoffel thank you for the simple effect that darkens a surface if you look at it at an angle.
- vehicle model by Eliederson (email: eliederson (at) isbt (dot) com (dot) br)
- skycube by MFG Thomas (email thomas-koehler (at) onlinehome (dot) de)
- environmental, chrome and mirror floor shaders are based on native shaders, that came with GameStudio A8.
- dsoft20 - I took calculations for vertex snapping, affine texture mapping and cut out polygons from their awesome shader pack and migrated them into acknex engine.
Their shader pack can be found here: 
https://github.com/dsoft20/psx_retroshader

![Alt text](https://github.com/3RUN/RetroAcknex/blob/master/screenshots/shot_3.jpg?raw=true "Preview.")