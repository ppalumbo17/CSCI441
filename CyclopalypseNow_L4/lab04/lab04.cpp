/*
 *  CSCI 441 / 598B, Computer Graphics, Fall 2016
 *
 *  Project: lab04
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2016
 *
 *  Description:
 *      Contains the base code for a basic flight simulator v 0.32 alpha...with sound!
 *
 */

#ifdef __APPLE__			// if compiling on Mac OS
    #include <ALUT/alut.h>  // OpenAL Headers
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>

    #include <GLUT/glut.h>  // OpenGL Headers
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else                       // else compiling on Linux OS
    #include <AL/alut.h>    // OpenAL Headers
    #include <AL/al.h>
    #include <AL/alc.h>

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

GLuint environmentDL;                       // display list for the 'city'

GLint windowId;                             // id for our main window

// Globals for OpenAL ---------------------------------------------
#define NUM_BUFFERS 3
#define NUM_SOURCES 3

ALCdevice *device;
ALCcontext *context;
ALuint buffers[ NUM_BUFFERS ];
ALuint sources[ NUM_SOURCES ];

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
    float spacing = 1.1f;

    environmentDL = glGenLists(1);
    glNewList(environmentDL, GL_COMPILE);

    //psych! everything's on a grid.
    for(int i = 0; i < gridX-1; i++) {
        for(int j = 0; j < gridY-1; j++) {
            //don't just draw a building ANYWHERE. 
            if( i % 2 && j % 2 && getRand() < 0.4f ) {
                glPushMatrix(); {
                    float height = getRand();
                    height = powf(height, 2.5)*10 + 1;

                    glColor3f( getRand(), getRand(), getRand() );
                    glTranslatef( (i - gridX/2.0f)*spacing, 0.0f, (j - gridY/2.0f)*spacing );
                    glTranslatef(0, height/2.0f, 0);
                	glScalef(1, height, 1);
	                glutSolidCube(1.0);
                }; glPopMatrix();
            }
        }
    }

	glDisable( GL_LIGHTING );
    glColor3f( 1, 1, 1 );
    for(int i = -gridX/2 - 1; i <= gridX/2 + 1; i++) {
        for(int j = -gridY/2 - 1; j <= gridY/2 + 1; j++) {
        	glPushMatrix(); {
			glBegin( GL_LINES ); {
				glVertex3f( i, 0, j );
				glVertex3f( i+spacing, 0, j );

				glVertex3f( i, 0, j );
				glVertex3f( i, 0, j+spacing );
			}; glEnd();
		}; glPopMatrix();
        }
    }
	glEnable( GL_LIGHTING );
    
    glPushMatrix(); {
        glColor3f( 1, 0, 0 );           // make it RED!
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

// positionListener() //////////////////////////////////////////////////////////
//
// This function updates the listener's position and orientation.  First, the
//  global listener variables are updated.  Then the position and orientation
//  are set through the approriate OpenAL calls.
//
////////////////////////////////////////////////////////////////////////////////
void positionListener( float posX, float posY, float posZ,
                       float dirX, float dirY, float dirZ,
                       float upX = 0, float upY = 1, float upZ = 0 ) {
    /* TODO #04: Position Our Listener */
	ALfloat listenerPosition[] = { posX,posY,posZ };

	alListenerfv(AL_POSITION, listenerPosition);

	ALfloat listenerOrientation[] = { dirX,dirY,dirZ, upX, upY, upZ };

	alListenerfv(AL_ORIENTATION, listenerOrientation);

}

// positionSource() ////////////////////////////////////////////////////////////
//
// This function updates the sources's position.  The position
//  is set through the approriate OpenAL calls.
//
////////////////////////////////////////////////////////////////////////////////
void positionSource( ALuint src, float posX, float posY, float posZ ) {
    /* TODO #09: Position a Source */
	ALfloat srcPosition[] = { posX, posY, posZ };
	alSourcefv(src, AL_POSITION, srcPosition);
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
            glColor3f( 0, 0, 1 );
            glScalef( 2, 0.5, 1 );
            glutSolidCube( 0.1 );
        }; glPopMatrix();
        
        // the left wing
        glPushMatrix(); {
            glColor3f( 1, 0, 0 );
            glScalef( 1.5, 0.5, 1 );
            glutSolidCone( 0.05, 0.2, 16, 16 );
        }; glPopMatrix();
        
        // the right wing
        glPushMatrix(); {
            glColor3f( 1, 0, 0 );
            glScalef( 1.5, 0.5, 1 );
            glRotatef( 180, 0, 1, 0 );
            glutSolidCone( 0.05, 0.2, 16, 16 );
        }; glPopMatrix();
        
        // the nose
        glPushMatrix(); {
            glColor3f( 0, 1, 0 );
            glRotatef( -90, 0, 1, 0 );
            glutSolidCone( 0.025, 0.3, 16, 16 );
        }; glPopMatrix();
        
        // the propeller
        glPushMatrix(); {
            glColor3f( 1, 1, 1 );
            glTranslatef( 0.1, 0, 0 );
            glRotatef( propAngle, 1, 0, 0 );
            glScalef( 1.1, 1, 0.025 );
            glutSolidCube( 0.1 );
        }; glPopMatrix();
        glPushMatrix(); {
            glColor3f( 1, 1, 1 );
            glTranslatef( 0.1, 0, 0 );
            glRotatef( propAngle+90, 1, 0, 0 );
            glScalef( 1.1, 1, 0.025 );
            glutSolidCube( 0.1 );
        }; glPopMatrix();
        
        // the tail
        glPushMatrix(); {
            glColor3f( 1, 1, 0 );
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
    
    /* TODO #05: Place our Listener every frame */
	positionListener(cameraX + cameraDirX*cameraRadius, cameraY + cameraDirY*cameraRadius, cameraZ + cameraDirZ*cameraRadius,
					cameraX + cameraDirX, cameraY + cameraDirY, cameraZ + cameraDirZ,
					0, 1, 0);
    /* TODO #??: Place our moving sources every frame */
	positionSource(sources[1], cameraX + cameraDirX, cameraY + cameraDirY, cameraZ + cameraDirZ);
	positionSource(sources[2], cameraX + cameraDirX*cameraRadius, cameraY + cameraDirY*cameraRadius, cameraZ + cameraDirZ*cameraRadius);
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
        exit(0);
    }

    // because the direction vector is unit length, and we probably don't want
    // to move one full unit every time a button is pressed, just create a constant
    // to keep track of how far we want to move at each step. you could make
    // this change w.r.t. the amount of time the button's held down for
    // simple scale-sensitive movement!
    const float MOVEMENT_CONSTANT = 0.3f;

    // move forward!
    if( key == 'w' || key == 'W' ) {
        // that's as simple as just moving along the direction.
        cameraX += cameraDirX * MOVEMENT_CONSTANT;
        cameraY += cameraDirY * MOVEMENT_CONSTANT;
        cameraZ += cameraDirZ * MOVEMENT_CONSTANT;
        
        propAngle += 25;
        if( propAngle > 360 ) propAngle -= 360;

		ALenum sourceState;
		alGetSourcei(sources[1], AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING) {
			alSourcePlay(sources[1]);
		}
        
    }

    // move backwards!
    if( key == 's' || key == 'S' ) {
        // just move BACKWARDS along the direction.
        cameraX -= cameraDirX * MOVEMENT_CONSTANT;
        cameraY -= cameraDirY * MOVEMENT_CONSTANT;
        cameraZ -= cameraDirZ * MOVEMENT_CONSTANT;
        
        propAngle -= 25;
        if( propAngle < 0 ) propAngle += 360;
        
		ALenum sourceState;
		alGetSourcei(sources[1], AL_SOURCE_STATE, &sourceState);
		if (sourceState != AL_PLAYING) {
			alSourcePlay(sources[1]);
		}
    }
    
    glutPostRedisplay();
}

