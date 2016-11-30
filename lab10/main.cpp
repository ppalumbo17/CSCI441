/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab10
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the implementation of a simple transparent texturing
 *      example. It loads in a texture, an alpha map, and combines them into an
 *      RGBA texture, which is then applied to a number of quads in the scene.
 *      The quads themselves are sorted from back to front in Z, but that means
 *      that when the camera moves to the far side of them and looks at them,
 *      they will not be rendered correctly (due to OpenGL's inherent restrictions
 *      w.r.t. transparent rendering). For correct rendering, they must be dynamically
 *      sorted each frame before they are rendered!
 *
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
 *
 */


#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <SOIL/soil.h>

#include "Point.h"
#include "Vector.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>
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
Point forestCenter;                         // center of our forest

GLuint treeTexHandle, groundTexHandle;      // a handle to the textures in our OpenGL context

// contains the coordinates of tree sprite positions.
// is filled randomly at the start of execution, and then referenced
// during rendering.
vector< Point > treeSpritePositions;


bool mySort(Point a, Point b){
    Vector view = Vector(forestCenter.getX()-cameraXYZ.getX(), 0.0, forestCenter.getZ()-cameraXYZ.getZ());
    //view.normalize();
    Vector spriteA = Vector(a.getX()-cameraXYZ.getX(), 0.0,a.getZ()-cameraXYZ.getZ());
    //spriteA.normalize();
    Vector spriteB = Vector(b.getX()-cameraXYZ.getX(), 0.0,b.getZ()-cameraXYZ.getZ());
    //spriteB.normalize();
    double aDist = dot(view, spriteA)/view.mag();
    double bDist = dot(view, spriteB)/view.mag();
    return (aDist>bDist);
}
//gets the distance of the sprite
double getSpriteDistance(Point sprite){
    Vector view = Vector(forestCenter.getX()-cameraXYZ.getX(), 0.0, forestCenter.getZ()-cameraXYZ.getZ());
    view.normalize();
    Vector spriteV = Vector(sprite.getX()-cameraXYZ.getX(), sprite.getY()-cameraXYZ.getY(),sprite.getZ()-cameraXYZ.getZ());
    spriteV.normalize();
    double dp = dot(view, spriteV);
    return dp;
}
//used in quicksort
// int partition(vector<Point> positions, int start, int end){
int partition(int start, int end){
    int i = start +1;
    // Point piv = positions.at(i);
    Point piv = treeSpritePositions.at(i);
    for(int j = start +1; j <= end; j++){
        if(getSpriteDistance(treeSpritePositions.at(j)) > getSpriteDistance(piv)){
            // iter_swap(positions.begin() + j, positions.begin() + i);
            iter_swap(treeSpritePositions.begin() + j, treeSpritePositions.begin() + i);
            i++;
        }
    }
    // iter_swap(positions.begin() + start, positions.begin() + i-1);
    iter_swap(treeSpritePositions.begin() + start, treeSpritePositions.begin() + i-1);
    return i-1;
} 
// void quicksort(vector<Point> positions, int start, int end){
void quicksort(int start, int end){
    if(start < end){
        int piv_pos = partition(start, end);
        quicksort(start, piv_pos-1);
        quicksort(piv_pos+1, end);
    }
}
// END GLOBAL VARIABLES ///////////////////////////////////////////////////////

