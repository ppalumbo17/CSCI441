/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: lab01
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2016
 *
 *  Description:
 *      Contains the code for a simple 2D OpenGL / GLUT example.  
 *  Animation, keyboard, and mouse interaction will be added to 
 *  this very basic example.
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
#include <math.h>

//global variables
int triforceAngle = 20;
int armAngle = 0;
bool armUp = true;
bool STOP_WAVE = false;
bool EVIL_TRIFORCE = false;
bool DRAW_SHURIKEN = false;
int myHeroX = 100;
int myHeroY = 0;
double heroRightArmX = 190;
double heroRightArmY = 100;
int myMouseX = 100;
int myMouseY = 100;
int shurikenRotation = 20;
double shurikenLocationX = 190;
double shurikenLocationY = 100;
double shurikenAngleX = 0;
double shurikenAngleY = 0;
double mouseLocationX = 100;
double mouseLocationY = 100;
// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 512, windowHeight = 512;

// global variable to keep track of the window id
int windowId;

const float DEG2RAD = 3.14159 / 180; //turn degrees into radians
//
//  void changeSize(int w, int h)
//
//      We will register this function as GLUT's reshape callback.
//   When the window is resized, the OS will tell GLUT and GLUT will tell
//   us by calling this function - GLUT will give us the new window width
//   and height as 'w' and 'h,' respectively - so save those to our global vars.
//
void changeSize( int w, int h ) {
    //save the new window width / height that GLUT informs us about
    windowWidth = w;
    windowHeight = h;

    // update the projection matrix based on the window size
    // the GL_PROJECTION matrix governs properties of the view;
    // i.e. what gets seen - use an Orthographic projection that ranges
    // from [0, windowWidth] in X and [0, windowHeight] in Y. (0,0) is the lower left.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, windowWidth, 0, windowHeight );
    
    // update the viewport as well - tell OpenGL we want to render the whole window
    glViewport( 0, 0, windowWidth, windowHeight );

    // just as good practice, switch back to GL_MODELVIEW mode; that's the matrix
    // that we want to be changing with glTranslatef(), glScalef(), etc., so
    // just in case we forget to explicitly set it later...
    glMatrixMode( GL_MODELVIEW );
}

//
//  void drawTriangle()
//
//		Issues a series of OpenGL commands to draw a triangle
//
/**void drawTriangle() {
	// push the current matrix before doing anything;
	// just a good, safe practice
	glPushMatrix(); {
		// tell OpenGL we want to draw triangles
		glBegin( GL_TRIANGLES ); {
			glVertex2f( -2.5, -2 );		// lower left corner
			glVertex2f(  2.5, -2 );		// lower right corner
			glVertex2f(  0.0f,  2.0f );	// top corner
		}; glEnd();	// tell OpenGL we are done drawing triangles
	}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
}**/

//
//  void drawTriforce()
//
//      Issues a series of OpenGL commands to draw three triangles in a
//  triangle shape that all fit inside a larger triangle.
//
/**void drawTriforce() {
	// push the current matrix before doing anything;
	// just a good, safe practice
	glPushMatrix(); {
	
		// make all the vertices be a nice, golden color.
		glColor3f( 0.9, 0.8, 0.1 );

		if (EVIL_TRIFORCE) {
			glColor3f(0.9, 0.1, 0.1);
		}
		// push the current matrix before doing anything;
		// just a good, safe practice		
		glPushMatrix(); {
			
			// the triangle will get rendered around the origin,
			// so move the origin to (-25, -20)
			glTranslatef( -2.50f, -2.00f, 0.0f );
			
			// scale the triangle by 10 in x and 10 in y
			//glScalef( 10.0f, 10.0f, 1.0f );
			
			// draw the triangle
			drawTriangle();
		}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
		
		// push the current matrix before doing anything;
		// just a good, safe practice		
		glPushMatrix(); {
			// the triangle will get rendered around the origin,
			// so move the origin to (25, -20)
			glTranslatef( 2.50f, -2.00f, 0.0f );
			
			// scale the triangle by 10 in x and 10 in y
			//glScalef( 10.0f, 10.0f, 1.0f );
			
			// draw the triangle
			drawTriangle();
		}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
		
		// push the current matrix before doing anything;
		// just a good, safe practice
		glPushMatrix(); {
			// the triangle will get rendered around the origin,
			// so move the origin to (0, 20)
			glTranslatef( 0.0f, 2.00f, 0.0f );
			
			// scale the triangle by 10 in x and 10 in y
			//glScalef( 10.0f, 10.0f, 1.0f );
			
			// draw the triangle
			drawTriangle();
		}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
		
	}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
}
**/

