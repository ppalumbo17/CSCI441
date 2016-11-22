/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: Lab08
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the implementation of a simple 3D environment, with 
 *  a user-controllable object that is followed by an arcball camera.
 *
 *  Author:
 *		Jeffrey Paone, Colorado School of Mines
 *  
 *  Notes:
 *
 */

// Very important!!!
// GLEW MUST BE INCLUDED BEFORE gl.h is included
#include <GL/glew.h>		// OpenGL Extension Wrangler

// Include our OpenGL & GLUT libraries
// GLUT automatically includes glu.h and gl.h
#ifdef __APPLE__
	#include <GLUT/glut.h>	// GLUT, GLU, GL Libraries for Mac
#else
	#include <GL/glut.h>	// GLUT, GLU, GL Libraries for Windows or *nix
#endif

#include <SOIL/SOIL.h>  	// Simple OpenGL Image Library

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
using namespace std;


// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

//global variables to keep track of window width and height.
//set to initial values for convenience, but we need variables
//for later on in case the window gets resized.
int windowWidth = 512, windowHeight = 512;

GLint leftMouseButton = GLUT_UP, rightMouseButton = GLUT_UP;    //status of the mouse buttons
int mouseX = 0, mouseY = 0;                 //last known X and Y of the mouse

float cameraTheta, cameraPhi, cameraRadius; //camera position in spherical coordinates
float cameraX, cameraY, cameraZ;            //camera position in cartesian coordinates
float objX, objY, objZ;                     //object position in world coordinates

float lPosition[4] = { 10, 10, 10, 1 };		//light position in world coordinates

enum LightType { POINT_LIGHT, DIRECTIONAL_LIGHT, SPOT_LIGHT };		// our types of lights
LightType lightType = POINT_LIGHT;				// current light type

// available objects to draw in our scene
enum OBJ_TYPE {
	CUBE,
	SPHERE,
	CONE,
	TORUS,
	TEAPOT,
	CYLINDER,
	DISK,
	PARDISK,
	DODECAHEDRON,
    OTHER
};

// our current object type
OBJ_TYPE currentObject = TEAPOT;

// display solid or wire objects
bool WIREFRAME = false;

// display GLU quadrics as a silhouette or not
bool SIL = false;


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
                            float &x, float &y, float &z) {
    x = radius * sinf(theta)*sinf(phi);
    y = radius * -cosf(phi);
    z = radius * -cosf(theta)*sinf(phi);
}
//
//  void drawAxes()
//      
//      Draw a set of unit vectors at the origin, showing the user
//      the positive and negative X, Y, and Z axes for perspective.
//
void drawAxes() {
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES); {
        glColor3f(1,0,0);
        glVertex3f(0,0,0); glVertex3f(1,0,0);
        glColor3f(0.5,0,0);
        glVertex3f(0,0,0); glVertex3f(-1,0,0);

        glColor3f(0,1,0);
        glVertex3f(0,0,0); glVertex3f(0,1,0);
        glColor3f(0,0.5,0);
        glVertex3f(0,0,0); glVertex3f(0,-1,0);

        glColor3f(0,0,1);
        glVertex3f(0,0,0); glVertex3f(0,0,1);
        glColor3f(0,0,0.5);
        glVertex3f(0,0,0); glVertex3f(0,0,-1);
    }; glEnd();

    glEnable(GL_LIGHTING);
}

//
//  void drawGrid()
//
//      Simple function to draw a grid in the X-Z plane using GL_LINES.
//
void drawGrid() {
    glDisable(GL_LIGHTING);

    glColor3f(1,1,1);
    glBegin(GL_LINES); {
        //draw the lines along the Z axis
        for(int i = 0; i < 11; i++) {
            glVertex3f((i-5), 0, -5);
            glVertex3f((i-5), 0, 5);
        }
        //draw the lines along the X axis
        for(int i = 0; i < 11; i++) {
            glVertex3f(-5, 0, (i-5));
            glVertex3f(5, 0, (i-5));
        }
    }; glEnd();

    glEnable(GL_LIGHTING);
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
//  void initScene()
//
//  A basic scene initialization function; should be called once after the
//      OpenGL context has been created. Doesn't need to be called further.
//      Just sets up a few function calls so that our main() is cleaner.
//
void initScene() {
	// enable depth testing 
    glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// enable light 0
    glEnable(GL_LIGHT0);

	// set the values for each component of lighting
    float diffuseLightCol[4] = { 1.0, 1.0, 1.0, 1 };	// white light
	float specularLightCol[4] = { 1.0, 1.0, 1.0, 1 };	// white light
    float ambientCol[4] = { 0.1, 0.1, 0.1, 1.0 };		// set this to be very low
	// and now set each value
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLightCol );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularLightCol );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
	
	// by default, OpenGL has a little bit of ambient light throughout the scene {0.2}
	// override this value to set the ambient scene light to black
	float black[4] = { 0.0, 0.0, 0.0, 1.0 };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, black );
	
    //tells OpenGL to blend colors across triangles. Once lighting is
    //enabled, this means that objects will appear smoother - if your object
    //is rounded or has a smooth surface, this is probably a good idea;
    //if your object has a blocky surface, you probably want to disable this.
    glShadeModel(GL_SMOOTH);
}