//
// registerTransparentOpenGLTexture()
//
//  Similar to the helper function that registers a single OpenGL texture, this
//      function registers a texture that has 4 channels (RGBA), but specifically
//      is used when the alpha channel is a separate array. This combination
//      could easily be performed in the function that reads the images from file instead.
//
bool registerTransparentOpenGLTexture(unsigned char *imageData, unsigned char *imageMask, int texWidth, int texHeight, GLuint &texHandle) {
    // combine the 'mask' array with the image data array into an RGBA array.
    unsigned char *fullData = new unsigned char[texWidth*texHeight*4];
    for(int j = 0; j < texHeight; j++) {
        for(int i = 0; i < texWidth; i++) {
            fullData[(j*texWidth+i)*4+0] = imageData[(j*texWidth+i)*3+0];	// R
            fullData[(j*texWidth+i)*4+1] = imageData[(j*texWidth+i)*3+1];	// G
            fullData[(j*texWidth+i)*4+2] = imageData[(j*texWidth+i)*3+2];	// B
            fullData[(j*texWidth+i)*4+3] = imageMask[(j*texWidth+i)*3+0];	// A
        }
    }
    
    // first off, get a real texture handle.
    glGenTextures(1, &texHandle);
    
    // make sure that future texture functions affect this handle
    glBindTexture(GL_TEXTURE_2D, texHandle);
    
    // set this texture's color to be multiplied by the surface colors --
    //  GL_MODULATE instead of GL_REPLACE allows us to take advantage of OpenGL lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // set the minification and magnification functions to be linear; not perfect
    //  but much better than nearest-texel (GL_NEAREST).
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Set the texture to repeat in S and T -- though it doesn't matter here
    //  because our texture coordinates are always in [0,0] to [1,1].
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // actually transfer the texture to the GPU and use MipMaps!!
    /*    gluBuild2DMipMaps(
     GL_TEXTURE_2D,              //still working with 2D textures, obv.
     GL_RGBA,                    //we're going to provide OpenGL with R, G, B, and A components...
     texWidth,                   //...of this width...
     texHeight,                  //...and this height.
     GL_RGBA,                    //and store it, internally, as RGBA (OpenGL will convert to floats between 0.0 and 1.0f)
     GL_UNSIGNED_BYTE,           //this is the format our data is in, and finally,
     fullData);                  //there's the data!
     */
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, fullData );
    
    // clean up our memory on the CPU side since it's already been transfered to the GPU
    delete fullData;
    
    // whoops! i guess this function can't fail. in an ideal world, there would
    // be checks with glGetError() that we could use to determine if this failed.
    return true;
}

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
void resizeWindow(int w, int h)
{
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
    }
    else if( rightMouseButton == GLUT_DOWN ) {
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
    glEnable(GL_DEPTH_TEST);
    
    // tell OpenGL not to use the material system; just use whatever we pass with glColor()
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    
    // tells OpenGL to not blend colors across triangles
    glShadeModel( GL_FLAT );
    
    // fill up the treeSpritePositions with some meaningful information...
    srand( time(NULL) );
    forestCenter = Point(0, 0, 0);
    int numSprites = 50;
    float rangeX = numSprites;
    float rangeZ = numSprites;
    for(int i = 0; i < numSprites; i++) {
        // TODO #1: Populate our sprite locations
        treeSpritePositions.push_back(Point(rand()% (numSprites+1) + (-numSprites/2), 0, i - numSprites/2));

    }
    
    // and enable alpha blending once and for all.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//
//  void drawGround()
//
//  Draws a textured ground quad in the XZ plane
//
void drawGround() {
    // draw our ground
    glPushMatrix(); {
        glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
        float squareSize = 400;
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, groundTexHandle );
        glBegin( GL_QUADS ); {
            glNormal3f( 0, 1, 0 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( -squareSize, 0, -squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( -squareSize, 0,  squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f(  squareSize, 0,  squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f(  squareSize, 0, -squareSize );
        }; glEnd();
    }; glPopMatrix();
}

//
//  void drawTexturedSprite()
//
//  Draws a textured quad in the XY plane
//
void drawTexturedSprite( float spriteXWidth, float spriteYWidth ) {
    glBegin( GL_QUADS ); {
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 0, 0.01 );
        glVertex3f( -(spriteXWidth/2.0f), spriteYWidth, 0 );
        
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 0, 1 );
        glVertex3f( -(spriteXWidth/2.0f),         0.0f, 0 );
        
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 1, 1 );
        glVertex3f( (spriteXWidth/2.0f),         0.0f, 0 );
        
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 1, 0.01 );
        glVertex3f( (spriteXWidth/2.0f), spriteYWidth, 0 );
    }; glEnd();
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
    
    gluLookAt(cameraXYZ.getX()+forestCenter.getX(), cameraXYZ.getY()+forestCenter.getY(), cameraXYZ.getZ()+forestCenter.getZ(),     // camera is located at (x,y,z) centered around forest
              forestCenter.getX(),                  forestCenter.getY(),                  forestCenter.getZ(),                      // camera is looking at forestCenter
              0.0f,                                 1.0f,                                 0.0f);                                    // up vector is (0,1,0) (positive Y)
    
    // clear the render buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // draw our textured ground quad
    drawGround();
    
    // enable textures
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, treeTexHandle );  // from here on out we draw trees so only bind this once to save context switching in OpenGL
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    
    // TODO #4: Sort trees by distance to camera
    // quicksort(0, treeSpritePositions.size()-1);
    sort(treeSpritePositions.begin(), treeSpritePositions.end(), mySort);
    // TODO #3: Compute rotation of tree to face camera
    Vector normal = Vector(0,0,1);
    // Vector dir = cameraXYZ-forestCenter;
    Vector dir = Vector(forestCenter.getX()-cameraXYZ.getX(), 0.0, forestCenter.getZ()-cameraXYZ.getZ());
    normal.normalize();
    dir.normalize();
    // Vector tar = forestCenter-normal;
    float rotation = acos(dot(dir,normal))*180/M_PI;
    Vector axis = cross(normal,dir);//vec4(dir.getX(), dir.getY(), dir.getZ(), 1);
    // cout << rotation << endl;
    
    for( unsigned int i = 0; i < treeSpritePositions.size(); i++ ) {
        //glRotatef(rotation, 0, axis.getY(), 0);
        glPushMatrix(); {
            //glRotatef(rotation, 0, axis.getY(), 0);
            // TODO #2: Draw our sprite!
            glTranslatef(treeSpritePositions.at(i).getX(),treeSpritePositions.at(i).getY(),treeSpritePositions.at(i).getZ());
            glRotatef(rotation, 0, axis.getY(), 0);
            drawTexturedSprite(20,20);
        }; glPopMatrix();
    }
    
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
    glutPostRedisplay();
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
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
    glutCreateWindow("Lab10: Billboarding");
    
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
    printf( "[INFO]: forest generated\n" );
    
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);

    ////// LOAD IN OUR TRANSPARENT TEXTURES //////////////////////////////////////////////////
    // some relevant variables for the texture files that we won't need later...so don't make them global
    int rgbWidth, rgbHeight, alphaWidth, alphaHeight, channels;
    unsigned char *treeTexData, *treeMaskData;

    treeTexData = SOIL_load_image( "textures/treeDiff.jpg",
                                  &rgbWidth, &rgbHeight, &channels,
                                  SOIL_LOAD_AUTO );
    printf( "[INFO]: read in image treeDiff.jpg of size %d x %d with %d colors\n", rgbWidth, rgbHeight, channels );
    treeMaskData = SOIL_load_image( "textures/treeOpa.jpg",
                                   &alphaWidth, &alphaHeight, &channels,
                                   SOIL_LOAD_AUTO );
    printf( "[INFO]: read in image treeOpa.jpg of size %d x %d with %d colors\n", alphaWidth, alphaHeight, channels );
    registerTransparentOpenGLTexture( treeTexData, treeMaskData, rgbWidth, rgbHeight, treeTexHandle );
    printf( "[INFO]: transparent texture read in and registered\n" );
    //////////////////////////////////////////////////////////////////////////////////////////
    
    groundTexHandle = SOIL_load_OGL_texture( "textures/ground.jpg",
                                            SOIL_LOAD_AUTO,
                                            SOIL_CREATE_NEW_ID,
                                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT );
    glBindTexture( GL_TEXTURE_2D, groundTexHandle );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // repeat our texture coordinates so our texture repeats over the ground
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // repeat our texture coordinates so our texture repeats over the ground
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    printf( "[INFO]: ground texture read in and registered\n" );
    
    //and enter the GLUT loop, never to exit.
    glutMainLoop();
    
    return(0);
}
