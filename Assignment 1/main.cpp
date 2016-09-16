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
	#include <math.h>
#endif

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 700, windowHeight = 150;

// global variable to keep track of the window id
int windowId;

const float DEG2RAD = 3.14159 / 180;

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


//void drawCircle
//draw a perfect circle, i hope
void drawCircle(float radius)
{
	glBegin(GL_LINE_LOOP);

	for(int i=0; i<360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex2f(cos(degInRad)*radius, sin(degInRad)*radius);
	}

	glEnd();
}

//void drawSmallTriangle()
//draw a triangle that can be rotated
void drawSmallTriangle() {
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(0, 15);
	glVertex2f(20, 0);
	glEnd();
	glPopMatrix();
}

//void drawSmallRect()
//draw a connecting rectangle mainly for horizontal use
void drawSmallRect() {
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, 15);
	glVertex2f(20, 15);
	glVertex2f(20, 0);
	glEnd();
	glPopMatrix();
}

//void drawLargeRect()
//draw a large connecting rectangle mainly for vertical use
void drawLargeRect() {
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, 70);
	glVertex2f(20, 70);
	glVertex2f(20, 0);
	glEnd();
	glPopMatrix();
}
//void drawLetterC()
//draw the letter c
void drawLetterC() {
	//glColor3f(0, 0, 0);
	glScalef(.9, .9, 0);
	//upper right triangle
	glPushMatrix();
	//glTranslatef(50, 110, 0);
	glTranslatef(40, 85, 0);
	drawSmallTriangle();
	glPopMatrix();
	//upper left triangle
	glPushMatrix();
	//glTranslatef(30, 110, 0);
	glTranslatef(20, 85, 0);
	glScalef(-1, 1, 1);
	drawSmallTriangle();
	glPopMatrix();
	//bottom right triangle
	glPushMatrix();
	//glTranslatef(50, 40, 0);
	glTranslatef(40, 15, 0);
	glScalef(1, -1, 1);
	drawSmallTriangle();
	glPopMatrix();
	//bottom left triangle
	glPushMatrix();
	//glTranslatef(30, 40, 0);
	glTranslatef(20, 15, 0);
	glRotatef(180, 0, 0, 1);
	drawSmallTriangle();
	glPopMatrix();

	//uper right rect
	glPushMatrix();
	//glTranslatef(50, 95, 0);
	glTranslatef(40, 70, 0);
	drawSmallRect();
	glPopMatrix();
	//upper middle rect
	glPushMatrix();
	//glTranslatef(30, 110, 0);
	glTranslatef(20, 85, 0);
	drawSmallRect();
	glPopMatrix();
	//lower right rect
	glPushMatrix();
	//glTranslatef(50, 40, 0);
	glTranslatef(40, 15, 0);
	drawSmallRect();
	glPopMatrix();
	//lower middle rect
	glPushMatrix();
	//glTranslatef(30, 25, 0);
	glTranslatef(20, 0, 0);
	drawSmallRect();
	glPopMatrix();
	//large rect
	glPushMatrix();
	//glTranslatef(10, 40, 0);
	glTranslatef(0, 15, 0);
	drawLargeRect();
	glPopMatrix();
}

//void drawLetterY()
//draw the letter Y
void drawLetterY() {
	glTranslatef(20, 0,0);
	//drawbase
	glPushMatrix();
	drawLargeRect();
	glPopMatrix();
	//draw upperleft triangle
	glPushMatrix();
	glTranslatef(0, 85, 0);
	glRotatef(90, 0, 0, 1);
	glScalef(-1, 1, 1);
	drawSmallTriangle();
	glPopMatrix();
	//draw centerleft triangle
	glPushMatrix();
	glTranslatef(0, 65, 0);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	glScalef(1, -1, 1);
	drawSmallTriangle();
	glPopMatrix();
	glPopMatrix();
	//draw upperright triangle
	glPushMatrix();
	glTranslatef(20, 85, 0);
	glRotatef(-90, 0, 0, 1);
	drawSmallTriangle();
	glPopMatrix();
	//draw centerright triangle
	glPushMatrix();
	glTranslatef(20, 65, 0);
	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	drawSmallTriangle();
	glPopMatrix();
	glPopMatrix();
	
	

}

