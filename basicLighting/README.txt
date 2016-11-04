CSCI 441, Computer Graphics, Fall 2015
Jeffrey Paone

Code Example: Lighting

    This program is a simple example of lighting in OpenGL.  A white light is
    set and material properties as well.  The user can toggle what type of 
    light (Point, Directional, Spot) is used.  Use this progam as a test to
    change lighting colors and material colors to see the resultant color.

Usage: 
    Pressing 'w' 'a' 's' 'd' moves the object around the grid.  

    Pressing the number keys 1-9 changes the object drawn.  Space bar converts
    the objects to wire models and the 'c' key displays GLU quadrics as a 
    silhouette.

    Pressing 'l' rotates the light source type between a Point Light, Directional
    Light, and Spot Light.  

    Left click and dragging the mouse moves the camera around while right click
    and dragging zooms the camera in or out.

    The user can also press the 'q' key to quit the program.

Compilation Instructions:
    Simply navigate to the directory and type 'make.' Only main.cpp needs
    to be linked with the OpenGL / freeglut libraries.

Notes:
    This example was worked through in class.
