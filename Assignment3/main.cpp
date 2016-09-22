/*
 *  CSCI 441, Computer Graphics, Fall 2015
 *
 *  Project: Assignment 3
 *  File: main.cpp
 *
 *	Author: Peter Palumbo
 *
 *  Description:
 *     Drive a car around a city.
 *
 */

#ifdef __APPLE__			// if compiling on Mac OS
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else					// else compiling on Linux OS
	#include <GL/freeglut.h>
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
static float cameraStepValue = 0.5;
GLint leftMouseButton; 		   	    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

float cameraX, cameraY, cameraZ;            // camera position in cartesian coordinates
float cameraTheta, cameraPhi;               // camera DIRECTION in spherical coordinates
float dirX, dirY, dirZ;                     // camera DIRECTION in cartesian coordinates

float characterX, characterY, characterZ; //character position
float transportRotation = 0;
float wheelRotation = 0;
float fanRotate =0;
float cameraRadius = 5;

bool keysPressed[256];
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
				glVertex3f(x, 0, z);
				
				//side 2
				glVertex3f(x, 0, z + 1);
				glVertex3f(x + 1, 0, z + 1);
				glVertex3f(x+1, height, z + 1);
				glVertex3f(x, height, z + 1);
				glVertex3f(x, 0, z + 1);

				//side 3
				glVertex3f(x + 1, 0, z + 1);
				glVertex3f(x + 1, 0, z);
				glVertex3f(x + 1, height, z);
				glVertex3f(x + 1, height, z+1);
				glVertex3f(x + 1, 0, z + 1);

				//side 4
				glVertex3f(x + 1, 0, z);
				glVertex3f(x, 0, z);
				glVertex3f(x, height, z);
				glVertex3f(x + 1, height, z);
				glVertex3f(x + 1, 0, z);


				glEnd();

			}
		}

	}
}
//draw cyclinder for character
//void drawCylinder()
void drawCylinder() {
	float numSteps = 26.0f;
	float radius = 1.0f;
	float length = 10.0f;
	float hl = length * 0.5f;
	float count = 0.0f;
	float step = 3.14159*2 / numSteps;
	glPushMatrix(); {
		glColor3f(.7, .3, .3);
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i <= numSteps; i++) {
			float x = cos(count)*radius;
			float y = sin(count)*radius;
			glVertex3f(x, y, -hl);
			glVertex3f(x, y, hl);

			count += step;
		}
		glEnd();
	}; glPopMatrix();
}
//draw a fan on my cylinder
void drawFan() {
	glPushMatrix();{
		glColor3f(.3,.9,.1);
		glScalef(.075,.075,.075);
		glRotatef(fanRotate, 0,0,1);
		glBegin(GL_TRIANGLE_FAN);
		//first fan
		glVertex3f(0,0,0);
		glVertex3f(53,-53,0);
		glVertex3f(20,-72,0);
		//second fan
		glVertex3f(0,0,0);
		glVertex3f(-53,-53,0);
		glVertex3f(-72,-20,0);
		//third fan
		glVertex3f(0,0,0);
		glVertex3f(-53,53,0);
		glVertex3f(-20,72,0);
		//fourth fan
		glVertex3f(0,0,0);
		glVertex3f(53,53,0);
		glVertex3f(72,20,0);

		glEnd();

	};
	glPopMatrix();
}

//draw a copter for the back of my car
//void drawCopter()
void drawCopter(){
	glPushMatrix();{
		//glRotatef(90,1,0,0);
		glPushMatrix();{
			glRotatef(90,1,0,0);
			drawCylinder();
		};glPopMatrix();
		glPushMatrix();{
			glTranslatef(0,6,0);
			glRotatef(90,1,0,0);
			drawFan();
		};glPopMatrix();
	};glPopMatrix();
}
//void draw the wheels of my car
//void drawWheels()
void drawWheels(){
	glPushMatrix(); {
		//drawCylinder();
		
		glColor3f(255,0.0,0.0);
		glPushMatrix();{
			glTranslatef(0.5, 1, 0); //rotate the wheels around the axel
			glRotatef(wheelRotation, 0, 0, 1);
			glutSolidTorus(1, 2, 50, 20); 
			//DON'T THINK THE WHEEL IS MOVING!?!? ME NEITHER
			//SO I PUT THIS RANDOM ASS TRIANGLE IN HERE TO PROVE IT UNCOMMENT IF YOU WANT PROOF
			// glBegin(GL_TRIANGLES); //begin random triangle //UNCOMMENT
			// glPushMatrix();{ //UNCOMMENT
			// glColor3f(.7,.3,.3); //UNCOMMENT
			// 	glVertex3f(5, 2*2, 0); //UNCOMMENT
			// 	glVertex3f(10, 2*2, 5); //UNCOMMENT
			// 	glVertex3f(0, 2*2, 5); //UNCOMMENT
			// glEnd(); //end random triangle //UNCOMMENT
			// };glPopMatrix(); //UNCOMMENT
			glTranslatef(-0.5, -1, 0);
		};glPopMatrix();
		glPushMatrix();{
			glTranslatef(10,0,0);
			glTranslatef(0.5, 1, 0);
			glRotatef(wheelRotation, 0, 0, 1);
			glutSolidTorus(1, 2, 50, 20);
			glTranslatef(-0.5, -1, 0);
		};glPopMatrix();

		glPushMatrix();{
			glTranslatef(10,0,10);
			glTranslatef(0.5, 1, 0);
			glRotatef(wheelRotation, 0, 0, 1);
			glutSolidTorus(1, 2, 50, 20);
			glTranslatef(-0.5, -1, 0);
		};glPopMatrix();

		glPushMatrix();{
			glTranslatef(0,0,10);
			glTranslatef(0.5, 1, 0);
			glRotatef(wheelRotation, 0, 0, 1);
			glutSolidTorus(1, 2, 50, 20);
			glTranslatef(-0.5, -1, 0);
		};glPopMatrix();
		//glPushMatrix();
		
		//drawFan();
		//glPopMatrix();
	};glPopMatrix();
}

//draw the body of the car
//void draw body
void drawBody(){

	glPushMatrix();{
		glBegin(GL_QUADS);
				//glColor3f(getRand(), getRand(), getRand());
				GLfloat height = 2;
				GLfloat x = 0;
				GLfloat z = 0;
				GLfloat length =12;
				GLfloat width = 8;
				//bottom
				glVertex3f(x, 0, z);
				glVertex3f(x, 0, z + width);
				glVertex3f(x + length, 0, z + width);
				glVertex3f(x + length, 0, z);

				//top
				glVertex3f(x, height, z);
				glVertex3f(x, height, z + width);
				glVertex3f(x + length, height, z + width);
				glVertex3f(x + length, height, z);

				//side 1
				glVertex3f(x, 0, z);
				glVertex3f(x, 0, z + width);
				glVertex3f(x, height, z + width);
				glVertex3f(x, height, z);
				glVertex3f(x, 0, z);
				
				//side 2
				glVertex3f(x, 0, z + width);
				glVertex3f(x + length, 0, z + width);
				glVertex3f(x+length, height, z + width);
				glVertex3f(x, height, z + width);
				glVertex3f(x, 0, z + width);

				//side 3
				glVertex3f(x + length, 0, z + width);
				glVertex3f(x + length, 0, z);
				glVertex3f(x + length, height, z);
				glVertex3f(x + length, height, z+width);
				glVertex3f(x + length, 0, z + width);

				//side 4
				glVertex3f(x + length, 0, z);
				glVertex3f(x, 0, z);
				glVertex3f(x, height, z);
				glVertex3f(x + length, height, z);
				glVertex3f(x + length, 0, z);


			glEnd();
			// glBegin(GL_TRIANGLES);
			// glPushMatrix();{
			// glColor3f(.7,.3,.3);
			// 	glVertex3f(x/2, height*2, 0);
			// 	glVertex3f(x, height*2, z/2);
			// 	glVertex3f(0, height*2, z/2);
			// glEnd();
			// };glPopMatrix();
	};glPopMatrix();


}
//draw my character that will move around the world
//void drawCharacter()
void drawCharacter() {
	//glRotatef(transportRotation,0,1,0);
	glPushMatrix();
	
	glTranslatef(characterX,0,characterZ);
	//glTranslatef(0,0,-4);
	glRotatef(transportRotation, 0, 1, 0);
	glScalef(.15f,.15f,.15f);
	
	
	glColor3f(.7,.3,.7);
	glPushMatrix();{

		drawWheels();
		glPushMatrix();{
			glTranslatef(0,0,5);
			drawCylinder();
			glTranslatef(10,0,0);
			drawCylinder();
		};glPopMatrix();
	};glPopMatrix();

	glPushMatrix();{
		glColor3f(.3,.3,.7);
		glTranslatef(0,0,1);
		drawBody();
	};glPopMatrix();
	glPushMatrix();{
		glColor3f(.7,.3,.7);
		glTranslatef(10,6,4.5);
		drawCopter();
	};glPopMatrix();
	//glTranslatef(0,0,4);
	glPopMatrix();
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
	
	cameraX = sin(cameraTheta)*sin(cameraPhi)*cameraRadius;// + characterX;
	cameraY = -cos(cameraPhi)*cameraRadius;// +characterY;
	cameraZ = -cos(cameraTheta)*sin(cameraPhi)*cameraRadius;// + characterZ;
	
	float length = sqrt(pow(dirX, 2) + pow(dirY, 2) + pow(dirZ, 2));
	
	// dirX /= length;
	// dirY /= length;
	// dirZ /= length;

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
		float scale = 0.005;
    	float dx = (x-mouseX)*scale;
    	float dy = (y-mouseY)*scale;
		cameraTheta -= dx;
		cameraPhi -= dy;
		//if (cameraPhi < 0 || cameraPhi > M_PI) {
			//cameraPhi = .001;
		//}
		mouseX = x;
		mouseY = y;
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
    gluLookAt( cameraX+characterX, cameraY+characterY, cameraZ+characterZ,      // camera is located at (10,10,10)
    			//cameraX, cameraY, cameraZ,
                characterX,  characterY,  characterZ,   	// camera is looking at (0,0,0)
    			//characterX, characterY, characterZ,
                0,  1,  0);     // up vector is (0,1,0) (positive Y)

    /** TODO #2: REMOVE TEAPOT & CREATE A CITY SCENE ON A GRID...but call it's display list! **/
	glCallList(environmentDL);
	
	drawCharacter();
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

	if (key == 's' || key == 'S') {
		keysPressed['s'] = true;
	}
	if (key == 'w' || key == 'W') {
		keysPressed['w'] = true;
	}
	if (key =='d'||key=='D'){
		keysPressed['d']=true;
		//recomputeOrientation();

	}
	if (key =='a'||key=='A'){
		keysPressed['a']=true;
		//recomputeOrientation();
	}

	// if (key == 'wd' && key == 'd') {
	// 	transportRotation += 2;
	// 	if(characterX < 49 && characterX > -49 && characterZ < 49 && characterZ > -49){
	// 		characterZ += cameraStepValue*sin(transportRotation*M_PI/180);
	// 		characterX -= cameraStepValue*cos(transportRotation*M_PI/180);
	// 		cameraZ += cameraStepValue*sin(transportRotation*M_PI/180);
	// 		cameraX -= cameraStepValue*cos(transportRotation*M_PI/180);
	// 		transportRotation += 2;
	// 		wheelRotation += 1;
	// 	}
	// 	else{
	// 		characterZ -= cameraStepValue*sin(transportRotation);
	// 		characterX += cameraStepValue*cos(transportRotation);
	// 		cameraZ -= cameraStepValue*sin(transportRotation);
	// 		cameraX += cameraStepValue*cos(transportRotation);
	// 	}
	// }
    //glutPostRedisplay();		// redraw our scene from our new camera POV
}