//void drawLetterL()
//draw the letter L
void drawLetterL() {
	//draw left tall
	glPushMatrix();
	drawLargeRect();
	glPopMatrix();
	//draw top
	glPushMatrix();
	glTranslatef(0, 70, 0);
	drawSmallRect();
	glPopMatrix();
	//draw bottom
	glPushMatrix();
	glTranslatef(20, 0, 0);
	drawSmallRect();
	glPopMatrix();
}

//void drawLetterO(){
//draw the letter O
void drawLetterO(){
	glScalef(.9, .9, 0);
	//draw left side
	glPushMatrix();
	drawLargeRect();
	glPopMatrix();
	//draw top 
	glPushMatrix();
	glTranslatef(20, 55, 0);
	drawSmallRect();
	glPopMatrix();
	//draw bottom
	glPushMatrix();
	glTranslatef(20, 0, 0);
	drawSmallRect();
	glPopMatrix();
	//draw left side
	glPushMatrix();
	glTranslatef(40, 0, 0);
	drawLargeRect();
	glPopMatrix();

}

//draw letter O using C
void drawLetterOUsingC() {
	drawLetterC();
	//lower right rect
	glPushMatrix();
	//glTranslatef(50, 40, 0);
	glTranslatef(40, 15, 0);
	drawLargeRect();
	glPopMatrix();
}

//void drawLetterP()
//draw the base for a letter P
void drawLetterP() {
	//draw left base
	glPushMatrix();
	drawLargeRect();
	glPopMatrix();
	//draw top left
	glPushMatrix();
	glTranslatef(0, 70, 0);
	drawSmallRect();
	glPopMatrix();
	//draw top
	glPushMatrix();
	glTranslatef(20, 70, 0);
	drawSmallRect();
	glPopMatrix();
	//draw rightside top
	glPushMatrix();
	glTranslatef(40, 70, 0);
	//glRotatef(-90, 0, 0, 1);
	drawSmallRect();
	glPopMatrix();
	//draw rightside bottom
	glPushMatrix();
	glTranslatef(40, 55, 0);
	//glRotatef(-90, 0, 0, 1);
	drawSmallRect();
	glPopMatrix();
	//draw bttom right
	glPushMatrix();
	glTranslatef(40, 40, 0);
	drawSmallRect();
	glPopMatrix();
	//draw bottom
	glPushMatrix();
	glTranslatef(20, 40, 0);
	drawSmallRect();
	glPopMatrix();
}

//void drawLetterA()
//draw the base for an A letter
void drawLetterA() {
	//draw left side
	glPushMatrix();
	drawLargeRect();
	glPopMatrix();
	//draw top left triangle
	glPushMatrix();
	glTranslatef(20, 70, 0);
	glScalef(-1, 1, 1);
	drawSmallTriangle();
	glPopMatrix();
	//draw top 
	glPushMatrix();
	glTranslatef(20, 70, 0);
	drawSmallRect();
	glPopMatrix();
	//draw bottom
	glPushMatrix();
	glTranslatef(20, 40, 0);
	drawSmallRect();
	glPopMatrix();
	//draw left side
	glPushMatrix();
	glTranslatef(40, 0, 0);
	drawLargeRect();
	glPopMatrix();
	//draw top right triangle
	glPushMatrix();
	glTranslatef(40, 70, 0);
	drawSmallTriangle();
	glPopMatrix();
}