void normalKeysUp( unsigned char key, int x, int y ) {
    if( key == 'w' || key == 'W'
       || key == 's' || key == 'S' ) {
		alSourceStop(sources[1]);
    }
}

// PrintOpenGLInfo() ////////////////////////////////////////////////////////////
//
//  Print the OpenGL version and attribute info to the screen
//
////////////////////////////////////////////////////////////////////////////////
void PrintOpenGLInfo() {
    fprintf( stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf( stdout, "[INFO]: | OpenGL Information                                     |\n");
    fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf( stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION) );
    fprintf( stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER) );
    fprintf( stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR) );
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
    windowId = glutCreateWindow("flight simulator 0.32 alpha");

    glEnable( GL_DEPTH_TEST );          // enable depth testing for hidden surface removal
    
    glEnable( GL_CULL_FACE );           // enable back face culling to speed render times
    glFrontFace( GL_CCW );              // denote front faces specified by counter-clockwise winding order
    glCullFace( GL_BACK );              // cull our back faces
    
    glShadeModel( GL_FLAT );            // use flat shading
    
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

    PrintOpenGLInfo();					// print our OpenGL versioning information
}

// cleanupOpenGL() /////////////////////////////////////////////////////////////
//
//  At exit, clean up all of our OpenGL objects
//
////////////////////////////////////////////////////////////////////////////////
void cleanupOpenGL() {
    glutDestroyWindow( windowId );  // destroy our window
}