//
//  void renderScene()
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a teapot to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
void renderScene(void) {
	//clear the render buffer
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawBuffer( GL_BACK );

    //update the modelview matrix based on the camera's position
    glMatrixMode(GL_MODELVIEW);             //make sure we aren't changing the projection matrix!
    glLoadIdentity();

    gluLookAt(cameraX+objX, cameraY+objY, cameraZ+objZ,     //camera is located at (x,y,z)
                objX, objY, objZ,                           //camera is looking at (0,0,0)
                0.0f, 1.0f, 0.0f);                          //up vector is (0,1,0) (positive Y)
	
	// position our light
	glLightfv( GL_LIGHT0, GL_POSITION, lPosition );		
	
	// specify each of our material component colors
	GLfloat diffCol[4] = { 0.2, 0.3, 0.6 };					// a nice blue color for diffuse 
	GLfloat specCol[4] = { 1.0, 1.0, 0.0 };					// yellow specular highlights
	GLfloat ambCol[4] = { 0.8, 0.8, 0.8 };					// this value can be large because the final value
															// of our ambient light will be this * lightAmbient
	
	// and now set them for the front and back faces
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffCol );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specCol );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 96.0 );		// as well as the shininess - this value ranges between 0 & 90
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambCol );

    // TODO #3: Apply our brick texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix(); {
        glTranslatef(objX, objY, objZ);
        
		// draw our current object as solid or wire as specified
		GLUquadricObj *myQuad = gluNewQuadric();
		switch( currentObject ) {
			case CUBE:
				if( WIREFRAME )
					glutWireCube( 2 );
				else
					glutSolidCube( 2 );
				break;
				
			case SPHERE:
				if( WIREFRAME )
					glutWireSphere( 1.5, 10, 10 );
				else  
					glutSolidSphere( 1.5, 10, 10 );
				break;
				
			case CONE:
				if( WIREFRAME )
					glutWireCone( 1.5, 3, 10, 10 );
				else 
					glutSolidCone( 1.5, 3, 10, 10 );
				break;
				
			case TORUS:
				if( WIREFRAME )
					glutWireTorus( 0.5, 1, 10, 10 );
				else  
					glutSolidTorus( 0.5, 1, 10, 10 );
				break;
				
			case TEAPOT:
				if( WIREFRAME ) {
					glutWireTeapot( 1.5 );
				} else {
					// there is a known bug with the POINT_LIGHTs being specified in reverse order for the GLUT Teapot
					glFrontFace( GL_CW );
					glutSolidTeapot( 1.5 );
					glFrontFace( GL_CCW );
				}
				break;
				
			case CYLINDER:
				if( WIREFRAME )
					gluQuadricDrawStyle( myQuad, GLU_LINE );
				else if( SIL )
					gluQuadricDrawStyle( myQuad, GLU_SILHOUETTE );
				else 
					gluQuadricDrawStyle( myQuad, GLU_FILL );
				gluCylinder( myQuad, 1, 1, 1, 10, 10 );
				break;
				
			case DISK:
				if( WIREFRAME )
					gluQuadricDrawStyle( myQuad, GLU_LINE );
				else if( SIL )
					gluQuadricDrawStyle( myQuad, GLU_SILHOUETTE );
				else 
					gluQuadricDrawStyle( myQuad, GLU_FILL );
				gluDisk( myQuad, 0.5, 1, 10, 10 );
				break;
				
			case PARDISK:
				if( WIREFRAME )
					gluQuadricDrawStyle( myQuad, GLU_LINE );
				else if( SIL )
					gluQuadricDrawStyle( myQuad, GLU_SILHOUETTE );
				else 
					gluQuadricDrawStyle( myQuad, GLU_FILL );
				gluPartialDisk( myQuad, 0.5, 1, 10, 10, 0, 120 );
				break;
				
			case DODECAHEDRON:
				if( WIREFRAME )
					glutWireDodecahedron();
				else  
					glutSolidDodecahedron();
				break;
                
            case OTHER:
                // TODO #4: make a quad
				glPushMatrix(); {
					glBegin(GL_QUADS); {              
						glNormal3f(0, 1, 0);
						glTexCoord2f(2, 0);
						glVertex3f(0, 0, 0);
						glNormal3f(0, 1, 0);
						glTexCoord2f(2, 2);
						glVertex3f(0, 0, 2);
						glNormal3f(0, 1, 0);
						glTexCoord2f(0, 2);
						glVertex3f(2, 0, 2);
						glNormal3f(0, 1, 0);
						glTexCoord2f(0, 0);
						glVertex3f(2, 0, 0);
					}; glEnd();
				}; glPopMatrix();
                break;
		};
		gluDeleteQuadric( myQuad );
	
    }; glPopMatrix();

    //scale the axes up so they stick through the dodecahedron
    glPushMatrix();
        glScalef(4,4,4);
        drawAxes();
    glPopMatrix();

    //move the grid down a touch so that it doesn't overlap with the axes
    glPushMatrix();
        glTranslatef(0,-1,0);
        drawGrid();
    glPopMatrix();

    //push the back buffer to the screen
    glutSwapBuffers();
}


