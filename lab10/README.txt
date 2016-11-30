CSCI 441, Computer Graphics, Fall 2016
Peter Palumbo

Code Example: Lab10

    This program is a simple demonstration of transparent (RGBA) textures
    in OpenGL. A transparent texture is read in from two image files 
    (a PPM for the RGB data and then a PPM with the alpha data) and combined
    into a single RGBA array that is then registered as a texture with OpenGL.

    In order to correctly render transparency in OpenGL, the transparent
    quads/triangles must be sorted EVERY FRAME, based on the position
    of the viewer.
    
    The transparent quads are then rotated to face the camera EVERY FRAME.

Usage: 
    The left mouse button allows the user to pan around the center of the 
    forest.  The right mouse button allows the user to zoom in and out.

    The user can also press the 'q' key to quit the program.

Compilation Instructions:
    Simply navigate to the directory and type 'make.'

Questions:
1) Yeah!
2) It looked good until I got to a certain point and then it would go flat for a bit.
3) Yes!
4) Yes, kinda.
5) Yep no more see through.
6) After a while, yes.
7) Yes it was, it's cool to see how illusions can make it easier for the computers workload.
8) The write up for this lab was just about right.
9) This lab took me about three hours.
