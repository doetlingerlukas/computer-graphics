# Semesterproject by Davide De Sclavis, Manuel Buchauer und Lukas Dötlinger

This is a animated Carousel for the Computer Graphics course
in the SS 2017.

Content of this README:

	- controls
	- texturing
	- execution of the program


## Controls

Following keyboard controls are available:
	
	- `1` : changes to camera mode 1, which is a rotating downfacing
			view from the top
	- `2` : changes to camera mode 2, which is a free controle mode, 
			if pressed again in mode 2, it will move to standard position
	- `0` : changes camera to default position
	- `p` : stops the view rotation, while in camera mode 1
	- `b` : stops the rotating of the carousel
	- `r` : changes the rotation direction
	- `+` : changes the speed of the rotation
	
	- `q` or `Q` : close the animation
	
Following mouse/keyboard commands are available if you switched to camera mode 2:

	- press any button on the mouse, while you move the mouse, to rotate 
			the camera view manually (only enabled in camera mode 2). 
	- `w` : move forward
	- `s` : move backward
	- `a` : move leftwards
	- `d` : move rightwards
	- `t` : move upwards
	- `g` : move downwards
	
Following keyboard controls are available for lightning/fog control:

	These commands will only change the color of the left light.
	We did this to give a static reference from the second light.
	- `6` : decrease the value of the color
	- `7` : increase the value of the color
	- `8` : decrease the hue of the color
	- `9` : increase the hue of the color
	
	Fog controls:
	- `f` : turn on/off fog
	- `m` : increase fog density
	- `n` : decrease fog density
	
	Lightning toggle controls:
	- `u` : turn on/off ambient lightning
	- `i` : turn on/off diffuse lightning
	- `o` : turn on/off specular lightning
	- `y` : turn on/off left lamp
	- `x` : turn on/off right lamp
	
	Switch lightning modes:
	- `l` : switch between 2 lightning modes
	- The second mode displays one light source inside the carousel.
	
	The values for ambient, diffuse and specular terms can be changed
	in the `Carousel.c` file. You will find those variables at the end
	of the first code segement (where all the other deklarations and 
	definitions are). We didn't add keyboard functions to increase/decrease
	those values, because we are slowly running out of keys. :)

## Texturing

This animated scene uses textures!
Every obj-file which is loaded can be correctly textured if it provieds
right **vertex-textures**. This is guaranteed by a custom function that
maps every vertex-texture to the right triangle (you can find this function
in the `Setup.c` file).
There are currently **3 different textures** mapped to objects. 
Keep in mind that the texture at the carousel is not very nice, since the
obj-file of the carousel has wrong vertex-textures!

We have also implemented 2 billboard textures. One tree and one cloud.
They are always facing the camera since they also rotate when the 
camera rotates, but they will always stay at the same position.
Keep in mind that this is only possible if you are in camera mode `2` 
and use the mouse (or trackpad) to rotate.

## Execution

This Project uses a Makefile to build and run it.
Navigate into the `Carousel` directory.
Now following commands are avaliable:

	- make: complie the programm
	- clean: removes all the compiled files
	- make run: This is the recommended command! 
					It builds and executes to programm!
