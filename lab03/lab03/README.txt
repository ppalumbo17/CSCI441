CSCI441 / 598B, Computer Graphics, Fall 2016
Dr. Jeffrey Paone

Code Example: Lab03

	This program draws a Bezier Curve in 3D space.  In addition to the curve,
    the user is able to view the control points and cage for the curve.

Usage: 
    Pressing 'w' moves the camera forward and pressing 's' moves the camera back.

    Left click and dragging the mouse moves the camera around.

    The user can also press the 'q' or ESC key to quit the program.

Compilation Instructions:
    Simply navigate to the directory and type 'make.' main.cpp needs
    to be linked with the OpenGL / freeglut libraries and GLUI.  The program also
    makes use of the Point class defined in Point.h

Notes:
