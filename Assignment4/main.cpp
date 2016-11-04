/*
 *  CSCI 441, Computer Graphics, Fall 2015
 *
 *  Project: Assignment 4
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
	//#include <GL/freeglut.h>
	#include <gl/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <GL/glui.h>			// include our GLUI header

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


// C++ Libraries we'll use
#include <fstream>			// we'll use ifstream	
#include <string>			// for, well strings!
#include <vector>			// and vectors (the storage container, not directional)
#include <sstream>
#include <iostream>

// Headers We've Written
#include "Point.h"

using namespace std;
// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

static size_t windowWidth  = 640;
static size_t windowHeight = 480;
static float aspectRatio;
static float cameraStepValue = 0.5;
GLint leftMouseButton, righMouseButton; 		   	    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

float cameraX, cameraY, cameraZ;            // camera position in cartesian coordinates
float cameraTheta, cameraPhi;               // camera DIRECTION in spherical coordinates
float dirX, dirY, dirZ;                     // camera DIRECTION in cartesian coordinates

float characterX, characterY, characterZ; //character position
float transportRotation = 0;
float wheelRotation = 0;
float fanRotate =0;
float cameraRadius = 5;


GLint menuId;				    			// handle for our menu

float sphereRadius = 0.5f; //the radius of the control points
float lineThickness = 3.0f;  //variable for line thickness between points

vector<Point> controlPoints;
float trackPointVal = 0.0f;
float timerValue = 0.0f; //timer value to control pixie

bool DRAW_CAGE = true;  //Menu option for drawing the bezier points
bool DRAW_CURVE = true;  //Menu option for drawing the bezier curve
bool CTRL_PRESSED = false;
bool WHICH_POINTS = true;

Point onBez;

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

///DRAWING HERO STARTS HERE///

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
// drawSolidSphere(float x,float y,float z)//
// draw a solid sphere at a location x, y, z
void drawSolidSphere(){
    glPushMatrix();{
        glColor4f(.2, .2, .8, .4);
        glutSolidSphere(sphereRadius, 10, 10);
    };glPopMatrix();
}

//drawSipit(float x, float y, float z)
//draw the spirit for the bezier curve at x,y,z
void drawSpirit(float inX, float inY, float inZ){
	glPushMatrix();{
		
        glTranslatef(inX, inY, inZ);
        // glRotatef(-fanRotate, 0,1,1);
        glPushMatrix();{
        	glScalef(2,2,2);
        	drawSolidSphere();
        };glPopMatrix();
        //One Sphere
        glPushMatrix();{
        	glRotatef(-fanRotate, 0,1,1);
        	glTranslatef(1,1,0);
        	drawSolidSphere();
        };glPopMatrix();
        //Two Sphere
        glPushMatrix();{
        	glRotatef(-fanRotate, 1,1,0);
        	glTranslatef(0,1,1);
        	drawSolidSphere();
        };glPopMatrix();
        //Three Sphere
        glPushMatrix();{
        	glRotatef(-fanRotate, 1,0,1);
        	glTranslatef(1,-1,0);
        	drawSolidSphere();
        };glPopMatrix();
        //Four Sphere
        glPushMatrix();{
        	glRotatef(-fanRotate, 1,-1,0);
        	glTranslatef(0,-1,1);
        	drawSolidSphere();
        };glPopMatrix();
        // //Five Sphere
        // glPushMatrix();{
        // 	// glRotatef(-fanRotate, 0,1,-1);
        // 	glTranslatef(1,1,1);
        // 	drawSolidSphere();
        // };glPopMatrix();
        // //Six Sphere
        // glPushMatrix();{
        // 	// glRotatef(-fanRotate, -1,1,0);
        // 	glTranslatef(-1,-1,-1);
        // 	drawSolidSphere();
        // };glPopMatrix();
        // //Seven Sphere
        // glPushMatrix();{
        // 	// glRotatef(-fanRotate, 0,-1,1);
        // 	glTranslatef(1,-1,-1);
        // 	drawSolidSphere();
        // };glPopMatrix();
        // //Eight Sphere
        // glPushMatrix();{
        // 	//glRotatef(-fanRotate, 1,-1,0);
        // 	glTranslatef(-1,-1,1);
        // 	drawSolidSphere();
        // };glPopMatrix();
	};glPopMatrix();
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

//STUFF FROM LAB03

// evaluateBezierCurve() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier Curve. 
//
////////////////////////////////////////////////////////////////////////////////
Point evaluateBezierCurve( Point p0, Point p1, Point p2, Point p3, float t ) {
    // TODO #08: Compute a point along a Bezier curve
	float bezX = pow(1-t, 3)*p0.getX() + 3*pow(1-t, 2)*t*p1.getX() + 3*(1-t)*pow(t, 2)*p2.getX() + pow(t, 3)*p3.getX();
    float bezY = pow(1-t, 3)*p0.getY() + 3*pow(1-t, 2)*t*p1.getY() + 3*(1-t)*pow(t, 2)*p2.getY() + pow(t, 3)*p3.getY();
    float bezZ = pow(1-t, 3)*p0.getZ() + 3*pow(1-t, 2)*t*p1.getZ() + 3*(1-t)*pow(t, 2)*p2.getZ() + pow(t, 3)*p3.getZ();

    return Point(bezX, bezY, bezZ);
}

// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution. 
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve( Point p0, Point p1, Point p2, Point p3, int resolution ) {
    // TODO #07: Draw a Bezier curve
    glPushMatrix();
    glLineWidth(lineThickness);
    glColor3f(0,0,1);
    glBegin(GL_LINE_STRIP);
    for(float i = 0; i < resolution; i+=.4){
        float time = float(i) / float(resolution);
        Point eval = evaluateBezierCurve(p0, p1, p2, p3, time);
        glVertex3f(eval.getX(),eval.getY(),eval.getZ());
        //drawSolidSphere(eval.getX(),eval.getY(),eval.getZ());
    }
    glEnd();
    glPopMatrix();
}
// drawSphere(float x,float y,float z)//
// draw a sphere at a location x, y, z
void drawSphere(float inX, float inY, float inZ){
    glPushMatrix();{
        glColor3f(.2, .8, .2);
        glTranslatef(inX, inY, inZ);
        glutWireSphere(sphereRadius, 10, 10);
    };glPopMatrix();
}

// myMenu() /////////////////////////////////////////////////////////////////////
//
//  Handles our Menu events
//
////////////////////////////////////////////////////////////////////////////////
void myMenu( int value ) {
	// TODO #02: handle our menu options
    switch(value){
        case 0:
            exit(0);
            break;
        case 1:
        	DRAW_CAGE = !(DRAW_CAGE);
        	break;
        case 2:
        	DRAW_CURVE = !(DRAW_CURVE);
        	glutPostRedisplay();
        	break;
    }

}

// createMenus() ///////////////////////////////////////////////////////////////
//
//  Handles creating a menu, adding menu entries, and attaching the menu to
//  a mouse button
//
////////////////////////////////////////////////////////////////////////////////
void createMenus() {
	// TODO #01: Create a Simple Menu
    glutCreateMenu( myMenu );
    glutAddMenuEntry("Quit",0);
    glutAddMenuEntry("Display/Hide Control Cage", 1);
    glutAddMenuEntry("Display/Hide Bezier Curve", 2);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);

}
// loadControlPoints() /////////////////////////////////////////////////////////
//
//  Load our control points from file and store them in a global variable.
//
////////////////////////////////////////////////////////////////////////////////
bool loadControlPoints( char* filename ) {
	// TODO #04: read in control points from file.  Make sure the file can be
	// opened and handle it appropriately.
    ifstream file(filename);
    if(!file.good()){
        return false;
    }
    string numPoints;
    getline(file, numPoints);
    if(file.is_open()){
        while(file.good()){
            string new_Line;
            getline(file, new_Line);
            stringstream lineStream(new_Line);
            string point_Member;
            vector<double> input_Points;
            while(getline(lineStream, point_Member, ',')){
                input_Points.push_back(atof(point_Member.c_str()));
            }
            //cout << "Error is here" << endl;
            if(file.good()){
                Point new_Point(input_Points.at(0),input_Points.at(1),input_Points.at(2));
                controlPoints.push_back(new_Point);
            }
        }
    }
    file.close();
	return true;
}

//drawControlPoints()
//draw the bezier curve and it's control points
void drawControlPoints(){
	// TODO #05: Draw our control points
    for(int i = 0; i < controlPoints.size(); i++){
    	if(DRAW_CAGE){
	        glPushMatrix();{
	            drawSphere(controlPoints.at(i).getX(),controlPoints.at(i).getY(),controlPoints.at(i).getZ());
	        };glPopMatrix();
	        if(i < controlPoints.size()-1){
	            glPushMatrix();{
	                glLineWidth(lineThickness);
	                glColor3f(1,1,0);
	                glBegin(GL_LINE_STRIP);
	                glVertex3f(controlPoints.at(i).getX(),controlPoints.at(i).getY(),controlPoints.at(i).getZ());
	                glVertex3f(controlPoints.at(i+1).getX(),controlPoints.at(i+1).getY(),controlPoints.at(i+1).getZ());
	                glEnd();
	            };glPopMatrix();
	        }
    	}
        if(i < controlPoints.size()-3 && (i%3==0) && DRAW_CURVE){
            renderBezierCurve(controlPoints.at(i), controlPoints.at(i+1), controlPoints.at(i+2), controlPoints.at(i+3), 200);
        }
    }
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
    if(glutGetModifiers()==GLUT_ACTIVE_CTRL){
    	CTRL_PRESSED = true;
    }    
    else{
    	CTRL_PRESSED = false;
    }
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
	if((leftMouseButton == GLUT_DOWN )&& CTRL_PRESSED){//(glutGetModifiers()==GLUT_ACTIVE_CTRL)){
		float scale = .05;
		float dy = (y-mouseY)*scale;
		cameraRadius += dy;
		recomputeOrientation();     // update camera (x,y,z) based on (radius,theta,phi)
		

		glutPostRedisplay();
	}
    else if(leftMouseButton == GLUT_DOWN && !CTRL_PRESSED){//(glutGetModifiers()==GLUT_ACTIVE_CTRL)) {
		float scale = 0.005;
    	float dx = (x-mouseX)*scale;
    	float dy = (y-mouseY)*scale;
		cameraTheta -= dx;
		
		if (cameraPhi < 0 || cameraPhi > M_PI) {
			cameraPhi += dy;
		}
		else{
			cameraPhi -= dy;
		}
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
	
	glPushMatrix();{
		glTranslatef(characterX+sin(transportRotation*M_PI/180),characterY-2,characterZ+cos(transportRotation*M_PI/180));
		glRotatef(transportRotation, 0, 1, 0);
		glScalef(.25,.25,.25);
		drawControlPoints();
		glPushMatrix();{
			drawSpirit(onBez.getX(),onBez.getY(),onBez.getZ());

		};glPopMatrix();
	};glPopMatrix();
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
	// if(glutGetModifiers()==GLUT_ACTIVE_CTRL){
	// 	CTRL_PRESSED = !(CTRL_PRESSED);
	// }

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
	// if(glutGetModifiers()!=GLUT_ACTIVE_CTRL){
	// 	CTRL_PRESSED = !(CTRL_PRESSED);
	// }
}
void myTimer(int value) {

	if(timerValue>=1.0){
		timerValue =0;
		WHICH_POINTS = !(WHICH_POINTS);
	}
	if(WHICH_POINTS){
		onBez = evaluateBezierCurve(controlPoints.at(0), controlPoints.at(1), controlPoints.at(2), controlPoints.at(3), timerValue);
		// glPushMatrix();{
		// 	glTranslatef(characterX+sin(transportRotation*M_PI/180),characterY-2,characterZ+cos(transportRotation*M_PI/180));
		// 	glRotatef(transportRotation, 0, 1, 0);
		// 	glScalef(.25,.25,.25);
		// 	drawSolidSphere(onBez.getX(),onBez.getY(),onBez.getZ());
		// 	};glPopMatrix();
	}
	else{
		onBez = evaluateBezierCurve(controlPoints.at(3), controlPoints.at(4), controlPoints.at(5), controlPoints.at(6), timerValue);
		// glPushMatrix();{
		// 	glTranslatef(characterX+sin(transportRotation*M_PI/180),characterY-2,characterZ+cos(transportRotation*M_PI/180));
		// 	glRotatef(transportRotation, 0, 1, 0);
		// 	glScalef(.25,.25,.25);
		// 	drawSolidSphere(onBez.getX(),onBez.getY(),onBez.getZ());
		// 	};glPopMatrix();
	}
	
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

	timerValue += .01;
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

	bool file_Good = loadControlPoints(argv[1]);
    if(!file_Good){
        cout << "Your file input was not good, please try again!" << endl;
        exit(-1);
    }
    // create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("Cyclopalypse Now Asssignment 4");

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

    //create menus
    createMenus();
    // do some basic OpenGL setup
    initScene();

    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
