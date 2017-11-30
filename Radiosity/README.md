# Radiosity Global Illumination

Code extended by Manuel Buchauer, Davide De Sclavis and Lukas Dötlinger 

## Description
This file demonstrates global illumination renderingbased on the radiosity method. The geometry (triangles) is divided into patchesfor which the form factors are determined employing Monte Carlo integration. Radiosity values for the patches are computed withan iterative solver. The final image (i.e. radiance) is obtained via tracing rays into the scene. Two output files are saved - one with constant shading of patches and one with bicubic colorinterpolation. The code is extended from software by user Hole and Kevin Beason, released under the MIT License.

The extended code produces two images, an image with the calculated patches and an image, which is a smoothed version of the first picture using barycentric interpolation.

## Run

To run the code you can do one of the following options.
``` shell 
make clean
make
./Radiosity
```
or
``` shell 
make run
```
We recommend to use the seciond option.