//void drawLeterS()
//draw the letter S
void drawLetterS() {
	glScalef(.87, .87, 0);
	//upper right triangle
	glPushMatrix();
	glTranslatef(40, 90, 0);
	drawSmallTriangle();
	glPopMatrix();
	//upper left triangle
	glPushMatrix();
	glTranslatef(20, 90, 0);
	glScalef(-1, 1, 1);
	drawSmallTriangle();
	glPopMatrix();
	//bottom right triangle
	glPushMatrix();
	glTranslatef(40, 15, 0);
	glScalef(1, -1, 1);
	drawSmallTriangle();
	glPopMatrix();
	//bottom left triangle
	glPushMatrix();
	glTranslatef(20, 15, 0);
	glRotatef(180, 0, 0, 1);
	drawSmallTriangle();
	glPopMatrix();
	//middle left triangle
	glPushMatrix();
	glTranslatef(20, 60, 0);
	glRotatef(180, 0, 0, 1);
	drawSmallTriangle();
	glPopMatrix();
	//middle right triangle
	glPushMatrix();
	glTranslatef(40, 45, 0);
	drawSmallTriangle();
	glPopMatrix();

	//uper right rect
	glPushMatrix();
	glTranslatef(40, 75, 0);
	drawSmallRect();
	glPopMatrix();
	//upper middle rect
	glPushMatrix();
	glTranslatef(20, 90, 0);
	drawSmallRect();
	glPopMatrix();
	//lower right rect
	glPushMatrix();
	glTranslatef(40, 15, 0);
	drawSmallRect();
	glPopMatrix();
	//center right rect
	glPushMatrix();
	glTranslatef(40, 30, 0);
	drawSmallRect();
	glPopMatrix();
	//lower middle rect
	glPushMatrix();
	glTranslatef(20, 0, 0);
	drawSmallRect();
	glPopMatrix();
	//center middle rect
	glPushMatrix();
	glTranslatef(20, 45, 0);
	drawSmallRect();
	glPopMatrix();
	//lower middle left rect
	glPushMatrix();
	glTranslatef(0, 60, 0);
	drawSmallRect();
	glPopMatrix();
	//upper middle left rect
	glPushMatrix();
	glTranslatef(0, 75, 0);
	drawSmallRect();
	glPopMatrix();
	//lower left rect
	glPushMatrix();
	glTranslatef(0, 15, 0);
	drawSmallRect();
	glPopMatrix();
	
}
//void drawLetterE()
//use letter L to draw E
void drawLetterE() {
	drawLetterL();

	//draw top right
	glPushMatrix();
	glTranslatef(20, 70, 0);
	drawSmallRect();
	glPopMatrix();
	//draw center
	glPushMatrix();
	glTranslatef(20, 35, 0);
	drawSmallRect();
	glPopMatrix();

}
//void drawName()
//Put it all together
void drawName() {
	glTranslatef(5, 25, 0);
	glScalef(.8, .8, 0);
	//CYCLOPALYPSE

	//C
	glPushMatrix();
	glColor3f(1, 0.6, 0.8);
	glTranslatef(0, 0, 0);
	drawLetterC();
	glPopMatrix();

	//Y
	glPushMatrix();
	glColor3f(1, .6, 0.6);
	glTranslatef(65, 0, 0);
	drawLetterY();
	glPopMatrix();

	//C
	glPushMatrix();
	glColor3f(1, 1, 0.6);
	glTranslatef(130, 0, 0);
	drawLetterC();
	glPopMatrix();

	//L
	glPushMatrix();
	glColor3f(0.36,1, 0.68);
	glTranslatef(200, 0, 0);
	drawLetterL();
	glPopMatrix();

	//O
	glPushMatrix();
	glColor3f(0.6, 1, 0.8);
	glTranslatef(250, 0, 0);
	drawLetterOUsingC();
	glPopMatrix();

	//P
	glPushMatrix();
	glColor3f(0.6, 1, 1);
	glTranslatef(320, 0, 0);
	drawLetterP();
	glPopMatrix();

	//A
	glPushMatrix();
	glColor3f(1, 0.6, 0.8);
	glTranslatef(390, 0, 0);
	drawLetterA();
	glPopMatrix();

	//L
	glPushMatrix();
	glColor3f(1, .6, 0.6);
	glTranslatef(460, 0, 0);
	drawLetterL();
	glPopMatrix();

	//Y
	glPushMatrix();
	glColor3f(1, 1, 0.6);
	glTranslatef(495, 0, 0);
	drawLetterY();
	glPopMatrix();

	//P
	glPushMatrix();
	glColor3f(0.36, 1, 0.68);
	glTranslatef(555, 0, 0);
	drawLetterP();
	glPopMatrix();

	//S
	glPushMatrix();
	glColor3f(0.6, 1, 0.8);
	glTranslatef(620, 0, 0);
	drawLetterS();
	glPopMatrix();

	//E
	glPushMatrix();
	glColor3f(0.6, 1, 1);
	glTranslatef(685, 0, 0);
	drawLetterE();
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();

	
}
//void drawCrest()
//draw a crest of triangle and circle
void drawCrest() {
	glPushMatrix();
	glColor3f(1, .6, .6);
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(100, 0);
	glVertex2f(50, 80);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(50, 26.67, 0);
	drawCircle(25);
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
    glClearColor( 0.4, 0.4, 0.4, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // switch to GL_MODELVIEW, for when we start using glTranslatef(), glScalef(), etc..
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	//*******************************************//
	//***                                     ***//
	//CYCLOPALYPSE
	drawName();
	glPushMatrix();
	glTranslatef(750, 10, 0);
	drawCrest();
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
