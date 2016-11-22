/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab11
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the implementation of a simple collision
 *  detection.  Particles, i.e. spheres, roll around the screen.
 *  If they collide with the edge of the bounding box, they bounce
 *  back into the world.  There are checks to also see if two
 *  particles collide in a perfectly elastic collision.
 *
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
 *
 */


// ALLLLLLLL of the includes that we use
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <SOIL/soil.h>

#include "Point.h"
#include "Vector.h"
#include "Ball.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fstream>
#include <list>
#include <string>
#include <vector>
using namespace std;


// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

int windowWidth = 512, windowHeight = 512;

GLint leftMouseButton, rightMouseButton;    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

float cameraTheta, cameraPhi, cameraRadius; // camera position in spherical coordinates
Point cameraXYZ;                            // camera position in cartesian coordinates

GLuint brickTexHandle, groundTexHandle;     // a handle to the textures in our OpenGL context
GLuint skyboxHandles[6];                    // all of our skybox handles

vector< Ball* > balls;                      // a collection of the balls in our scene

// some parameters to control the running of our program
float groundSize = 10;                      // the size of our ground plane and therefore bounding box
float ballRadius = 0.5;                     // the base radius of all our spheres (when created, the spheres will
                                                // have an actual radius of ballRadius +/- rand()
int numBalls = 10;                          // the number of balls in our scene

// END GLOBAL VARIABLES ///////////////////////////////////////////////////////

