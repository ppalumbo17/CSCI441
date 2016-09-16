/*
 *  CSCI 441, Computer Graphics, Fall 2015
 *
 *  Project: lab02
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2015
 *
 *  Description:
 *      Contains the base code for a basic flight simulator.
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
#include <time.h>
#include <math.h>

// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

static size_t windowWidth  = 640;
static size_t windowHeight = 480;
static float aspectRatio;
static float cameraStepValue = 2.5;
GLint leftMouseButton; 		   	    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

float cameraX, cameraY, cameraZ;            // camera position in cartesian coordinates
float cameraTheta, cameraPhi;               // camera DIRECTION in spherical coordinates
float dirX, dirY, dirZ;                     // camera DIRECTION in cartesian coordinates

GLuint environmentDL;                       // display list for the 'city'


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

    /** TODO #3: DRAW A GRID IN THE XZ-PLANE USING GL_LINES **/
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	for (int i = -50; i < 50; i++) {
		glVertex3f(-50, 0, i);
		glVertex3f(50, 0, i);
		glVertex3f(i, 0, -50);
		glVertex3f(i, 0, 50);
	}
	glEnd();
    /*
     *	As noted above, we are done drawing with OpenGL Primitives, so we
     *	must turn lighting back on.
     */
    glEnable( GL_LIGHTING );
}

// drawCity() //////////////////////////////////////////////////////////////////
//
//  Function to draw a random city using GLUT 3D Primitives
//
////////////////////////////////////////////////////////////////////////////////
void drawCity() {
    // TODO #4: Randomly place buildings of varying heights with random colors
	int height = 1;
	for (int x = -50; x < 50; x++) {
		for (int z = -50; z < 50; z++) {
			if (x % 2 == 0 && z % 2 == 0 && getRand() < .4) {
				glBegin(GL_QUADS);
				glColor3f(getRand(), getRand(), getRand());
				GLfloat height = getRand() * 10 + 1;
				//bottom
				glVertex3f(x, 0, z);
				glVertex3f(x, 0, z + 1);
				glVertex3f(x + 1, 0, z + 1);
				glVertex3f(x + 1, 0, z);

				//top
				glVertex3f(x, height, z);
				glVertex3f(x, height, z + 1);
				glVertex3f(x + 1, height, z + 1);
				glVertex3f(x + 1, height, z);

				//side 1
				glVertex3f(x, 0, z);
				glVertex3f(x, 0, z + 1);
				glVertex3f(x, height, z + 1);
				glVertex3f(x, height, z);
				
				//side 2
				glVertex3f(x, 0, z + 1);
				glVertex3f(x + 1, 0, z + 1);
				glVertex3f(x, height, z + 1);
				glVertex3f(x + 1, height, z + 1);

				//side 3
				glVertex3f(x + 1, 0, z + 1);
				glVertex3f(x + 1, 0, z);
				glVertex3f(x + 1, height, z + 1);
				glVertex3f(x + 1, height, z);

				//side 4
				glVertex3f(x + 1, 0, z);
				glVertex3f(x, 0, z);
				glVertex3f(x, height, z);
				glVertex3f(x + 1, height, z);


				glEnd();

			}
		}

	}
}

// generateEnvironmentDL() /////////////////////////////////////////////////////
//
//  This function creates a display list with the code to draw a simple 
//      environment for the user to navigate through.
//
//  And yes, it uses a global variable for the display list.
//  I know, I know! Kids: don't try this at home. There's something to be said
//      for object-oriented programming after all.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironmentDL() {
    // TODO #1 Create a Display List & Call our Drawing Functions
	GLsizei n = 1;
	environmentDL = glGenLists(n);
	//start our display lists
	glNewList(environmentDL, GL_COMPILE);

	glPushMatrix(); {
		//glutSolidTeapot(2);
		drawCity();
		drawGrid();
	}; glPopMatrix();

	glEndList();
    
}


// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based 
//  on its position in spherical coordinates. Should be called every time 
//  cameraTheta, cameraPhi, or cameraRadius is updated. 
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
    // TODO #5: Convert spherical coordinates into a cartesian vector
    // see Wednesday's slides for equations.  Extra Hint: Slide #34
	
	dirX = sin(cameraTheta)*cos(cameraPhi);// +cameraX;
	dirY = -cos(cameraPhi);// +cameraY;
	dirZ = -cos(cameraTheta)*sin(cameraPhi);// +cameraZ;
	
	float length = sqrt(pow(dirX, 2) + pow(dirY, 2) + pow(dirZ, 2));
	
	dirX /= length;
	dirY /= length;
	dirZ /= length;

    // and NORMALIZE this directional vector!!!
    
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
    // update the left mouse button states, if applicable
    if(button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;    
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
	mouseX = x;
	mouseY = y;
    if(leftMouseButton == GLUT_DOWN) {
		cameraTheta += (mouseX - x)*.005;
		if (cameraPhi > 0 && cameraPhi < 3.1416) {
			cameraPhi += (mouseY - y)*.005;
		}
        recomputeOrientation();     // update camera (x,y,z) based on (radius,theta,phi)
		

	glutPostRedisplay();	    // redraw our scene from our new camera POV
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
    generateEnvironmentDL();
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a scene to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(void)  {
    //clear the render buffer
    glDrawBuffer( GL_BACK );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //update the modelview matrix based on the camera's position
    glMatrixMode(GL_MODELVIEW);              //make sure we aren't changing the projection matrix!
    glLoadIdentity();
    // TODO #6: Change how our lookAt matrix gets constructed
    gluLookAt( cameraX, cameraY, cameraZ,      // camera is located at (10,10,10)
                dirX+cameraX,  dirY+cameraY,  dirZ+cameraZ,   	// camera is looking at (0,0,0)
                0,  1,  0);     // up vector is (0,1,0) (positive Y)

    /** TODO #2: REMOVE TEAPOT & CREATE A CITY SCENE ON A GRID...but call it's display list! **/
	glCallList(environmentDL);

    //push the back buffer to the screen
    glutSwapBuffers();
}


// normalKeysDown() ////////////////////////////////////////////////////////////
//
//  GLUT keyboard callback; gets called when the user presses a key.
//
////////////////////////////////////////////////////////////////////////////////
void normalKeysDown(unsigned char key, int x, int y) {
	float length = sqrt(pow(cameraX, 2) + pow(cameraY, 2) + pow(cameraZ, 2));
    if(key == 'q' || key == 'Q' || key == 27)
        exit(0);

	if (key == 'w' || key == 'W') {
		cameraX -= dirX + cameraStepValue;
		cameraY -= dirY + cameraStepValue;
		cameraZ -= dirZ + cameraStepValue;
	}
	if (key == 's' || key == 'S') {
		cameraX += dirX + cameraStepValue;
		cameraY += dirY + cameraStepValue;
		cameraZ += dirZ + cameraStepValue;
	}
    glutPostRedisplay();		// redraw our scene from our new camera POV
}




// main() //////////////////////////////////////////////////////////////////////
//
//  Program entry point. Does not process command line arguments.
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    // create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("flight simulator 0.31");

    // give the camera a scenic starting point.
    cameraX = 60;
    cameraY = 40;
    cameraZ = 30;
    cameraTheta = -M_PI / 3.0f;
    cameraPhi = M_PI / 2.8f;
    recomputeOrientation();

    // register callback functions...
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
    glutKeyboardFunc(normalKeysDown);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);

    // do some basic OpenGL setup
    initScene();

    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
