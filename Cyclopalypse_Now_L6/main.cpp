/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab06
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2016
 *
 *  Description:
 *      Contains the base code for a basic flight simulator v 0.33...with lighting! 
 *  (but no sound...maybe in verison v0.34)
 *
 */

#ifdef __APPLE__			// if compiling on Mac OS
    #include <GLUT/glut.h>  // OpenGL Headers
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else                       // else compiling on Linux OS
    #include <GL/glut.h>    // OpenGL Headers
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

// C headers
#include <time.h>           // needed for time()
#include <stdio.h>          // allows us to print to the terminal prettily
#include <stdlib.h>         // gives us exit(), atexit(), and rand() !
#include <math.h>           // cos(), sin(), etc.

// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

// Globals for OpenGL ---------------------------------------------
static size_t windowWidth = 640;            // the width of our window
static size_t windowHeight = 480;           // the height of our window
static float aspectRatio;                   // the aspect ratio of our window

GLint leftMouseButton, rightMouseButton;    // status of the mouse buttons
GLboolean ctrlClick;                        // status of a control + click
GLint mouseX = 0, mouseY = 0;               // last known X and Y of the mouse

GLfloat cameraX, cameraY, cameraZ;          // camera position in cartesian coordinates
GLfloat cameraTheta, cameraPhi;             // camera DIRECTION in spherical coordinates
GLfloat cameraDirX, cameraDirY, cameraDirZ; // camera DIRECTION in cartesian coordinates
GLfloat cameraRadius;                       // distance to our object of interest

GLfloat propAngle = 0.0f;                   // angle of rotation for our plane propeller

bool LIGHT0_FLAG = true;
bool LIGHT1_FLAG = true;				// Light ON/OFF Flags
GLuint environmentDL;                       // display list for the 'city'

GLint windowId;                             // id for our main window