//void drawCircle()
//draw a perfect circle, i hope
void drawCircle(float radius)
{
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(radius);
	glBegin(GL_LINE_LOOP);
	//glBegin(GL_LINES);
	
	for (int i = 0; i<360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex2f(cos(degInRad)*radius, sin(degInRad)*radius);
	}

	glEnd();
}

//void drawFilledCircle()
//draw a filled in circle rather than loop
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius) {
	int num_triangles = 40;

	GLfloat twoPi = 2.0f * 3.14159;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int i = 0; i <= num_triangles; i++) {
		glVertex2f(
			x + (radius * cos(i *  twoPi / num_triangles)),
			y + (radius * sin(i * twoPi / num_triangles))
		);
	}
	glEnd();
}

//void drawSmallRect()
//draw a connecting rectangle mainly for horizontal use
void drawSmallRect() {
	glPushMatrix();
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(0, 10);
	glVertex2f(20, 10);
	glVertex2f(20, 0);
	glEnd();
	glPopMatrix();
}

//void drawEmptyRect()
//draw an Empty rectangle not filled in
void drawEmptyRect() {
	glPushMatrix();
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(0, 10);
	glVertex2f(20, 10);
	glVertex2f(20, 0);
	glEnd();
	glPopMatrix();
}

void drawSmallTriangle() {
	glPushMatrix();
	//glScalef(.5, .5, 0);
	glColor3f(.749, .247, .247);
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(25, 0);
	glVertex2f(12.5, 20);
	glEnd();
	glPopMatrix();
}



// void drawHead()
//draw the head of my cyclops
void drawHead() {
	//scale by twice the size because for some reason it was below even though I didn't want it there
	//glScalef(10, 10, 1);
	//draw main head
	glPushMatrix();
	glColor3f(.894, .678, .467);
	drawFilledCircle(0, 0, 25);
	glPopMatrix();
	//draw mouth
	glPushMatrix();
	//translate down to mouth position
	glTranslatef(-5, -15, 0);
	//scale by a quarter
	glScalef(.5, .5, 1);
	glColor3f(.894, .467, .467);
	drawSmallRect();
	glPopMatrix();
	//offcenter eye
	glTranslatef(0, 5, 0);
	//draw eye background
	glPushMatrix();
	glColor3f(1, 1, 1);
	drawFilledCircle(0, 0, 10);
	glPopMatrix();
	//draw pupil
	glPushMatrix();
	glColor3f(0, .00, .0);
	drawFilledCircle(0, 0, 5);
	glPopMatrix();
	
}
//void drawUpperBody()
//draw a very dull upper body of my hero
void drawUpperBody() {
	glPushMatrix();
	//draw main body
	glPushMatrix();
	glColor3f(.894, .678, .467);
	glBegin(GL_QUADS);
	glVertex2f(10, 0);
	glVertex2f(0, 100);
	glVertex2f(90, 100);
	glVertex2f(80, 0);
	glEnd();
	glPopMatrix();

	//draw left peck
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(10, 70, 0);
	glScalef(1.25, .25, 0);
	drawSmallRect();
	glPopMatrix();
	//draw right peck
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(55, 70, 0);
	glScalef(1.25, .25, 0);
	drawSmallRect();
	glPopMatrix();
	//draw six pack
	glPushMatrix();
	//glTranslatef(-3, 0, 0);
	glColor3f(0, 0, 0);
	//topleft
	glPushMatrix(); {
		glTranslatef(22, 40, 0);
		drawEmptyRect();
	}glPopMatrix();
	//topright
	glPushMatrix(); {
		glTranslatef(47, 40, 0);
		drawEmptyRect();
	}glPopMatrix();
	//middle left
	glPushMatrix(); {
		glTranslatef(22, 25, 0);
		drawEmptyRect();
	}glPopMatrix();
	//middle right
	glPushMatrix(); {
		glTranslatef(47, 25, 0);
		drawEmptyRect();
	}glPopMatrix();
	//bottom left
	glPushMatrix(); {
		glTranslatef(22, 10, 0);
		drawEmptyRect();
	}glPopMatrix();
	//bottom right
	glPushMatrix(); {
		glTranslatef(47, 10, 0);
		drawEmptyRect();
	}glPopMatrix();

	glPopMatrix();

	glPopMatrix();


}

