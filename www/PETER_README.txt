Peter Palumbo / ppalumbo@mymail.mines.edu 
God Squad / Kyle Thistlewood, Peter Paulumbo, Corbin Olds
Code Submitted by Kyle Thistlewood
Midterm Project - Guild Wars

This program runs a bezier curve with arc parameterization and equal step size. We also use a bezier surface to draw our grid.
We also can move around the world with a wanderer hero. On top of all of that we have cameras than can switch from arc ball, to free, to first person.

Use: w,a,s,d to move wandering hero, use w,a,s,d to move free cam in free cam, first person and arc ball use the mouse clicked in to move around the world.
Right click on the mouse to pull up the menus. Q to quit

Compile: To run the files due  do ./guildWars.exe "filename"

Bugs: The only thing we didn't get to was not being able to have our characters facing the normal.
There is a bug with my "horn of my car being huge at the start. Don't know why this is, but once
the program starts it works normally.

File format:
Note the spaces in the file:

<number of Bézier Surfaces for ground>

<sixteen control points for each surface>
...
<number of control points for Bézier Curve track>

<control point x> <control point y> <control point z>
...
<number of objects>

<object type> <object x, y, z> <object orientation x, y, z> <object size>
...

Rough Draft Of Responsibilites/Contributions of each student:

I did the bezier patch;
I did the same step bezier curve;
I moved the character based on his position in the bezier surface;
I did the text over the characters;

Time: 25+ hours

Lab Help: 6? - I guess the bezier curve stuff helped a little?

Fun: 5 - This was too hard and long to be fun
