/*
 *  CSCI 441, Computer Graphics, Fall 2015
 *
 *  Project: lab03
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2015
 *
 *  Description:
 *      Contains the base code for 3D Bezier Curve visualizer.
 *
 */

// HEADERS /////////////////////////////////////////////////////////////////////

// OpenGL / GLUT Libraries we need
#ifdef __APPLE__			// if compiling on Mac OS
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else					// else compiling on Linux OS
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

//#include <GL/glui.h>			// include our GLUI header


// C Libraries we need
#include <time.h>			// for time() to seed our RNG
#include <stdio.h>			// allow to print to terminal
#include <stdlib.h>			// access to rand() and exit()
#include <math.h>			// for cosf(), sinf(), etc.


// C++ Libraries we'll use
#include <fstream>			// we'll use ifstream	
#include <string>			// for, well strings!
#include <vector>			// and vectors (the storage container, not directional)
using namespace std;


// Headers We've Written
#include "Point.h"

// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

static size_t windowWidth = 640;
static size_t windowHeight = 480;
static float aspectRatio;

GLint leftMouseButton, rightMouseButton;    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

float cameraX, cameraY, cameraZ;            // camera position in cartesian coordinates
float cameraTheta, cameraPhi;               // camera DIRECTION in spherical coordinates
float dirX, dirY, dirZ;                     // camera DIRECTION in cartesian coordinates

GLint menuId;				    			// handle for our menu

vector<Point> controlPoints;
float trackPointVal = 0.0f;

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() {
    return rand() / (float)RAND_MAX;
}

// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawGrid() {
    /*
     *	We will get to why we need to do this when we talk about lighting,
     *	but for now whenever we want to draw something with an OpenGL
     *	Primitive - like a line, quad, point - we need to disable lighting
     *	and then reenable it for use with the GLUT 3D Primitives.
     */
    glDisable( GL_LIGHTING );

    // draw our grid....what? triple nested for loops!  crazy!  but it works :)
    glColor3f( 1, 1, 1 );
    glLineWidth(2.0f);	// make our grid lines a little thicker so they are easier to see
    for( int dir = 0; dir < 2; dir++ ) {
        for( int i = -5; i < 6; i++ ) {
            glBegin( GL_LINE_STRIP ); {
                for( int j = -5; j < 6; j++ )
                    glVertex3f( dir < 1 ? i : j, 
		    			      0, 
			        dir < 1 ? j : i );
            }; glEnd();
        }
    }
    glLineWidth(1.0f);  // but be sure to set our line width back to its original size so we don't mess up future drawing

    /*
     *	As noted above, we are done drawing with OpenGL Primitives, so we
     *	must turn lighting back on.
     */
    glEnable( GL_LIGHTING );
}


// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based 
//  on its position in spherical coordinates. Should be called every time 
//  cameraTheta, cameraPhi, or cameraRadius is updated. 
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
    dirX =  sinf(cameraTheta)*sinf(cameraPhi);
    dirZ = -cosf(cameraTheta)*sinf(cameraPhi);
    dirY = -cosf(cameraPhi);

    //and normalize this directional vector!
    float mag = sqrt( dirX*dirX + dirY*dirY + dirZ*dirZ );
    dirX /= mag;  dirY /= mag;  dirZ /= mag;

    glutPostRedisplay();
}

// evaluateBezierCurve() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier Curve. 
//
////////////////////////////////////////////////////////////////////////////////
Point evaluateBezierCurve( Point p0, Point p1, Point p2, Point p3, float t ) {
    // TODO #08: Compute a point along a Bezier curve
	
    return Point(0,0,0);
}

// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution. 
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve( Point p0, Point p1, Point p2, Point p3, int resolution ) {
    // TODO #07: Draw a Bezier curve
}

