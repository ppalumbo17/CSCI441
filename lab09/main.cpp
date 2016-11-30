/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab09
 *  File: main.cpp
 *
 *  Description:
 *      This lab will demonstrate how to use shaders by
 *  modifying a sphere.
 *
 *  Author:
 *      Jeffrey Paone, Colorado School of Mines
 *  
 *  Notes:
 *
 *
 */

// ALLLLLLLL of the includes that we use
// LOOKHERE #2
#include <GL/glew.h>

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/freeglut.h>
#endif

#include <SOIL/SOIL.h>

#include "include/Shader_Utils.h"
#include "include/Point.h"
#include "include/Vector.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

int windowWidth = 512, windowHeight = 512;  // window dimensions

GLint leftMouseButton, rightMouseButton;    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

Point cameraTPR;                            // spherical coordinates of camera mapped XYZ - TPR (theta phi radius)
Point cameraLookAt;                         // cpoint camera is looking at

GLuint shaderProgramHandle = 0;             // handle to our shader program
GLuint uniformTimeLoc = 0;					// location of our time uniform variable

// END GLOBAL VARIABLES ////////////////////////////////////////////////////////

//
// void computeArcballPosition(Point tpr, Point &xyz);
//
// This function updates the camera's position in cartesian coordinates based 
//  on its position in spherical coordinates. The user passes in the current phi,
//  theta, and radius, as a single point (X value of point = phi, etc.)  as well 
//  as a Point variable to hold the resulting X, Y, and Z values.
//
//  That Point gets filled with the camera's position in R3.
//
void computeArcballPosition( Point tpr, Point &xyz ) {
    xyz.setX( tpr.getZ() *  sinf( tpr.getX() )*sinf( tpr.getY() ) );
    xyz.setY( tpr.getZ() * -cosf( tpr.getY() ) );
    xyz.setZ( tpr.getZ() * -cosf( tpr.getX() )*sinf( tpr.getY() ) );
}

//
//  void resizeWindow(int w, int h)
//
//      We will register this function as GLUT's reshape callback.
//   When the window is resized, the OS will tell GLUT and GLUT will tell
//   us by calling this function - GLUT will give us the new window width
//   and height as 'w' and 'h,' respectively - so save those to our global vars.
//
void resizeWindow(int w, int h) {
    float aspectRatio = w / (float)h;

    windowWidth = w;
    windowHeight = h;

    //update the viewport to fill the window
    glViewport(0, 0, w, h);

    //update the projection matrix with the new window properties
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,aspectRatio,0.1,100000);
}

