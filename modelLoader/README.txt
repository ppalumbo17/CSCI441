CSCI 441, Computer Graphics, Fall 2015
Jeffrey Paone

Code Example: modelLoader

    This program reads in and renders a WaveFront object (*.obj) file, a GEOMVIEW *.off
    file, a Stanford *.ply file, or a STL *.stl file.  Models that include texture
    coordinates and have an accompanying diffuse texture, will be properly mapped.  The included
    .h and .cpp files (Object, Face, PointBase, Point, Vector, Matrix, Material ) handle 
    all the behind the scenes magic.

Usage: 
    Pressing 'w' 'a' 's' 'd' moves the object around the grid.  

    Pressing 'l' rotates the light source type between a Point Light, Directional
    Light, and Spot Light.  

    Left click and dragging the mouse moves the camera around while right click
    and dragging zooms the camera in or out.

    The user can also press the 'q' key to quit the program.

Compilation Instructions:
    Simply navigate to the directory and type 'make.' Needs
    to be linked with the OpenGL / freeglut and SOIL libraries.

Notes:

TO DO Items:
    > Handle specular maps
    > Handle bump/normal maps
    > Replace with VBOs
    > Make getFaces() more efficient
