Peter Palumbo/ Cyclopalypse Now
Assignment 2
Computer Graphics
September 7, 2016


This program creates my hero, a cyclops.  He has the ability to move left and right.  He is friendly so he is constantly waving.  Also he is a rogue (more on that later) so he can throw shurikens.

Usage:
Keybinds: 
w= pause arm wave
a= move left
d= move right
c= throw shuriken
mouse click= throw shuriken at location of mouse pointer (Note it doesn't follow the mouse. That's a feature not a bug. Shurikens aren't magic)

Compiling:
You should be able to run make or gmake and will see an assignment2.exe file.  Run that to bring up the program.

This assignment took roughly 6 hours to complete.
The lab was relatively helpful for this assignment, I'd give it a 6.
This assignment was fun, however was a bit frustrating at times and I wish I had more time to make things look pretty. A 7.

Questions:
1) 
glPushMatrix();
glTranslatef(windowWidth, windowHeight, 0);
glRotatef(angle, ...)
glTranslatef(-windoWidth, -windowHeight, 0);
glPopMatrix();

2)
glPushMatrix();
glTranslatef(i, j, 0);
glRotatef(angle, ...)
glTranslatef(-i, -j, 0);
glPopMatrix();

3)
GLfloat m[16] = {some matrix}
glMultMatrixf(m);