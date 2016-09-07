/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab00a
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2016
 *
 *  Description:
 *      Contains the code for a simple 2D OpenGL / GLUT example.
 *
 */

#ifdef __APPLE__				// if compiling on Mac OS
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
#else							// if compiling on Linux or Windows OS
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#endif

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 512, windowHeight = 512;

// global variable to keep track of the window id
int windowId;

//
//  void changeSize(int w, int h)
//
//      We will register this function as GLUT's reshape callback.
//   When the window is resized, the OS will tell GLUT and GLUT will tell
//   us by calling this function - GLUT will give us the new window width
//   and height as 'w' and 'h,' respectively - so save those to our global vars.
//
void changeSize( int w, int h ) {
    //save the new window width / height that GLUT informs us about
    windowWidth = w;
    windowHeight = h;

    // update the projection matrix based on the window size
    // the GL_PROJECTION matrix governs properties of the view;
    // i.e. what gets seen - use an Orthographic projection that ranges
    // from [0, windowWidth] in X and [0, windowHeight] in Y. (0,0) is the lower left.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, windowWidth, 0, windowHeight );
    
    // update the viewport as well - tell OpenGL we want to render the whole window
    glViewport( 0, 0, windowWidth, windowHeight );

    // just as good practice, switch back to GL_MODELVIEW mode; that's the matrix
    // that we want to be changing with glTranslatef(), glScalef(), etc., so
    // just in case we forget to explicitly set it later...
    glMatrixMode( GL_MODELVIEW );
}
//void drawMyTriangle()
//This will draw our triangles so we can manipulate them
void drawMyTriangle() {
	
	glPushMatrix();
	glColor3f(.9, .8, .1);
	glBegin(GL_TRIANGLES);
	glVertex2f(-50, -50);
	glVertex2f(50, -50);
	glVertex2f(0, 30);
	glEnd();
	glPopMatrix();

}
//void translateTriangle
void drawMyTriforce() {
	glTranslatef(300, 300, 0);
	glPushMatrix();
	glTranslatef(-25, -25, 0);
	drawMyTriangle();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(75, -25, 0);
	drawMyTriangle();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(25, 55, 0);
	drawMyTriangle();
	glPopMatrix();
}

//  void renderScene()
//
//      We will register this function as GLUT's display callback.
//  This is where the magic happens - all rendering is done here.
//
void renderScene() {
    // clear whatever was drawn to the screen last time - 
    // set the clear color to black and then signal to clear the RGB buffer.
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // switch to GL_MODELVIEW, for when we start using glTranslatef(), glScalef(), etc..
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	//*******************************************//
	//***                                     ***//
	//***   YOUR CODE WILL GO HERE            ***//
	glColor3f(.9, .8, .1);
	glBegin(GL_TRIANGLES);
	//First
	glVertex2f(100, 100);
	glVertex2f(200, 100);
	glVertex2f(150, 180);
	//Second
	glVertex2f(200, 100);
	glVertex2f(300, 100);
	glVertex2f(250, 180);
	//Third
	glVertex2f(150, 180);
	glVertex2f(250, 180);
	glVertex2f(200, 260);

	
	glEnd();
	drawMyTriforce();

	//***                                     ***//
	//*******************************************//

    // flush the OpenGL commands and make sure they get rendered!
    glFlush();
}

// you should know what main does...
int main( int argc, char *argv[] ) {
	
	// initialize GLUT... always need this call.
	glutInit( &argc, argv );
	
	// request a window with just an RGB frame buffer, nothing fancy...
	// place it at (50,50) on the screen and set its size to our constants from earlier
	glutInitDisplayMode( GLUT_RGB );
	glutInitWindowPosition( 50, 50 );
	glutInitWindowSize( windowWidth, windowHeight );
	
	// this actually creates our window giving it title "Lab 00A"
	windowId = glutCreateWindow( "Lab 00A" );
	
	// register our reshape and display callbacks with GLUT - these functions are above
	glutDisplayFunc( renderScene );
	glutReshapeFunc( changeSize );
	
	// begin GLUT loop
	glutMainLoop();
	
	return 0;
}