void cleanupOpenGL();                       // function prototype for our clean OpenGL func

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() {
    return rand() / (float)RAND_MAX;
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
    int gridX = 100;
    int gridY = 100;
    float spacing = 0.5f;

    environmentDL = glGenLists(1);
    glNewList(environmentDL, GL_COMPILE);

    // DRAW OUR CITY
    for(int i = 0; i < gridX-1; i++) {
        for(int j = 0; j < gridY-1; j++) {
            //don't just draw a building ANYWHERE. 
            if( i % 2 && j % 2 && getRand() < 0.4f ) {
                glPushMatrix(); {
                    float height = getRand();
                    height = powf(height, 2.5)*10 + 1;

                    // TODO #02: Add a random material color
					GLfloat color[4] = { getRand(), getRand(), getRand(), 1 };
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
                    glTranslatef( (i - gridX/2.0f)*1.1, 0.0f, (j - gridY/2.0f)*1.1 );
                    glTranslatef(0, height/2.0f, 0);
                	glScalef(1, height, 1);
	                glutSolidCube(1.0);
                }; glPopMatrix();
            }
        }
    }

    // DRAW OUR GRID
	//glDisable( GL_LIGHTING );
    //glColor3f( 1, 1, 1 );
	GLfloat color[4] = { .07568, .61424, .07568, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
    for(float i = -gridX/2 - 1; i <= gridX/2 + 1; i+=spacing) {
        for(float j = -gridY/2 - 1; j <= gridY/2 + 1; j+=spacing) {
        	glPushMatrix(); {
			glBegin( GL_QUADS ); {              // TODO #04: Make this into a quad
				glNormal3f(0, 1, 0);
				glVertex3f( i, 0, j );
				glNormal3f(0, 1, 0);
				glVertex3f(i, 0, j + spacing);
				glNormal3f(0, 1, 0);
				glVertex3f(i + spacing, 0, j + spacing);
				glNormal3f(0, 1, 0);
				glVertex3f( i+spacing, 0, j );

				
				
				
				
			}; glEnd();
		}; glPopMatrix();
        }
    }
	glEnable( GL_LIGHTING );
    
    // DRAW OUR TEAPOT
    glPushMatrix(); {
        // TODO #03: Make our teapot cool
		GLfloat color[4] = { .7038,.27048,.0828,1 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
        //glColor3f( 1, 0, 0 );           // make it RED!
        glFrontFace( GL_CW );           // we are using back-face culling.  There is a bug with the GLUT Teapot that
                                        // the vertices are specified in a clockwise order, so tell OpenGL to use
                                        // a clockwise winding order to denote front faces.
        glTranslatef( 0, 1, 0 );        // translate our teapot above the ground
        glutSolidTeapot( 2.0 );         // draw our teapot
        glFrontFace( GL_CCW );          // make sure we set our winding order back to the default of counter-clockwise
                                        // so future faces will be culled properly.
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
    cameraDirX =  sinf( cameraTheta ) * sinf( cameraPhi );
    cameraDirZ = -cosf( cameraTheta ) * sinf( cameraPhi );
    cameraDirY = -cosf( cameraPhi );

    //and normalize this directional vector!
    float mag = sqrt( cameraDirX*cameraDirX + cameraDirY*cameraDirY + cameraDirZ*cameraDirZ );
    cameraDirX /= mag;  cameraDirY /= mag;  cameraDirZ /= mag;
    
    glutPostRedisplay();
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
void mouseCallback( int button, int state, int thisX, int thisY ) {
    //update the left and right mouse button states, if applicable
    if( button == GLUT_LEFT_BUTTON )
        leftMouseButton = state;
    else if( button == GLUT_RIGHT_BUTTON )
        rightMouseButton = state;
    
    // check if the Control key was pressed while we had a mouse click
    if( glutGetModifiers() & GLUT_ACTIVE_CTRL ) {
        ctrlClick = true;
    } else {
        ctrlClick = false;
    }
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}

// mouseMotion() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for mouse movement. We update cameraPhi, cameraTheta, and/or
//      cameraRadius (for arcBall) based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
////////////////////////////////////////////////////////////////////////////////
void mouseMotion(int x, int y) {
    if( leftMouseButton == GLUT_DOWN ) {
        if( !ctrlClick ) {
            cameraTheta += (x - mouseX)*0.005;
            cameraPhi   += (mouseY - y)*0.005;

            // make sure that phi stays within the range (0, M_PI)
            if(cameraPhi <= 0)
                cameraPhi = 0+0.001;
            if(cameraPhi >= M_PI)
                cameraPhi = M_PI-0.001;
        } else {
            cameraRadius += ((x - mouseX) + (y - mouseY)) * 0.01;
            if( cameraRadius < -2.5 ) {
                cameraRadius = -2.5f;
            }
            if( cameraRadius > 0.5 ) {
                cameraRadius = 0.5f;
            }
        }
        
        recomputeOrientation();     // update camera (x,y,z) based on (radius,theta,phi)
    }
    mouseX = x;
    mouseY = y;
}

// drawPlane() //////////////////////////////////////////////////////////////////
//
//  A very CRUDE plane
//
////////////////////////////////////////////////////////////////////////////////
void drawPlane() {
    glPushMatrix(); {
        glRotatef( -90, 0, 1, 0 );
        glRotatef( 90, 0, 0, 1 );
        
        // the body of our plane
        glPushMatrix(); {
            //glColor3f( 0, 0, 1 );
			GLfloat color[4] = {0,0,1,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glScalef( 2, 0.5, 1 );
            glutSolidCube( 0.1 );
        }; glPopMatrix();
        
        // the left wing
        glPushMatrix(); {
            //glColor3f( 1, 0, 0 );
			GLfloat color[4] = { 1,0,0,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glScalef( 1.5, 0.5, 1 );
            glutSolidCone( 0.05, 0.2, 16, 16 );
        }; glPopMatrix();
        
        // the right wing
        glPushMatrix(); {
            //glColor3f( 1, 0, 0 );
			GLfloat color[4] = { 1,0,0,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glScalef( 1.5, 0.5, 1 );
            glRotatef( 180, 0, 1, 0 );
            glutSolidCone( 0.05, 0.2, 16, 16 );
        }; glPopMatrix();
        
        // the nose
        glPushMatrix(); {
            //glColor3f( 0, 1, 0 );
			GLfloat color[4] = { 0,1,0,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glRotatef( -90, 0, 1, 0 );
            glutSolidCone( 0.025, 0.3, 16, 16 );
            
            glTranslatef( 0, 0, 0.2 );      // move the origin to the front of our nose cone
            
            // TODO #08: Place our second light here
			float pos[4] = { 0, 0, 0, 1.0 };           
			glLightfv(GL_LIGHT1, GL_POSITION, pos);
			float dir[4] = { 0,0,1,0 };
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);
			glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 12);
			glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 100);
        }; glPopMatrix();
        
        // the propeller
        glPushMatrix(); {
            //glColor3f( 1, 1, 1 );
			GLfloat color[4] = { 1,1,1,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glTranslatef( 0.1, 0, 0 );
            glRotatef( propAngle, 1, 0, 0 );
            glScalef( 1.1, 1, 0.025 );
            glutSolidCube( 0.1 );
        }; glPopMatrix();
        glPushMatrix(); {
            //glColor3f( 1, 1, 1 );
			GLfloat color[4] = { 1,1,1,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glTranslatef( 0.1, 0, 0 );
            glRotatef( propAngle+90, 1, 0, 0 );
            glScalef( 1.1, 1, 0.025 );
            glutSolidCube( 0.1 );
        }; glPopMatrix();
        
        // the tail
        glPushMatrix(); {
            //glColor3f( 1, 1, 0 );
			GLfloat color[4] = { 1,1,0,1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
            glRotatef( -90, 1, 0, 0 );
            glutSolidCone( 0.02, 0.1, 16, 16 );
        }; glPopMatrix();
    }; glPopMatrix();
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a scene to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(void)  {
    
    // make sure we are drawing to the back buffer
    glDrawBuffer( GL_BACK );
    
    // update the modelview matrix based on the camera's position
    glMatrixMode( GL_MODELVIEW );                               // make sure we aren't changing the projection matrix!
    glLoadIdentity();                                           // start with a clean matrix
    gluLookAt( cameraX+cameraDirX*cameraRadius, cameraY+cameraDirY*cameraRadius, cameraZ+cameraDirZ*cameraRadius,           // camera is located at (x,y,z) and some zoom distance away
               cameraX+cameraDirX,              cameraY+cameraDirY,              cameraZ+cameraDirZ,                        // camera is looking at at (x,y,z) + (dx,dy,dz) -- straight ahead
               0.0f,                            1.0f,                               0.0f );                                 // up vector is (0,1,0) (positive Y)
    
	float lPosition[4] = { 0.0, 10.0, 0.0, 1.0 };           // TODO #05: MOVE THIS to where? HERE
	glLightfv(GL_LIGHT0, GL_POSITION, lPosition);
    // clear the render buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // execute our display list
    glCallList( environmentDL );
    
    glPushMatrix(); {
        glTranslatef( cameraX+cameraDirX, cameraY+cameraDirY, cameraZ+cameraDirZ ); // we are going to cheat and use our look at point to place our plane
        glRotatef( -cameraTheta*180/M_PI, 0, 1, 0 );            // rotate the plane with our camera theta direction (we need to rotate the opposite direction so we always look at the back)
        glRotatef( cameraPhi*180/M_PI, 1, 0, 0 );               // rotate the plane with our camera phi direction
        drawPlane();                                            // draw our plane now
    }; glPopMatrix();

    // push the back buffer to the screen
    glutSwapBuffers();
}


// normalKeysDown() ////////////////////////////////////////////////////////////
//
//  GLUT keyboard callback; gets called when the user presses a key.
//
////////////////////////////////////////////////////////////////////////////////
void normalKeysDown( unsigned char key, int x, int y )  {
    if( key == 'q' || key == 'Q' || key == 27 ) {
        cleanupOpenGL();
        exit(0);
    }

    // because the direction vector is unit length, and we probably don't want
    // to move one full unit every time a button is pressed, just create a constant
    // to keep track of how far we want to move at each step. you could make
    // this change w.r.t. the amount of time the button's held down for
    // simple scale-sensitive movement!
    float movementConstant = 0.3f;

    // move forward!
    if( key == 'w' || key == 'W' ) {
        // that's as simple as just moving along the direction.
        cameraX += cameraDirX * movementConstant;
        cameraY += cameraDirY * movementConstant;
        cameraZ += cameraDirZ * movementConstant;
        
        propAngle += 25;
        if( propAngle > 360 ) propAngle -= 360;
        
    }

    // move backwards!
    if( key == 's' || key == 'S' ) {
        // just move BACKWARDS along the direction.
        cameraX -= cameraDirX * movementConstant;
        cameraY -= cameraDirY * movementConstant;
        cameraZ -= cameraDirZ * movementConstant;
        
        propAngle -= 25;
        if( propAngle < 0 ) propAngle += 360;
        
    }

	//turn cameras on and off
	if (key == '1') {
		LIGHT0_FLAG = !LIGHT0_FLAG;
		if (LIGHT0_FLAG) {
			glEnable(GL_LIGHT0);
		}
		else if (!LIGHT0_FLAG) {
			glDisable(GL_LIGHT0);
		}
	}
	if (key == '2') {
		LIGHT1_FLAG = !LIGHT1_FLAG;
		if (LIGHT1_FLAG) {
			glEnable(GL_LIGHT1);
		}
		else if (!LIGHT1_FLAG) {
			glDisable(GL_LIGHT1);
		}
	}
    
    glutPostRedisplay();
}

// PrintOpenGLInfo() ////////////////////////////////////////////////////////////
//
//  Print the OpenGL version and attribute info to the screen
//
////////////////////////////////////////////////////////////////////////////////
void PrintOpenGLInfo() {
	int maxNumLights;
	glGetIntegerv( GL_MAX_LIGHTS, &maxNumLights );

    fprintf( stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf( stdout, "[INFO]: | OpenGL Information                                     |\n");
    fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf( stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION) );
    fprintf( stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER) );
    fprintf( stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR) );
    fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf( stdout, "[INFO]: |   Max Number of FFP Lights: %26d |\n", maxNumLights );
    fprintf( stdout, "[INFO]: \\--------------------------------------------------------/\n\n");
}

// initializeOpenGL() //////////////////////////////////////////////////////////
//
//  Do all of our one time OpenGL & GLUT setup
//
////////////////////////////////////////////////////////////////////////////////
void initializeOpenGL( int argc, char *argv[] ) {
    glutInit( &argc, argv );
    
    // create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowPosition( 50, 50 );
    glutInitWindowSize( windowWidth, windowHeight );
    windowId = glutCreateWindow("flight simulator 0.33 alpha");

    glEnable( GL_DEPTH_TEST );          // enable depth testing for hidden surface removal
    
    glEnable( GL_CULL_FACE );           // enable back face culling to speed render times
    glFrontFace( GL_CCW );              // denote front faces specified by counter-clockwise winding order
    glCullFace( GL_BACK );              // cull our back faces
    
    glShadeModel( GL_FLAT );            // use flat shading
    
    glEnable( GL_LIGHTING );            // we are now using lighting
    glEnable( GL_LIGHT0 );              // and turn on Light 0 please (thank you)
    float diffuseCol[4] = { .2, .1, .2, 1.0 };           // TODO #06: Change the colors!
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseCol );
    float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
   
	glDisable(GL_LIGHTING);
    // TODO #07: Set up the next light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	//float lPosition[4] = { 0.0, 10.0, 0.0, 1.0 };          
	//glLightfv(GL_LIGHT1, GL_POSITION, lPosition);
	float diffuseCol1[4] = { 1,1,1,1 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseCol1);
	

    // TODO #01: vvvv Goodbye colors, we're using Materia!  I mean, materials.
    								// tell OpenGL not to use the material system; just use whatever we
    	// pass with glColor*()
    // TODO #01: Delete These ^^^^

    PrintOpenGLInfo();
}

// cleanupOpenGL() /////////////////////////////////////////////////////////////
//
//  At exit, clean up all of our OpenGL objects
//
////////////////////////////////////////////////////////////////////////////////
void cleanupOpenGL() {
    glutDestroyWindow( windowId );  // destroy our window
}

// main() //////////////////////////////////////////////////////////////////////
//
//  Program entry point. Does not process command line arguments.
//
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] ) {
    
    initializeOpenGL( argc, argv );     // do all of our setup for OpenGL
   
    // give the camera a scenic starting point.
    cameraX = 60;
    cameraY = 40;
    cameraZ = 30;
    cameraTheta = -M_PI / 3.0f;
    cameraPhi = M_PI / 2.8f;
    cameraRadius = 0.0f;
    recomputeOrientation();
    
    // register callback functions...
    glutSetKeyRepeat( GLUT_KEY_REPEAT_ON );
    glutKeyboardFunc( normalKeysDown     );
    glutDisplayFunc(  renderScene        );
    glutReshapeFunc(  resizeWindow       );
    glutMouseFunc(    mouseCallback      );
    glutMotionFunc(   mouseMotion        );

    atexit( cleanupOpenGL );            // callback that gets called right before the program exits
                                        // we can pass multiple functions that get placed onto a call stack
    
    srand( time(NULL) );                // seed our random number generator
    generateEnvironmentDL();            // create our environment display list
    
    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
