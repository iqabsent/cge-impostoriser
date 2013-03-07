# MSc CGE (Tools & Middleware - tool) Impostoriser #
==============

Make impostors of a .geom file.

By: Willem B. van der Merwe
33245888

!!Requires a working libjpeg compilation!!


Fails to deliver .. I suspect a corrupted libjpeg dll .. will recompile before demo & hope that fixes it..

To be used as command-line tool.

Usage: impostorise.exe [source] [horizontal steps] [vertical steps]
- [source] must be a .geom file
- [horizontal steps] number of angles horizontally (4 + multiple of 4)
- [vertical steps] number of angles vertically (3 + multiple of 2)

Defaults used - [assets/duck.geom] [8] [5]

Outputs to impostors.jpg


## Code Layout ##
Single .cpp file (main.cpp)

### render() ###
Function to render loops for amount of vertical and horizontal angles specified and generates images from different camera angles in 100 x 100 pixel "viewport" sections of the window.

### screendump ###
Function to use libjpeg, writing all pixels from a glReadPixels() call to a .jpg file