//
//  void normalKeys(unsigned char key, int x, int y)
//
//      GLUT keyboard callback for regular characters.
//
void normalKeys(unsigned char key, int x, int y) {
    //kindly quit if the user requests!
    if(key == 'q' || key == 'Q')
        exit(0);
	
	switch( key ) {
		case 'd':
		case 'D':
			objX += 0.1f;
			break;
			
		case 'a':
		case 'A':
			objX -= 0.1f;
			break;
			
		case 'w':
		case 'W':
			objZ -= 0.1f;
			break;
			
		case 's':
		case 'S':
			objZ += 0.1f;
			break;
			
		case '1':	currentObject = CUBE;			break;
		case '2':	currentObject = SPHERE;			break;
		case '3':	currentObject = CONE;			break;
		case '4':	currentObject = TORUS;			break;
		case '5':	currentObject = TEAPOT;			break;
		case '6':	currentObject = CYLINDER;		break;
		case '7':	currentObject = DISK;			break;
		case '8':	currentObject = PARDISK;		break;
		case '9':	currentObject = DODECAHEDRON;	break;
        case '0':   currentObject = OTHER;          break;
			
		case ' ':	WIREFRAME = !WIREFRAME;			break;
		case 'c':	SIL = !SIL;						break;
			
		case 'l':
		case 'L':
			// change our lighting type
			if( lightType == POINT_LIGHT ) {		// if a POINT_LIGHT light
				// change to DIRECTIONAL_LIGHT light
				lightType = DIRECTIONAL_LIGHT;
				
				// set the direction of the light
				lPosition[0] = 3.0;
				lPosition[1] = -4.0;
				lPosition[2] = -5.0;
				lPosition[3] = 0.0;	// note this is 0
				
				// we mostly need to set the pos[3] "w" value to 0.0 to signify it is a vector
				// the position will be set in the renderScene() method
			} else if( lightType == DIRECTIONAL_LIGHT ) {	// if a DIRECTIONAL_LIGHT light
				// change to SPOT_LIGHT light
				lightType = SPOT_LIGHT;
				
				// set the position of the light
				lPosition[0] = -4.0;
				lPosition[1] = 2.0;
				lPosition[2] = -3.0;
				lPosition[3] = 1.0;	// note this is 1
				// the position will be set in the renderScene() method
				
				// set the direction of the light
				GLfloat SPOT_LIGHTDirection[4] = { 4.0, -2.0, 3.0, 0.0 };
				glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, SPOT_LIGHTDirection );
				
				// set the angle of our cone in degrees
				glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 30 );	// the cone will be double this value so 60 degrees
															// acceptable values are between 0 and 90
															// or the special value of 180
				// set the exponent of our falloff
				glLightf( GL_LIGHT0, GL_SPOT_EXPONENT, 30 );// acceptable values are between 0 and 128
			} else if( lightType == SPOT_LIGHT ) {	// if a SPOT_LIGHT light
				// change to POINT_LIGHT light
				lightType = POINT_LIGHT;
				
				// set the position of the light
				lPosition[0] = 10.0;
				lPosition[1] = 10.0;
				lPosition[2] = 10.0;
				lPosition[3] = 1.0;	// note this is 1
				
				// return the angle of our cone back to 180 to signify back to POINT_LIGHT light
				// 180 is the default value
				glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 180 );
				// return the falloff to 0 which is the default value
				glLightf( GL_LIGHT0, GL_SPOT_EXPONENT, 0 );
			}
			break;
	}
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
    if(button == GLUT_LEFT_BUTTON) {
        leftMouseButton = state;
    } else if(button == GLUT_RIGHT_BUTTON) {
        rightMouseButton = state;
	}
    
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
void mouseMotion(int x, int y)
{
    //mouse is moving; if left button is held down...
    if(leftMouseButton == GLUT_DOWN ) {
		
		//update theta and phi! 
		cameraTheta += (x - mouseX)*0.005;
		cameraPhi   += (y - mouseY)*0.005;
		
		// we don't care if theta goes out of bounds; it'll just loop around.
		// phi, though.. it'll flip out if it flips over top. keep it in (0, M_PI)
		if(cameraPhi <= 0)
			cameraPhi = 0+0.001;
		if(cameraPhi >= M_PI)
			cameraPhi = M_PI-0.001;
        
		//update camera (x,y,z) based on (radius,theta,phi)
		computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
							   cameraX, cameraY, cameraZ);
	}
	else if( rightMouseButton == GLUT_DOWN ) {
		//for the right mouse button, just determine how much the mouse has moved total.
		//not the best "zoom" behavior -- if X change is positive and Y change is negative,
		//(along the X = Y line), nothing will happen!! but it's ok if you zoom by
		//moving left<-->right or up<-->down, which works for most people i think.
		double totalChangeSq = (x - mouseX) + (y - mouseY);
		cameraRadius += totalChangeSq*0.01;
		
		//limit the camera radius to some reasonable values so the user can't get lost
		if(cameraRadius < 2.0) 
			cameraRadius = 2.0;
		if(cameraRadius > 50.0) 
			cameraRadius = 50.0;
		
		//update camera (x,y,z) based on (radius,theta,phi)
		computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
							   cameraX, cameraY, cameraZ);
	}
	
    //and save the current mouseX and mouseY.
    mouseX = x;
    mouseY = y;
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
//  PARAMETERS:
//	string filename         :   input	: filename of image to load
//	int imageWidth          :   output	: this parameter will have the image width
//                                        set when function returns
//	int imageHeight         :   output	: this parameter will have the image height
//                                        set when function returns
//	unsigned char* imageData :  output  : this parameter will have the RGB data
//                                        set when function returns
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

