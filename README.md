# shaderRender

ShaderRender is a program for shaderpixel rendering with hot-reload fragment shader, easy keyboard controls and usefull uniforms, inspeared from [https://www.shadertoy.com/](shadertoy.com)

##Installation
###Linux
+ You will thoses packages `libtool` `autoconf` `cmake` `xorg-dev` `libglu1-mesa-dev` `libbsd-dev`
+ and just `make` the project

###Mac OSX
+ You will need libtool, autoconf and cmake packages
+ and just `make` the project

###Windows
+ GL HF

##Usage
./visualishader \<shader file\> [optional texture channels]

##Uniforms
name | description
--- | ---
iResolution | dynamic resolution of the window
iGlobalTime | time in sec and form the program startup
iFrame | number of frame from the program startup
iMouse | x and y are the coordinates of the mouse, z and w are 1 if the mouse button is pressed
iScrollAmount | scroll amount of the mouse (usefull for a zoom)
iMoveAmount | x, y and z represent the 3d camera position and w the zoom (controlled with - and +)
iForward | a forward vector from the camera
iFractalWindow | window coordinates for fractal calcul
iChannel[0-7] | texture channels

##Controls
Positive | Negative | description
--- | --- | ---
`esc` | X | escape program
`right arrow` / `D` | `left arrow` / `A` | move horizontally
`up arrow` / `W` | `down arrow` / `S` | move vertically
`Q` | `E` | move on the z axis
`numpad +` / `=` | `numpad -` / `-` | move w coordinate
`space` | X | pause / unpause current time
`C` | X | lock / unlock mouse cursor