// PrintOpenALInfo() ////////////////////////////////////////////////////////////
//
//  Print the OpenAL version and attribute info to the screen
//
////////////////////////////////////////////////////////////////////////////////
void PrintOpenALInfo() {
    fprintf( stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf( stdout, "[INFO]: | OpenAL Information                                     |\n");
    fprintf( stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf( stdout, "[INFO]: |   OpenAL Version:  %35s |\n", alGetString(AL_VERSION) );
    fprintf( stdout, "[INFO]: |   OpenAL Renderer: %35s |\n", alGetString(AL_RENDERER) );
    fprintf( stdout, "[INFO]: |   OpenAL Vendor:   %35s |\n", alGetString(AL_VENDOR) );
    fprintf( stdout, "[INFO]: \\--------------------------------------------------------/\n\n");
}

// initializeOpenAL() //////////////////////////////////////////////////////////
//
//  Do all of our one time OpenAL & ALUT setup
//
////////////////////////////////////////////////////////////////////////////////
void initializeOpenAL( int argc, char *argv[] ) {
    ALsizei size, freq;
    ALenum 	format;
    ALvoid 	*data;
    ALboolean loop;
    
    /* TODO #01: Setup ALUT and OpenAL */
	alutInit(&argc, argv);

	ALCdevice *device = alcOpenDevice(NULL); // open default
	ALCcontext *context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
    
    /* TODO #06: Generate Buffers & Sources */
	alGenBuffers(NUM_BUFFERS, buffers);
	alGenSources(NUM_SOURCES, sources);
    /* TODO #08: Create Our Stationary Sound */
	#ifdef __APPLE__
		alutLoadWAVFile((ALbyte*)"wavs/siren.wav", &format, &data, &size, &freq);
	#else
		alutLoadWAVFile((ALbyte*)"wavs/siren.wav", &format, &data, &size, &freq, &loop);
	#endif
		alBufferData(buffers[0], format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

	alSourcei(sources[0], AL_BUFFER, buffers[0]);
	alSourcei(sources[0], AL_LOOPING, AL_TRUE);
	//Create our Moving Listener
	#ifdef __APPLE__
			alutLoadWAVFile((ALbyte*)"wavs/Running.wav", &format, &data, &size, &freq);
	#else
			alutLoadWAVFile((ALbyte*)"wavs/Running.wav", &format, &data, &size, &freq, &loop);
	#endif
			alBufferData(buffers[1], format, data, size, freq);
			alutUnloadWAV(format, data, size, freq);

	alSourcei(sources[1], AL_BUFFER, buffers[1]);
	alSourcei(sources[1], AL_LOOPING, AL_TRUE);
	//Create Background Music
	#ifdef __APPLE__
		alutLoadWAVFile((ALbyte*)"wavs/background.wav", &format, &data, &size, &freq);
	#else
		alutLoadWAVFile((ALbyte*)"wavs/background.wav", &format, &data, &size, &freq, &loop);
	#endif
		alBufferData(buffers[2], format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

	alSourcei(sources[2], AL_BUFFER, buffers[2]);
	alSourcei(sources[2], AL_LOOPING, AL_TRUE);
    /* TODO #10: Position our Stationary Source */
	positionSource(sources[0], 0, 0, 0);
	positionSource(sources[2], cameraX, cameraY, cameraZ);

    PrintOpenALInfo();					// print our OpenAL versioning information
}

// cleanupOpenAL() /////////////////////////////////////////////////////////////
//
//  At exit, clean up all of our OpenAL objects
//
////////////////////////////////////////////////////////////////////////////////
void cleanupOpenAL() {
    /* TODO #03: Cleanup OpenAL & ALUT */
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	alutExit();
    
    /* TODO #07: Delete our Buffers and Sources */
	alDeleteBuffers(NUM_BUFFERS, buffers);
	alDeleteSources(NUM_SOURCES, sources);
}

// main() //////////////////////////////////////////////////////////////////////
//
//  Program entry point. Does not process command line arguments.
//
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] ) {
    
    initializeOpenGL( argc, argv );     // do all of our setup for OpenGL
    initializeOpenAL( argc, argv );     // do all of our setup for OpenAL
   
    // give the camera a scenic starting point.
    cameraX = 60;
    cameraY = 40;
    cameraZ = 30;
    cameraTheta = -M_PI / 3.0f;
    cameraPhi = M_PI / 2.8f;
    cameraRadius = 0.0f;
    recomputeOrientation();
    
    /* TODO #12: Position our Listener before we start playing the sound */
	positionListener(cameraX + cameraDirX*cameraRadius, cameraY + cameraDirY*cameraRadius, cameraZ + cameraDirZ*cameraRadius,
		cameraX + cameraDirX, cameraY + cameraDirY, cameraZ + cameraDirZ,
		0, 1, 0);
    /* TODO #??: Position our other sources appropriately before they start playing */
    
    // register callback functions...
    glutSetKeyRepeat( GLUT_KEY_REPEAT_ON );
    glutKeyboardFunc( normalKeysDown     );
    glutKeyboardUpFunc( normalKeysUp     );
    glutDisplayFunc(  renderScene        );
    glutReshapeFunc(  resizeWindow       );
    glutMouseFunc(    mouseCallback      );
    glutMotionFunc(   mouseMotion        );

    /* TODO #02: Add a callback to clean up OpenAL */
    atexit( cleanupOpenGL );            // callback that gets called right before the program exits
	atexit(cleanupOpenAL);                                     // we can pass multiple functions that get placed onto a call stack
    
    srand( time(NULL) );                // seed our random number generator
    generateEnvironmentDL();            // create our environment display list

    /* TODO #11: Play our stationary source FOREVER! */
	alSourcePlay(sources[0]);
	alSourcePlay(sources[2]);
    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