//
// void computeArcballPosition(float theta, float phi, float radius,
//                              float &x, float &y, float &z);
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. The user passes in the current phi,
//  theta, and radius, as well as variables to hold the resulting X, Y, and Z values.
//  Those variables for X, Y, and Z values get filled with the camera's position in R3.
//
void computeArcballPosition(float theta, float phi, float radius,
                            Point &xyz) {
    xyz.setX( radius * sinf(theta)*sinf(phi) );
    xyz.setY( radius * -cosf(phi) );
    xyz.setZ( radius * -cosf(theta)*sinf(phi) );
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
    
    // update the viewport to fill the window
    glViewport(0, 0, w, h);
    
    // update the projection matrix with the new window properties
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
//  GLUT callback for mouse movement. We update cameraPhi, cameraTheta, and/or
//      cameraRadius based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
void mouseMotion(int x, int y) {
    // mouse is moving; if left button is held down...
    if(leftMouseButton == GLUT_DOWN ) {
        
        // update theta and phi!
        cameraTheta += (x - mouseX)*0.005;
        cameraPhi   += (y - mouseY)*0.005;
        
        // we don't care if theta goes out of bounds; it'll just loop around.
        // phi, though.. it'll flip out if it flips over top. keep it in (0, M_PI)
        if(cameraPhi <= 0)
        cameraPhi = 0+0.001;
        if(cameraPhi >= M_PI)
        cameraPhi = M_PI-0.001;
        
        // update camera (x,y,z) based on (radius,theta,phi)
        computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
                               cameraXYZ);
    } else if( rightMouseButton == GLUT_DOWN ) {
        // for the right mouse button, just determine how much the mouse has moved total.
        // not the best "zoom" behavior -- if X change is positive and Y change is negative,
        // (along the X = Y line), nothing will happen!! but it's ok if you zoom by
        // moving left<-->right or up<-->down, which works for most people i think.
        double totalChangeSq = (x - mouseX) + (y - mouseY);
        cameraRadius += totalChangeSq*0.01;
        
        // limit the camera radius to some reasonable values so the user can't get lost
        if(cameraRadius < 2.0)
        cameraRadius = 2.0;
        if(cameraRadius > 150.0)
        cameraRadius = 150.0;
        
        // update camera (x,y,z) based on (radius,theta,phi)
        computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
                               cameraXYZ);
    }
    
    // and save the current mouseX and mouseY.
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
void initScene() {
    // tell OpenGL to use a depth test
    glEnable(GL_DEPTH_TEST);
    
    // tell OpenGL not to use the material system; just use whatever we pass with glColor()
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    
    // tells OpenGL to not blend colors across triangles
    glShadeModel( GL_SMOOTH );
    
    // and enable alpha blending once and for all.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//
//  void populateBalls()
//
//  Randomly create numBalls balls.  Each ball will be randomly placed on our ground
//      plane.  It will be pointed in a random direction and will have a random
//      radius.  It IS possible that two spheres are created in the same place.
//      We'll just live with this result and restart the program if it happens.
//
void populateBalls() {
    srand( time(NULL) );
    float rangeX = groundSize*2;
    float rangeZ = groundSize*2;
    
    // for each ball
    for(int i = 0; i < numBalls; i++) {
        // create a new ball
        balls.push_back(  new Ball( Point( rand()/(float)RAND_MAX * rangeX - rangeX/2.0f,           // at a random location X
                                           0.0f,                                                        // Y
                                           (rangeZ * (i/(float)numBalls)) - rangeZ/2.0f),               // Z
                                    Vector( rand()/(float)RAND_MAX, 0.0, rand()/(float)RAND_MAX ),  // with a random direction
                                    ballRadius + (rand()/(float)RAND_MAX - 0.5)*0.5 ) );            // and a random radius
    }
}

//
//  void drawSkyboxPanel( ...params... )
//
//  Draws a single textured quad.  This became a bit more complex than intended, but it
//      allows for a generic definition.  We specify the two in-plane dimensions (dim1
//      and dim2) to determine our four corners.  These corners are centered around
//      point and are size distance away.  The quad is oriented with the provided
//      normal.  The provided texture is applied to the quad and can be flipped
//      along either axis if needed.
//
void drawSkyboxPanel( GLuint texHandle, Point point, Vector dim1, Vector dim2, Vector normal, int flippingFactor, int flippingFactor2, int size ) {
    // we're cheating by making our quads intersect at the edges to try and remove the joint line we sometimes see
    Point p1 = point + dim1*-(size+1) + dim2*-(size+1);  // LL corner
    Point p2 = point + dim1*(size+1) + dim2*-(size+1);   // LR corner
    Point p3 = point + dim1*(size+1) + dim2*(size+1);    // UR corner
    Point p4 = point + dim1*-(size+1) + dim2*(size+1);   // UL corner
    
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texHandle );
    glPushMatrix(); {
        glColor4f(1,1,1,1);
        glBegin( GL_QUADS ); {
            glTexCoord2f(0, 0);
            normal.glNormal();
            p1.glVertex();
            
            glTexCoord2f(1*flippingFactor, 0);
            normal.glNormal();
            p2.glVertex();
            
            glTexCoord2f(1*flippingFactor, 1*flippingFactor2);
            normal.glNormal();
            p3.glVertex();
            
            glTexCoord2f(0, 1*flippingFactor2);
            normal.glNormal();
            p4.glVertex();
        }; glEnd();
    }; glPopMatrix();
    glDisable( GL_TEXTURE_2D );
}

//
//  void drawSkybox( int size )
//
//  Draws the six planes of our sky box.  Texturing each and having them oriented the correct way.
//
void drawSkybox( int size ) {
    //                 texHandle             Point                  dim1            dim2            Normal         flip   size          panel
    drawSkyboxPanel( skyboxHandles[0], Point(   -1,     0, -size), Vector(1,0,0), Vector(0,1,0), Vector( 0, 0, 1), -1,  1, size );     // back
    drawSkyboxPanel( skyboxHandles[1], Point(-size,     0,    -1), Vector(0,0,1), Vector(0,1,0), Vector( 1, 0, 0),  1,  1, size );     // left
    drawSkyboxPanel( skyboxHandles[2], Point(    1,     0,  size), Vector(1,0,0), Vector(0,1,0), Vector( 0, 0,-1),  1,  1, size );     // front
    drawSkyboxPanel( skyboxHandles[3], Point( size,     0,     1), Vector(0,0,1), Vector(0,1,0), Vector(-1, 0, 0), -1,  1, size );     // right
    drawSkyboxPanel( skyboxHandles[4], Point(    0, -size,     0), Vector(1,0,0), Vector(0,0,1), Vector( 0, 1, 0), -1, -1, size );     // bottom
    drawSkyboxPanel( skyboxHandles[5], Point(    0,  size,     0), Vector(1,0,0), Vector(0,0,1), Vector( 0,-1, 0), -1,  1, size );     // top
    //                 texHandle             Point                  dim1            dim2            Normal         flip   size          panel
}

