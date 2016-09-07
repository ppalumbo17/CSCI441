/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab01
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2016
 *
 *  Description:
 *      Contains the code for a simple 2D OpenGL / GLUT example.  
 *  Animation, keyboard, and mouse interaction will be added to 
 *  this very basic example.
 *
 */

#ifdef __APPLE__			// if compiling on Mac OS
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else					// else compiling on Linux OS
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//global variables
int triforceAngle = 20;
bool EVIL_TRIFORCE = false;
int myMouseX = 100;
int myMouseY = 100;
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

//
//  void drawTriangle()
//
//		Issues a series of OpenGL commands to draw a triangle
//
void drawTriangle() {
	// push the current matrix before doing anything;
	// just a good, safe practice
	glPushMatrix(); {
		// tell OpenGL we want to draw triangles
		glBegin( GL_TRIANGLES ); {
			glVertex2f( -2.5, -2 );		// lower left corner
			glVertex2f(  2.5, -2 );		// lower right corner
			glVertex2f(  0.0f,  2.0f );	// top corner
		}; glEnd();	// tell OpenGL we are done drawing triangles
	}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
}

//
//  void drawTriforce()
//
//      Issues a series of OpenGL commands to draw three triangles in a
//  triangle shape that all fit inside a larger triangle.
//
void drawTriforce() {
	// push the current matrix before doing anything;
	// just a good, safe practice
	glPushMatrix(); {
	
		// make all the vertices be a nice, golden color.
		glColor3f( 0.9, 0.8, 0.1 );

		if (EVIL_TRIFORCE) {
			glColor3f(0.9, 0.1, 0.1);
		}
		// push the current matrix before doing anything;
		// just a good, safe practice		
		glPushMatrix(); {
			
			// the triangle will get rendered around the origin,
			// so move the origin to (-25, -20)
			glTranslatef( -2.50f, -2.00f, 0.0f );
			
			// scale the triangle by 10 in x and 10 in y
			//glScalef( 10.0f, 10.0f, 1.0f );
			
			// draw the triangle
			drawTriangle();
		}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
		
		// push the current matrix before doing anything;
		// just a good, safe practice		
		glPushMatrix(); {
			// the triangle will get rendered around the origin,
			// so move the origin to (25, -20)
			glTranslatef( 2.50f, -2.00f, 0.0f );
			
			// scale the triangle by 10 in x and 10 in y
			//glScalef( 10.0f, 10.0f, 1.0f );
			
			// draw the triangle
			drawTriangle();
		}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
		
		// push the current matrix before doing anything;
		// just a good, safe practice
		glPushMatrix(); {
			// the triangle will get rendered around the origin,
			// so move the origin to (0, 20)
			glTranslatef( 0.0f, 2.00f, 0.0f );
			
			// scale the triangle by 10 in x and 10 in y
			//glScalef( 10.0f, 10.0f, 1.0f );
			
			// draw the triangle
			drawTriangle();
		}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
		
	}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
}

//
//  void renderScene()
//
//      We will register this function as GLUT's display callback.
//  This is where the magic happens - all rendering is done here.
//
void renderScene() {
	//add to back buffer
	glDrawBuffer(GL_BACK);

    // clear whatever was drawn to the screen last time - 
    // set the clear color to black and then signal to clear the RGB buffer.
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // switch to GL_MODELVIEW, for when we start using glTranslatef(), glScalef(), etc..
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // push the current matrix before doing anything;
    // just a good, safe practice
    glPushMatrix(); {

		// the triforce will get rendered around the origin,
		// so move the origin to (200,300)...
		//glTranslatef( 200, 300, 0 );
		glTranslatef(myMouseX, myMouseY, 0.0f);

		// and then rotate it 45 degrees about its new origin
		glRotatef( triforceAngle, 0, 0, 1 );

		// and then scale it 2X in x and 2X in y
		glScalef( 10, 10, 1 );
		
		// send OpenGL a series of instructions that render our triforce
		drawTriforce();
	}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
	
    // flush the OpenGL commands and make sure they get rendered!
    //glFlush();
	glutSwapBuffers();
}

//LAB 01 IMPLEMENTATIONS BELOW THIS LINE

//void myKeyboard()
//this will control our keyboard calls
void myKeyboard(unsigned char key, int mouseX, int mouseY) {
	if (key == 27) {
		exit(0);
	}
	if (key == 99) {
		EVIL_TRIFORCE = !EVIL_TRIFORCE;
	}
}

//void myTimer(int)
//timer function to enable animation of our triangles
void myTimer(int value) {
	triforceAngle += 2;
	glutPostRedisplay();
	glutTimerFunc(1000/60, myTimer, 0);
}

//void myMouse(int button, int state, int mouseX, int mouseY)
//all mouse movement functions
void myMouse(int button, int state, int mouseX, int mouseY) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			EVIL_TRIFORCE = true;
		}
		if (state == GLUT_UP) {
			EVIL_TRIFORCE = false;
		}
	}
}

//void myMotion(int mouseX, int mouseY)
//control passive mouse motions
void myMotion(int mouseX, int mouseY) {
	myMouseX = mouseX;
	myMouseY = mouseY;
}

//
//  int main( int argc, char* argv[] )
//
int main( int argc, char* argv[] ) {
    // initialize GLUT... always need this call.
    glutInit( &argc, argv );

    // request a window with just an RGB frame buffer, nothing fancy...
    // place it at (50,50) on the screen and set its size to our constants from earlier
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowPosition( 50, 50 );
    glutInitWindowSize( windowWidth, windowHeight );
    
    // this actually creates our window. 
    windowId = glutCreateWindow( "Triforce" );

    // register our reshape and display callbacks with GLUT - these functions are above
    glutDisplayFunc( renderScene );
    glutReshapeFunc( changeSize );

	/********** ADD CALLBACKS ***************/
	glutKeyboardFunc(myKeyboard);
	glutTimerFunc(1000/60, myTimer, 0);
	glutMouseFunc(myMouse);
	glutPassiveMotionFunc(myMotion);
    // begin GLUT loop
    glutMainLoop();

    return 0;
}