// resizeWindow() //////////////////////////////////////////////////////////////
//
//  GLUT callback for window resizing. Resets GL_PROJECTION matrix and viewport.
//
////////////////////////////////////////////////////////////////////////////////
void resizeWindow(int w, int h) {
    aspectRatio = w / (float)h;

    windowWidth = w;
    windowHeight = h;

    //update the viewport to fill the window
    glViewport(0, 0, w, h);

    //update the projection matrix with the new window properties
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,aspectRatio,0.1,100000);
}



// mouseCallback() /////////////////////////////////////////////////////////////
//
//  GLUT callback for mouse clicks. We save the state of the mouse button
//      when this is called so that we can check the status of the mouse
//      buttons inside the motion callback (whether they are up or down).
//
////////////////////////////////////////////////////////////////////////////////
void mouseCallback(int button, int state, int thisX, int thisY) {
    //update the left and right mouse button states, if applicable
    if(button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}

// mouseMotion() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for mouse movement. We update cameraPhi, cameraTheta, and/or
//      cameraRadius based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
////////////////////////////////////////////////////////////////////////////////
void mouseMotion(int x, int y) {
    if(leftMouseButton == GLUT_DOWN) {
        cameraTheta += (x - mouseX)*0.005;
        cameraPhi   += (mouseY - y)*0.005;

        // make sure that phi stays within the range (0, M_PI)
        if(cameraPhi <= 0)
            cameraPhi = 0+0.001;
        if(cameraPhi >= M_PI)
            cameraPhi = M_PI-0.001;
        
        recomputeOrientation();     //update camera (x,y,z) based on (radius,theta,phi)
    }

    mouseX = x;
    mouseY = y;
}



// initScene() /////////////////////////////////////////////////////////////////
//
//  A basic scene initialization function; should be called once after the
//      OpenGL context has been created. Doesn't need to be called further.
//
////////////////////////////////////////////////////////////////////////////////
void initScene()  {
    glEnable(GL_DEPTH_TEST);

    //******************************************************************
    // this is some code to enable a default light for the scene;
    // feel free to play around with this, but we won't talk about
    // lighting in OpenGL for another couple of weeks yet.
    float lightCol[4] = { 1, 1, 1, 1};
    float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
    float lPosition[4] = { 10, 10, 10, 1 };
    glLightfv( GL_LIGHT0, GL_POSITION,lPosition );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,lightCol );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
	
    // tell OpenGL not to use the material system; just use whatever we 
    // pass with glColor*()
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    //******************************************************************

    glShadeModel(GL_FLAT);

    srand( time(NULL) );	// seed our random number generator

    // give the camera a scenic starting point.
    cameraX = 6;
    cameraY = 4;
    cameraZ = 3;
    cameraTheta = -M_PI / 3.0f;
    cameraPhi = M_PI / 2.8f;
    recomputeOrientation();
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a scene to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(void)  {
    // clear the render buffer
    glDrawBuffer( GL_BACK );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // update the modelview matrix based on the camera's position
    glMatrixMode( GL_MODELVIEW );                           // make sure we aren't changing the projection matrix!
    glLoadIdentity();
    gluLookAt(  cameraX, cameraY, cameraZ,                  // camera is located at (x,y,z)
                cameraX+dirX, cameraY+dirY, cameraZ+dirZ,   // camera is looking at at (x,y,z) + (dx,dy,dz) -- straight ahead
                0.0f,1.0f,0.0f);                            // up vector is (0,1,0) (positive Y)

    // 
    glPushMatrix(); {
	drawGrid();
    }; glPopMatrix();

    // TODO #05: Draw our control points
    
    // TODO #06: Connect our control points
		
    // TODO #07: Draw the Bezier Curve!

    //push the back buffer to the screen
    glutSwapBuffers();
}


// normalKeysDown() ////////////////////////////////////////////////////////////
//
//  GLUT keyboard callback; gets called when the user presses a key.
//
////////////////////////////////////////////////////////////////////////////////
void normalKeysDown( unsigned char key, int x, int y ) {
    if( key == 'q' || key == 'Q' || key == 27 )
        exit(0);

    // because the direction vector is unit length, and we probably don't want
    // to move one full unit every time a button is pressed, just create a constant
    // to keep track of how far we want to move at each step. you could make
    // this change w.r.t. the amount of time the button's held down for
    // simple scale-sensitive movement!
    float movementConstant = 0.3f;

    // move forward!
    if( key == 'w' || key == 'W' ) {
        //that's as simple as just moving along the direction.
        cameraX += dirX*movementConstant;
        cameraY += dirY*movementConstant;
        cameraZ += dirZ*movementConstant;
    }

    // move backwards!
    else if( key == 's' || key == 'S' ) {
        //just move BACKWARDS along the direction.
        cameraX -= dirX*movementConstant;
        cameraY -= dirY*movementConstant;
        cameraZ -= dirZ*movementConstant;
    }
}

// myTimer() ////////////////////////////////////////////////////////////////////
//
//  GLUT timer callback; gets called when a timer expires
//
////////////////////////////////////////////////////////////////////////////////
void myTimer( int value ) {
    // redraw our display
    glutPostRedisplay();
    // register a new timer callback
    glutTimerFunc( 1000.0f / 60.0f, myTimer, 0 );
}

// myMenu() /////////////////////////////////////////////////////////////////////
//
//  Handles our Menu events
//
////////////////////////////////////////////////////////////////////////////////
void myMenu( int value ) {
	// TODO #02: handle our menu options
}

// createMenus() ///////////////////////////////////////////////////////////////
//
//  Handles creating a menu, adding menu entries, and attaching the menu to
//  a mouse button
//
////////////////////////////////////////////////////////////////////////////////
void createMenus() {
	// TODO #01: Create a Simple Menu
}

// registerCallbacks() /////////////////////////////////////////////////////////
//
//  Register all of our callbacks for GLUT.
//
////////////////////////////////////////////////////////////////////////////////
void registerCallbacks() {
    // keyboard callbacks
    glutSetKeyRepeat(   GLUT_KEY_REPEAT_ON );
    glutKeyboardFunc(   normalKeysDown     );

    // mouse callbacks
    glutMouseFunc(      mouseCallback      );
    glutMotionFunc(     mouseMotion        );

    // display callbacks
    glutDisplayFunc(    renderScene        );
    glutReshapeFunc(    resizeWindow       );

    // timer callback
    glutTimerFunc( 1000.0f / 60.0f, myTimer, 0 );
}

// loadControlPoints() /////////////////////////////////////////////////////////
//
//  Load our control points from file and store them in a global variable.
//
////////////////////////////////////////////////////////////////////////////////
bool loadControlPoints( char* filename ) {
	// TODO #04: read in control points from file.  Make sure the file can be
	// opened and handle it appropriately.

	return true;
}

// main() //////////////////////////////////////////////////////////////////////
//
//  Program entry point. Takes a single command line argument for our 
//    control points file.
//
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv ) {

    // TODO #03: make sure a control point CSV file was passed in.  Then read the points from file

    // create a double-buffered GLUT window at (50, 50) with predefined window size
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowPosition( 50, 50 );
    glutInitWindowSize( windowWidth, windowHeight );
    glutCreateWindow( "Lab03 - Bezier Curves" );

    fprintf(stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf(stdout, "[INFO]: | OpenGL Information                                     |\n");
    fprintf(stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf(stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION));
    fprintf(stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER));
    fprintf(stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR));
    //fprintf(stdout, "[INFO]: |   GLUI Version:    %35.2f |\n", GLUI_VERSION);   ***COMMENTED DUE TO NOT BEING ABLE TO MAKE***
    fprintf(stdout, "[INFO]: \\--------------------------------------------------------/\n");

    // do some basic OpenGL setup
    initScene();

    // create our menu options and attach to mouse button
    createMenus();

    // register callback functions...
    registerCallbacks();

    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