//
//  void renderScene()
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a teapot to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
void renderScene(void) {
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    // set our camera position
    gluLookAt( cameraXYZ, Point(0,0,0), Vector(0,1,0) );    // Hey! I rewrote the gluLookAt() to take gluLookAt( Point, Point, Vector )
    
    // clear the render buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // draw our ground
    glPushMatrix(); {
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
        float squareSize = groundSize + ballRadius;
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, groundTexHandle );
        glBegin( GL_QUADS ); {
            glNormal3f( 0, 1, 0 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( -squareSize, 0, -squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( -squareSize, 0,  squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f(  squareSize, 0,  squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f(  squareSize, 0, -squareSize );
        }; glEnd();
    }; glPopMatrix();
    
    // draw our skybox
    drawSkybox( 200 );
    
    // enable textures.
    glEnable( GL_TEXTURE_2D );
    glColor4f(1,1,1,1);
    glBindTexture(GL_TEXTURE_2D, brickTexHandle);   // make all of our spheres bricky

    // draw every sphere
    for( unsigned int i = 0; i < balls.size(); i++ ) {
        balls[i]->draw();
    }
    
    glDisable( GL_TEXTURE_2D );
    
    //push the back buffer to the screen
    glutSwapBuffers();
}

//
//  void normalKeysDown(unsigned char key, int x, int y)
//
//      GLUT keyboard callback for when a regular key is pressed.
//
void normalKeysDown(unsigned char key, int x, int y) {
    switch( key ) {
        case 'q':
        case 'Q':
        exit(0);
        break;
    }
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
void myTimer(int value){
    // TODO #1
    //   move every ball forward along its heading
	for (int i = 0; i < balls.size(); i++) {
		balls.at(i)->moveForward();
		/*if (balls.at(i)->location.getX() > groundSize ||
			balls.at(i)->location.getX() < -groundSize ||
			balls.at(i)->location.getZ() > groundSize ||
			balls.at(i)->location.getZ() < -groundSize) {

			balls.at(i)->moveBackward();*/
			
			if (balls.at(i)->location.getX() > groundSize) {
				balls.at(i)->moveBackward();
				Vector normal = Vector(-1, 0, 0);
				balls.at(i)->direction = balls.at(i)->direction - 2 * dot(balls.at(i)->direction, normal)*normal;
			}
			else if (balls.at(i)->location.getX() < -groundSize) {
				balls.at(i)->moveBackward();
				Vector normal = Vector(1, 0, 0);
				balls.at(i)->direction = balls.at(i)->direction - 2 * dot(balls.at(i)->direction, normal)*normal;
			}
			else if (balls.at(i)->location.getZ() > groundSize) {
				balls.at(i)->moveBackward();
				Vector normal = Vector(0, 0, -1);
				balls.at(i)->direction = balls.at(i)->direction - 2 * dot(balls.at(i)->direction, normal)*normal;
			}
			else if (balls.at(i)->location.getZ() < -groundSize) {
				balls.at(i)->moveBackward();
				Vector normal = Vector(0, 0, 1);
				balls.at(i)->direction = balls.at(i)->direction - 2 * dot(balls.at(i)->direction, normal)*normal;
			}
			//This is breaking it
			for (int j = 0; j < balls.size()-1; j++) {
				if (i == j) {
					j++;
				}
				double dist = sqrt(pow(balls.at(i)->location.getX() - balls.at(j)->location.getX(), 2) +
					pow(balls.at(i)->location.getY() - balls.at(j)->location.getY(), 2));
				if (balls.at(i)->radius + balls.at(j)->radius > dist) {
					balls.at(i)->moveBackward();
					balls.at(j)->moveBackward();

				}
			}
		//}
		/*
		if (balls.at(i)->location.getX() > groundSize) {
			balls.at(i)->moveBackward();
		}
		else if (balls.at(i)->location.getX() < -groundSize) {
			balls.at(i)->moveBackward();
		}
		else if (balls.at(i)->location.getZ() > groundSize) {
			balls.at(i)->moveBackward();
		}
		else if (balls.at(i)->location.getZ() < -groundSize) {
			balls.at(i)->moveBackward();
		}
		*/
	}
    
    
    // TODO #2
    //   check every ball if it "collides" with the edges of our ground plane
    
    
    // TODO #3
    //   check for interball collisions
    //   warning this may not be perfect...balls can get caught and
    //     continually bounce back-and-forth in place off of
    //     each other but with fewer balls we would need just the
    //     right conditions of multiple balls colliding all at
    //     once.
    
    
    glutPostRedisplay();
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
}

//
//  registerOpenGLRGBTexture( ...params... )
//
//  Takes in the RGB data of a texture of size width * height.  Registers
//      the texture with OpenGL setting the handle value to textureHandle.
//      Also sets the texture parameters for Min, Mag, S, T, and application
//      mode.
//
bool registerOpenGLRGBTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {
    // make sure we are receiving data
    if(textureData == 0) {
        fprintf(stderr,"Cannot register texture; no data specified.");
        return false;
    }
    
    glGenTextures(1, &textureHandle);                                   // generate a handle
    glBindTexture(GL_TEXTURE_2D, textureHandle);                        // bind the handle
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        // set our mode to modulate with lighting
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // lerp on min filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // lerp on mag filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // repeat along S dimension
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // repeat along T dimesion
    
    GLenum errCode;
    const GLubyte *errString;
    
    // check for error
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        fprintf(stderr,"Cannot register texture; OpenGL error: %s.", errString);
        return false;
    }
    
    // send the texture to the GPU
    glTexImage2D( GL_TEXTURE_2D,    // texture type (a 2D texture)
                              0,    // level of detail
                         GL_RGB,    // internal storage data format
                       texWidth,    // width of texture
                      texHeight,    // height of texture
                              0,    // border = 0
                         GL_RGB,    // display format, must match the internal format
               GL_UNSIGNED_BYTE,    // data type in storage
                    textureData);   // the actual data
    
    return true;
}


// readPPM() ///////////////////////////////////////////////////////////////////
//
//  This function reads an ASCII PPM, returning true if the function succeeds and
//      false if it fails. If it succeeds, the variables imageWidth and
//      imageHeight will hold the width and height of the read image, respectively.
//
//  It's not terribly robust.
//
//  Returns the image as an unsigned character array containing
//      imageWidth*imageHeight*3 entries (for that many bytes of storage).
//
//  NOTE: this function expects imageData to be UNALLOCATED, and will allocate
//      memory itself. If the function fails (returns false), imageData
//      will be set to NULL and any allocated memory will be automatically deallocated.
//
////////////////////////////////////////////////////////////////////////////////
bool readPPM(string filename, int &imageWidth, int &imageHeight, unsigned char* &imageData)
{
    FILE *fp = fopen(filename.c_str(), "r");
    int temp, maxValue;
    fscanf(fp, "P%d", &temp);
    if(temp != 3)
    {
        fprintf(stderr, "Error: PPM file is not of correct format! (Must be P3, is P%d.)\n", temp);
        fclose(fp);
        return false;
    }
    
    //got the file header right...
    fscanf(fp, "%d", &imageWidth);
    fscanf(fp, "%d", &imageHeight);
    fscanf(fp, "%d", &maxValue);
    
    //now that we know how big it is, allocate the buffer...
    imageData = new unsigned char[imageWidth*imageHeight*3];
    if(!imageData)
    {
        fprintf(stderr, "Error: couldn't allocate image memory. Dimensions: %d x %d.\n", imageWidth, imageHeight);
        fclose(fp);
        return false;
    }
    
    //and read the data in.
    for(int j = 0; j < imageHeight; j++)
    {
        for(int i = 0; i < imageWidth; i++)
        {
            int r, g, b;
            fscanf(fp, "%d", &r);
            fscanf(fp, "%d", &g);
            fscanf(fp, "%d", &b);
            
            imageData[(j*imageWidth+i)*3+0] = r;
            imageData[(j*imageWidth+i)*3+1] = g;
            imageData[(j*imageWidth+i)*3+2] = b;
        }
    }
    
    fclose(fp);
    return true;
}

//
//  registerSOILTexture( filename, handle )
//
//  Loads a texture using SOIL and registers it with OpenGL
//
void registerSOILTexture( string filename, GLuint &handle ) {
    // load the texture image and return a handle to the texture
    handle = SOIL_load_OGL_texture( filename.c_str(),
                                            SOIL_LOAD_AUTO,
                                            SOIL_CREATE_NEW_ID,
                                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT );
    glBindTexture(GL_TEXTURE_2D, handle);                               // bind the handle
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        // set our mode to modulate with lighting
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );   // lerp MIPMAP and lerp on texel on min filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // lerp on mag filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // repeat along S dimension
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // repeat along T dimesion
}

//
//  registerTextures()
//
//  Register all of our textures with OpenGL.  This includes the brick for our spheres,
//      our ground plane, and our skybox.  Provide some output to the user so they know
//      what is causing the loading delay.
//
void registerTextures() {
    ////// LOAD IN OUR TEXTURES //////////////////////////////////////////////////////////////
    unsigned char *brickTexData;
    int brickTexWidth, brickTexHeight;
    readPPM("textures/brick.ppm", brickTexWidth, brickTexHeight, brickTexData);
    registerOpenGLRGBTexture(brickTexData, brickTexWidth, brickTexHeight, brickTexHandle);
    printf( "[INFO]: brick texture read in and registered\n" );
    
    registerSOILTexture( "textures/metal.jpg", groundTexHandle );
    printf( "[INFO]: ground texture read in and registered\n" );
    
    printf( "[INFO]: registering skybox..." );
    fflush( stdout );
    registerSOILTexture( "textures/skybox/back.png",   skyboxHandles[0] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/right.png",  skyboxHandles[1] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/front.png",  skyboxHandles[2] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/left.png",   skyboxHandles[3] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/bottom.png", skyboxHandles[4] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/top.png",    skyboxHandles[5] );   printf( "." ); fflush( stdout );
    printf( "skybox textures read in and registered!\n" );
    //////////////////////////////////////////////////////////////////////////////////////////
}

// main() //////////////////////////////////////////////////////////////////////
//
//  Program entry point. Does not process command line arguments.
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    //create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("Lab11: Collision Detection");
    
    printf( "[INFO]: GLUT initialized and OpenGL Context created\n" );
    
    fprintf(stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf(stdout, "[INFO]: | OpenGL Information                                     |\n");
    fprintf(stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf(stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION));
    fprintf(stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER));
    fprintf(stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR));
    fprintf(stdout, "[INFO]: \\--------------------------------------------------------/\n\n");
    
    //give the camera a 'pretty' starting point!
    cameraRadius = 25.0f;
    cameraTheta = 2.80;
    cameraPhi = 2.0;
    computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
                           cameraXYZ);
    
    // register callback functions...
    glutKeyboardFunc(normalKeysDown);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);
    
    // do some basic OpenGL setup
    initScene();
    printf( "[INFO]: OpenGL Scene Initialized\n" );
    registerTextures();
    printf( "[INFO]: All textures registered\n" );
    // create all of our balls
    populateBalls();
    printf( "[INFO]: Spheres have been randomly generated\n" );
    
    // start our timer
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
    
    printf( "[INFO]: And here...we...go...\n\n" );
    
    //and enter the GLUT loop, never to exit.
    glutMainLoop();
    
    return(0);
}