//called when a key is realeased
//void normalKeyUp(unsigned char key, int x, int y)
void normalKeysUp(unsigned char key, int x, int y){
	if (key == 's' || key == 'S') {
		keysPressed['s'] = false;
	}
	if (key == 'w' || key == 'W') {
		keysPressed['w'] = false;
	}
	if (key =='d'||key=='D'){
		keysPressed['d']=false;
		//recomputeOrientation();

	}
	if (key =='a'||key=='A'){
		keysPressed['a']=false;
		//recomputeOrientation();
	}
}
void myTimer(int value) {

	if(keysPressed['s']){
		if(characterX < 49 && characterX > -49 && characterZ < 49 && characterZ > -49){
			characterZ -= cameraStepValue*sin(transportRotation*M_PI/180);
			characterX += cameraStepValue*cos(transportRotation*M_PI/180);
			// cameraZ -= cameraStepValue*sin(transportRotation*M_PI/180);
			// cameraX += cameraStepValue*cos(transportRotation*M_PI/180);
			wheelRotation -= 7;
		}
		// else{
		// 	characterZ += cameraStepValue*sin(transportRotation);
		// 	characterX -= cameraStepValue*cos(transportRotation);
		// 	cameraZ += cameraStepValue*sin(transportRotation);
		// 	cameraX -= cameraStepValue*cos(transportRotation);
		// }
	}
	if(keysPressed['w']){
		if(characterX < 49 && characterX > -49 && characterZ < 49 && characterZ > -49){
			characterZ += cameraStepValue*sin(transportRotation*M_PI/180);
			characterX -= cameraStepValue*cos(transportRotation*M_PI/180);
			// cameraZ += cameraStepValue*sin(transportRotation*M_PI/180);
			// cameraX -= cameraStepValue*cos(transportRotation*M_PI/180);
			wheelRotation += 7;
		}
		// else{
		// 	characterZ -= cameraStepValue*sin(transportRotation);
		// 	characterX += cameraStepValue*cos(transportRotation);
		// 	cameraZ -= cameraStepValue*sin(transportRotation);
		// 	cameraX += cameraStepValue*cos(transportRotation);
		// }
	}
	if(keysPressed['a']){
		transportRotation += 2;
		cameraTheta += 2*M_PI/180;
	}
	if(keysPressed['d']){
		transportRotation -= 2;
		cameraTheta -= 2*M_PI/180;
	}

	fanRotate += 5;
	glutPostRedisplay();
	glutTimerFunc(1000/60, myTimer, 0);
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
    cameraX = 40;
    cameraY = 15;
    cameraZ = 30;
    cameraTheta = -M_PI / 3.0f;
    cameraPhi = M_PI / 2.8f;
    characterX = 30;
    characterY = 5;
    characterZ = 30;
    recomputeOrientation();

    // register callback functions...
    glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
    glutKeyboardFunc(normalKeysDown);
    glutKeyboardUpFunc(normalKeysUp);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(1000/60, myTimer, 0);
    // do some basic OpenGL setup
    initScene();

    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