//void drawLeftLeg()
//draw the leg of my hero
void drawLeftLeg() {
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(.631, .121, .121);
	glVertex2f(5, 0);
	glVertex2f(0, 100);
	glVertex2f(35, 100);
	glVertex2f(25, 0);
	glEnd();
	glPopMatrix();
}

//void drawRightLeg()
//draw the right leg of my hero
void drawRightLeg() {
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	drawLeftLeg();
	glPopMatrix();
}

//void drawLeftArm()
//draw an arm of my hero
void drawLeftArm() {
	//draw using a scale of small rect
	glPushMatrix();
	glColor3f(.894, .678, .467);
	glRotatef(-90, 0, 0, 1);
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	glScalef(4, 1.5, 0);
	drawSmallRect();
	glPopMatrix();
	glPopMatrix();
}
//void drawRightArm()
//draw the right arm of my hero
void drawRightArm() {
	//draw using a scale of small rect
	glPushMatrix();
	glColor3f(.894, .678, .467);
	glRotatef(-90, 0, 0, 1);
	glScalef(4, 1.5, 0);
	drawSmallRect();
	glPopMatrix();
}

//void drawShuriken()
//draw the shuriken to be thrown at the target
void drawShuriken(double locationX, double locationY) {
	glPushMatrix();
	glTranslatef(locationX, locationY, 0);
	//draw first triangle
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef((37/3), (20/3), 0);
	glRotatef(shurikenRotation, 0, 0, 1);
	glTranslatef(-(37 / 3), -(20 / 3), 0);
	drawSmallTriangle();
	glPopMatrix();
	//draw second triangle
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef((37 / 3), (20 / 3), 0);
	glRotatef(shurikenRotation+180, 0, 0, 1);
	glTranslatef(-(37 / 3), -(20 / 3), 0);
	drawSmallTriangle();
	glPopMatrix();
	glPopMatrix();


}
//void waveArm()
//hero's constant motion
void waveArm() {
	if (armUp) {
		glRotatef(-armAngle, 0, 0, 1);
	}
	else if(!armUp){
		glRotatef(armAngle+180, 0, 0, 1);
	}
}

//void moveHeroLeft()
//move the hero character left
void moveHeroLeft() {
	myHeroX -= 4;
	heroRightArmX -= 4;
}

//void moveHeroRightt()
//move the hero character left
void moveHeroRight() {
	myHeroX += 4;
	heroRightArmX += 4;
}

//void throwShuriken()
//throw a shuriken from the right arm to the target (the mouse)
void throwShuriken(double rightArmX, double rightArmY, double mouseLocX, double mouseLocY) {
	shurikenLocationX = rightArmX;
	shurikenLocationY = rightArmY;
	mouseLocationX = mouseLocX;
	mouseLocationY = mouseLocY;
	//is this right? TODO
	shurikenAngleX = mouseLocationX - shurikenLocationX;
	shurikenAngleY = mouseLocationY - shurikenLocationY;
	DRAW_SHURIKEN = true;
}
//void drawHero()
//puts all the magic pieces together of my hero
void drawHero() {
	glPushMatrix();
	//translate to correct position
	glTranslatef(myHeroX, myHeroY, 0);

	glPushMatrix();
	//add left leg
	glPushMatrix();
	glTranslatef(10, 0, 0);
	drawLeftLeg();
	glPopMatrix();
	//add right leg
	glPushMatrix();
	glTranslatef(80, 0, 0);
	drawRightLeg();
	glPopMatrix();
	//add body
	glPushMatrix();
	glTranslatef(0, 100, 0);
	drawUpperBody();
	glPopMatrix();
	//add left arm
	glPushMatrix();
	glTranslatef(0, 200, 0);
	waveArm();
	drawLeftArm();
	glPopMatrix();
	//add right arm
	glPushMatrix();
	glTranslatef(90, 200, 0);
	drawRightArm();
	glPopMatrix();
	//add right arm
	glPushMatrix();
	glTranslatef(45, 225, 0);
	drawHead();
	glPopMatrix();

	glPopMatrix();
}