// registerOpenGLTexture() /////////////////////////////////////////////////////
//
//  This function registers an OpenGL texture and sets the appropriate texture
//	properties.
//
//  PARAMETERS:
//	unsigned char *textureData :	input :	this contains the raw RGB data to
//                                          of the image
//	unsigned int texWidth	   :    input : the width of our image as reported
//                                          by readPPM()
//	unsigned int texHeight	   :    input : the height of our image as reported
//                                          by readPPM()
//	GLuint textureHandle	   :   output : the handle to our texture, will be
//                                          set when function returns
////////////////////////////////////////////////////////////////////////////////
bool registerOpenGLTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {
    // TODO #2b: Fill this in
	glEnable(GL_TEXTURE_2D);
	//glGenTextures(texHeight, textureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return true;
}


//
//  int main(int argc, char **argv)
//
//      No offense but you should probably know what the main function does.
//      argc is the number of arguments to the program (program name included),
//      and argv is an array of strings; argv[i] is the ith command line
//      argument, where 0 <= i <= argc
//
int main(int argc, char **argv) {
    //create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("Lab08: Texturing");

    //give the camera a 'pretty' starting POINT_LIGHT!
    cameraRadius = 12.0f;
    cameraTheta = 2.80;
    cameraPhi = 2.0;

    objX = objY = objZ = 0.0f;
	
    computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
						   cameraX, cameraY, cameraZ);
	

    //register callback functions...
    glutKeyboardFunc(normalKeys);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
	glutMouseFunc( mouseCallback );
	glutMotionFunc( mouseMotion );

    //do some basic OpenGL setup to initialize our lights
    initScene();
    
    // TODO #1: Read in PPM
	
	unsigned char* brickArray;
	int imageWidth;
	int imageHeight;
	unsigned int err = 0;
	readPPM("./textures/brick.ppm", imageWidth, imageHeight, brickArray);
    // TODO #2a: Register PPM Texture
	registerOpenGLTexture(brickArray, imageWidth, imageHeight, err);
    
    // TODO #5: Read in non-PPM
	GLuint texHandle =
		SOIL_load_OGL_texture(
			"./textures/mines.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			| SOIL_FLAG_INVERT_Y
			| SOIL_FLAG_COMPRESS_TO_DXT
		);
    // TODO #6: Register non-PPM
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, texHandle);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// register our timer function
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);

    //and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
