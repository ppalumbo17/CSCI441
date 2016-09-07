/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab00b
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

//void draw rectangle
// Draw a rectangular shape to act as a building
void drawRectangle() {

	glPushMatrix();
	glColor3f(0.6, 0.75, 0.78);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, 300);
	glVertex2f(80, 300);
	glVertex2f(80, 0);
	glEnd();
	glPopMatrix();

}

//void drawSquare()
//Draw a square that can be scaled for windows and buildings

void drawSquare() {
	glPushMatrix();
	//glColor3f(.9, .8, .1);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, 20);
	glVertex2f(20, 20);
	glVertex2f(20, 0);
	glEnd();
	glPopMatrix();
}


//void drawWindows()
//draw windows on buildings

void drawWindows() {
	glColor3f(.9, .8, .1);
	//top left window
	
	glPushMatrix();
	glTranslatef(15, 250, 0);
	drawSquare();
	glPopMatrix();
	//top right window
	
	glPushMatrix();
	glTranslatef(45, 250, 0);
	drawSquare();
	glPopMatrix();

	//middle top left window

	glPushMatrix();
	glTranslatef(15, 200, 0);
	drawSquare();
	glPopMatrix();
	//middle top right window

	glPushMatrix();
	glTranslatef(45, 200, 0);
	drawSquare();
	glPopMatrix();

	//middle bottom left window

	glPushMatrix();
	glTranslatef(15, 150, 0);
	drawSquare();
	glPopMatrix();
	//middle bottom right window

	glPushMatrix();
	glTranslatef(45, 150, 0);
	drawSquare();
	glPopMatrix();

	//bottom left window

	glPushMatrix();
	glTranslatef(15, 100, 0);
	drawSquare();
	glPopMatrix();
	//bottom right window

	glPushMatrix();
	glTranslatef(45, 100, 0);
	drawSquare();
	glPopMatrix();
}

//void drawDoor()
//draw a door on a building

void drawDoor() {
	glColor3f(0, 0, 0);
	glTranslatef(20, 0, 0);

	glPushMatrix();
	
	glScalef(2, 2, 0);
	
	drawSquare();
	glPopMatrix();
}

//void drawBuilding()
//put it all together into one call
void drawBuilding() {
	glPushMatrix();
	drawRectangle();
	drawWindows();
	drawDoor();
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
    glClearColor( 0, 144, 255, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // switch to GL_MODELVIEW, for when we start using glTranslatef(), glScalef(), etc..
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	//*******************************************//
	//***                                     ***//

	//draw multiple buildings
	glPushMatrix();
	glTranslatef(35, 0, 0);
	//first building
	drawBuilding();
	//second building
	glPushMatrix();
	glTranslatef(120, 0, 0);
	drawBuilding();
	glPopMatrix();
	//third building
	glPushMatrix();
	glTranslatef(240, 0, 0);
	drawBuilding();
	glPopMatrix();
	//fourth building
	glPushMatrix();
	glTranslatef(360, 0, 0);
	drawBuilding();
	glPopMatrix();
	glPopMatrix();

	//***   YOUR CODE WILL GO HERE            ***//
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
	
	// this actually creates our window giving it title "Lab 00B"
	windowId = glutCreateWindow( "Lab 00B" );
	
	// register our reshape and display callbacks with GLUT - these functions are above
	glutDisplayFunc( renderScene );
	glutReshapeFunc( changeSize );
	
	// begin GLUT loop
	glutMainLoop();
	
	return 0;
}
