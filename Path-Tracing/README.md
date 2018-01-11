# Path-Tracing Global Illumination

Code extended by Manuel Buchauer, Davide De Sclavis and Lukas Dötlinger 

## Description
This file demonstrates global illumination rendering based on the Pat-Tracing method employing Monte Carlo integration. Geometry is either represented by Spheres or Triangles. The code is extended from software by user Hole and Kevin Beason, released und
er the MIT License.

The extended code produces a rendered image `image.ppm`.

## Features 

The coded uses parallel computation with OpenMP.

There are different material available for an object in the scene.
DIFF: This is the diffuse material.
REFR: A material representing perfectly transmissive dielectric glas.
SPEC: A material representing a perfectly scpecular reflection resulting in a mirror.
GLOS: A material representing a glossy material that is not perfectly reflective.
TRSL: A translucent dielectric glas material that is not perfectly transmissive.

There is also the possibility to render an image with the included Thin-Lense to generate a depth of field effect. Values like aperture and focal length may be changed in the code. Although this is not possible, since they are already configured to generate a clearly visible depth of field effect.

## Run

To run the code, execute following commands:
``` shell 
make clean
make
./Radiosity
```
To get a less noisy picture, the number of samples can be increased by running:
 `./Radiosity n`
Where n tands for an integer number. We recommend 16 to get a fairly good image. Keep in mind that incrising samples also increases computation speed!

To run the code with the Thin-Lense, execute following command:
`./Radiosity n thin`