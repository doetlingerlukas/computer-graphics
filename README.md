# Semesterproject by Davide De Sclavis, Manuel Buchauer und Lukas Dötlinger

This is a animated Carousel for the Computer Graphics course
in the SS 2017.

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

## Execution

This Project uses a Makefile to build and run it.
Navigate into the `Carousel` directory.
Now following commands are avaliable:

	- make: complie the programm
	- clean: removes all the compiled files
	- make run: This is the recommended command! 
					It builds and executes to programm!