//
//  void renderScene()
//
//      We will register this function as GLUT's display callback.
//  This is where the magic happens - all rendering is done here.
//
void renderScene() {
	//add to back buffer
	glDrawBuffer(GL_BACK);

    // clear whatever was drawn to the screen last time - 
    // set the clear color to black and then signal to clear the RGB buffer.
    glClearColor( 0.26, 0.78, 0.78, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // switch to GL_MODELVIEW, for when we start using glTranslatef(), glScalef(), etc..
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // push the current matrix before doing anything;
    // just a good, safe practice
    glPushMatrix(); {

		// the triforce will get rendered around the origin,
		// so move the origin to (200,300)...
		//glTranslatef( 200, 300, 0 );
		//glTranslatef(myMouseX, myMouseY, 0.0f);

		// and then rotate it 45 degrees about its new origin
		//glRotatef( triforceAngle, 0, 0, 1 );

		// and then scale it 2X in x and 2X in y
		//glScalef( 10, 10, 1 );
		
		// send OpenGL a series of instructions that render our triforce
		drawHero();
		if (DRAW_SHURIKEN) {
			drawShuriken(shurikenLocationX, shurikenLocationY);
		}
	}; glPopMatrix(); // we pushed the matrix earlier, pop it to prevent overflows
	
    // flush the OpenGL commands and make sure they get rendered!
    //glFlush();
	glutSwapBuffers();
}

//LAB 01 IMPLEMENTATIONS BELOW THIS LINE

//void myKeyboard()
//this will control our keyboard calls
void myKeyboard(unsigned char key, int mouseX, int mouseY) {
	if (key == 27) {
		exit(0);
	}
	if (key == 119) {
		STOP_WAVE = !STOP_WAVE;
	}
	if (key == 97) {
		moveHeroLeft();
	}
	if (key == 100) {
		moveHeroRight();
	}
	if (key == 99) {
		throwShuriken(heroRightArmX, heroRightArmY, myMouseX, myMouseY);
	}
}

//void myTimer(int)
//timer function to enable animation of our triangles
void myTimer(int value) {
	if (armAngle < 180 && !STOP_WAVE) {
		armAngle += 2;
	}
	else if (armAngle == 180) {
		armUp = !armUp;
		armAngle = 0;
	}
	/**else if (armAngle > 0 && !armUp) {
		armAngle -= 2;
	}
	else if (armAngle == 0) {
		armUp = !armUp;
	}**/
	//throw the shuriken
	if (DRAW_SHURIKEN && (abs(shurikenLocationX - mouseLocationX) > 5 || abs(shurikenLocationY - mouseLocationY) > 5)) {
		shurikenLocationX += (shurikenAngleX/60);
		shurikenLocationY += (shurikenAngleY/60);
		shurikenRotation += 8;
	}
	else {
		DRAW_SHURIKEN = false;
	}
	//check if off screen
	if (myHeroX < -100) {
		myHeroX = 500;
		heroRightArmX = 590;
	}
	if (myHeroX > 512) {
		myHeroX = -100;
		heroRightArmX = -10;
	}
	glutPostRedisplay();
	glutTimerFunc(1000/60, myTimer, 0);
}

//void myMouse(int button, int state, int mouseX, int mouseY)
//all mouse movement functions
void myMouse(int button, int state, int mouseX, int mouseY) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			EVIL_TRIFORCE = true;
		}
		if (state == GLUT_UP) {
			throwShuriken(heroRightArmX, heroRightArmY, myMouseX, myMouseY);
		}
	}
}

//void myMotion(int mouseX, int mouseY)
//control passive mouse motions
void myMotion(int mouseX, int mouseY) {
	myMouseX = mouseX;
	myMouseY = windowHeight-(mouseY);
}

//
//  int main( int argc, char* argv[] )
//
int main( int argc, char* argv[] ) {
    // initialize GLUT... always need this call.
    glutInit( &argc, argv );

    // request a window with just an RGB frame buffer, nothing fancy...
    // place it at (50,50) on the screen and set its size to our constants from earlier
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowPosition( 50, 50 );
    glutInitWindowSize( windowWidth, windowHeight );
    
    // this actually creates our window. 
    windowId = glutCreateWindow( "Cyclopalypse Now" );

    // register our reshape and display callbacks with GLUT - these functions are above
    glutDisplayFunc( renderScene );
    glutReshapeFunc( changeSize );

	/********** ADD CALLBACKS ***************/
	glutKeyboardFunc(myKeyboard);
	glutTimerFunc(1000/60, myTimer, 0);
	glutMouseFunc(myMouse);
	glutPassiveMotionFunc(myMotion);
    // begin GLUT loop
    glutMainLoop();

    return 0;
}