//
//  void mouseCallback(int button, int state, int thisX, int thisY)
//
//  GLUT callback for mouse clicks. We save the state of the mouse button
//      when this is called so that we can check the status of the mouse
//      buttons inside the motion callback (whether they are up or down).
//
void mouseCallback(int button, int state, int thisX, int thisY) {
    //update the left and right mouse button states, if applicable
    if(button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;
    else if(button == GLUT_RIGHT_BUTTON)
        rightMouseButton = state;
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}

//
//  void mouseMotion(int x, int y)
//
//  GLUT callback for mouse movement. We update the model's rotational angles
//      if the left mouse button is held down.
//
void mouseMotion(int x, int y) {
    //if the user is holding down the left mouse button, update the angles.
    if(leftMouseButton == GLUT_DOWN) {
        cameraTPR.setX( cameraTPR.getX() + (x - mouseX)*0.005 );
        cameraTPR.setY( cameraTPR.getY() + (y - mouseY)*0.005 );

        // make sure that phi stays within the range (0, M_PI)
        if(cameraTPR.getY() <= 0)
            cameraTPR.setY( 0+0.001 );
        if(cameraTPR.getY() >= M_PI)
            cameraTPR.setY( M_PI-0.001 );
        
        //cameraDirX/Y/Z will just get updated at the start of every
        //render frame, so don't worry about updating them here. 
    } else if(rightMouseButton == GLUT_DOWN) {
        //for the right mouse button, just determine how much the mouse has moved total.
        //not the best "zoom" behavior -- if X change is positive and Y change is negative,
        //(along the X = Y line), nothing will happen!! but it's ok if you zoom by
        //moving left<-->right or up<-->down, which works for most people i think.
        double totalChangeSq = (x - mouseX) + (y - mouseY);
        cameraTPR.setZ( cameraTPR.getZ() + totalChangeSq*0.01 );
        
        //limit the camera radius to some reasonable values so the user can't get lost
        if(cameraTPR.getZ() < 1.0)
            cameraTPR.setZ( 1.0 );
        if(cameraTPR.getZ() > 50.0)
            cameraTPR.setZ( 50.0 );
    }

    mouseX = x;
    mouseY = y;
}

//
//  void initScene()
//
//  A basic scene initialization function; should be called once after the
//      OpenGL context has been created. Doesn't need to be called further.
//      Just sets up a few function calls so that our main() is cleaner.
//
void initScene()  {
    glEnable(GL_DEPTH_TEST);

    //tell OpenGL not to use the material system; just use whatever we pass with glColor()
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    float lightCol[4] = { 1, 1, 1, 1};
    float ambientCol[4] = {0.1, 0.1, 0.1, 1.0};
    glLightfv( GL_LIGHT0,GL_DIFFUSE,lightCol );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    //tells OpenGL to blend colors across triangles. Once lighting is
    //enabled, this means that objects will appear smoother - if your object
    //is rounded or has a smooth surface, this is probably a good idea;
    //if your object has a blocky surface, you probably want to disable this.
    glShadeModel(GL_SMOOTH);
    
    // give our camera a pretty starting point
    cameraTPR = Point(2.8, 2.0, 25.0);
    cameraLookAt = Point(0,0,0);
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for scene rendering.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(void)  {
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // compute where on the sphere our camera is located
    Point cameraPosition;
    computeArcballPosition(cameraTPR, cameraPosition);
    
    // now compute where our sphere is located relative to the look at point
    Point eyePosition = cameraPosition + (cameraLookAt - Point(0,0,0));
    
    //update the modelview matrix based on the camera's position
    glMatrixMode(GL_MODELVIEW);         //make sure we aren't changing the projection matrix!
    glLoadIdentity();
    gluLookAt( eyePosition, cameraLookAt, Vector(0,1,0) );
    
    // this is some code to enable a default light for the scene;
    // feel free to play around with this, but we won't talk about
    // lighting in OpenGL for another couple of weeks yet.
    float lPosition[4] = { 10, 10, 10, 1 };
    glLightfv( GL_LIGHT0,GL_POSITION,lPosition );
    
    // draw everything of interest

    glPushMatrix(); {
    	// TODO #11: use our shader for the first sphere, but not the second sphere

    	// TODO #16: get our elapsed time and pass it to the shader

		glutSolidSphere( 5.0f, 32.0f, 32.0f );

		glTranslatef( 0, 0, -10 );
    	glutSolidSphere( 5.0f, 32.0f, 32.0f );

    }; glPopMatrix();

    //push the back buffer to the screen
    glutSwapBuffers();
}

//
//  void normalKeys(unsigned char key, int x, int y)
//
//      GLUT keyboard callback for regular characters.
//
void normalKeys(unsigned char key, int x, int y)  {
    //kindly quit if the user requests!
    if(key == 'q' || key == 'Q')
        exit(0);
    
    // move camera forward - shrink the radius (remember Z = Radius for spherical point)
    else if(key == 'w' || key == 'W')
        cameraTPR.setZ( cameraTPR.getZ() - 0.1f );
    
    // move camera backward - increase the radius
    else if(key == 's' || key == 'S')
        cameraTPR.setZ( cameraTPR.getZ() + 0.1f );
}

//
// void myTimer(int value)
//
//  We have to take an integer as input per GLUT's timer function prototype;
//  but we don't use that value so just ignore it. We'll register this function
//  once at the start of the program, and then every time it gets called it
//  will perpetually re-register itself and tell GLUT that the screen needs
//  be redrawn. yes, I said "needs be."
//
void myTimer(int value) {
    glutPostRedisplay();

    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
}

// setupShaders() //////////////////////////////////////////////////////////////
//
//  A simple helper subrouting that performs the necessary steps to enable shaders
//      and bind them appropriately. Note because we're using global variables,
//      everything is relatively hard-coded, including filenames, and none of
//      the information is passed to the function as parameters.
//
////////////////////////////////////////////////////////////////////////////////
void setupShaders() {

	// TODO #7: compile shader program
    
	// TODO #15: get our uniform location
}

//
//  int main(int argc, char **argv)
//
//      No offense but you should probably know what the main function does.
//      argc is the number of arguments to the program (program name included),
//      and argv is an array of strings; argv[i] is the ith command line
//      argument, where 0 <= i <= argc
//
int main( int argc, char **argv ) {
    // create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
    glutInitWindowPosition( 50, 50 );
    glutInitWindowSize( windowWidth, windowHeight );
    glutCreateWindow( "Lab09: GLSL Shaders!" );

    /* initialize GLEW */
    // LOOKHERE #1
    GLenum glewResult = glewInit();
    /* check for an error */
    if( glewResult != GLEW_OK ) {
        printf( "[ERROR]: Error initalizing GLEW\n");
        return 0;
    }
    
    GLint maxVertexAttribs = 0;
    glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs );
    
    GLint maxLights = 0;
    glGetIntegerv( GL_MAX_LIGHTS, &maxLights );
    
    GLint maxTextures = 0;
    glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures );
    
    fprintf( stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf( stdout, "[INFO]: | OpenGL Information                                     |\n");
    fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf( stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION) );
    fprintf( stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER) );
    fprintf( stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR) );
    fprintf( stdout, "[INFO]: |   Shading Version: %35s |\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );
    fprintf( stdout, "[INFO]: >--------------------------------------------------------<\n");
    fprintf( stdout, "[INFO]: |   Max # Vertex Attributes:  %26d |\n", maxVertexAttribs );
    fprintf( stdout, "[INFO]: |   Max # Lights:    %35d |\n", maxLights );
    fprintf( stdout, "[INFO]: |   Max # Textures:  %35d |\n", maxTextures );
    fprintf( stdout, "[INFO]: \\--------------------------------------------------------/\n\n");
    
    /* make sure OpenGL 2.0 is supported */
    if( !glewIsSupported( "GL_VERSION_2_0" ) ) {
        printf( "[ERROR]: System does not support OpenGL 2.0 and GLSL\n" );
        return 0;
    }
    
    printf( "[INFO]: System supports OpenGL2.0 and GLSL!\n\n" );

    //do some basic OpenGL setup
    initScene();
    printf( "[INFO]: OpenGL Scene set up\n" );
    
    // set up our shaders (the files are hardcoded in)
    setupShaders();
    printf( "[INFO]: Shader compilation complete.\n" );

    // register callback functions...
    glutKeyboardFunc(normalKeys);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);

    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
