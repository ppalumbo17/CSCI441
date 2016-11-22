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
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <GL/glui.h>

#include <SOIL/soil.h>

#include "include/Shader_Utils.h"
#include "include/Point.h"
#include "include/Vector.h"

#include "venus/Object.h"
#include "venus/Face.h"
#include "venus/Material.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "MyPoint.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>
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

Object *obj;
GLuint texHandleBk;
GLuint texHandleDn;
GLuint texHandleFt;
GLuint texHandleLf;
GLuint texHandleRt;
GLuint texHandleUp;

string fragmentShaderFile;
string vertexShaderFile;
string particleSystemFile;
//Assignment 4 Global Variables

static float aspectRatio;
static float cameraStepValue = 0.5;
float cameraX, cameraY, cameraZ;            // camera position in cartesian coordinates
float cameraTheta, cameraPhi;               // camera DIRECTION in spherical coordinates
float dirX, dirY, dirZ;                     // camera DIRECTION in cartesian coordinates

float characterX, characterY, characterZ; //character position
float transportRotation = 0;
float wheelRotation = 0;
float fanRotate =0;
float cameraRadius = 5;

unsigned char* brickArray;
int imageWidth;
int imageHeight;
GLuint brickTex;


GLint menuId;                               // handle for our menu

float sphereRadius = 0.5f; //the radius of the control points
float lineThickness = 3.0f;  //variable for line thickness between points

vector<vector<double> > *particleSystemsFiles;
vector<ParticleSystem *> particleSystems;
vector<MyPoint> controlPoints;
float trackPointVal = 0.0f;
float timerValue = 0.0f; //timer value to control pixie

bool DRAW_CAGE = true;  //Menu option for drawing the bezier points
bool DRAW_CURVE = true;  //Menu option for drawing the bezier curve
bool CTRL_PRESSED = false;
bool WHICH_POINTS = true;

MyPoint onBez;

bool keysPressed[256];
GLuint environmentDL;                       // display list for the 'city'
// END GLOBAL VARIABLES ////////////////////////////////////////////////////////


//Assignment 4 code for drawing cars and stuff////////////////////////////////////////

// getRand() ///////////////////////////////////////////////////////////////////
//
//  Simple helper function to return a random number between 0.0f and 1.0f.
//
////////////////////////////////////////////////////////////////////////////////
float getRand() {
    return rand() / (float)RAND_MAX;
}

//readPPM()
//
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
//registerOpenGLTexture()
//
bool registerOpenGLTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {
    // TODO #2b: Fill this in
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texHeight, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    return true;
}

//drawSkybox()

void drawSkybox(){
    float stepVal = 100;
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();{
        glTranslatef(-stepVal, 0, -stepVal);
        
            //bottom
            glPushMatrix();{
                glBindTexture(GL_TEXTURE_2D, texHandleDn);
                glBegin(GL_QUADS);{
                glNormal3f(0, 1, 0);
                glTexCoord2f(1, 0);
                glVertex3f(0, -stepVal, 0);
                glNormal3f(0, 1, 0);
                glTexCoord2f(1, 1);
                glVertex3f(2*stepVal, -stepVal, 0);
                glNormal3f(0, 1, 0);
                glTexCoord2f(0, 1);
                glVertex3f(2*stepVal, -stepVal, 2*stepVal);
                glNormal3f(0, 1, 0);
                glTexCoord2f(0, 0);
                glVertex3f(0, -stepVal, 2*stepVal);
                }; glEnd();
            };glPopMatrix();
            //back
            glPushMatrix();{
                glBindTexture(GL_TEXTURE_2D, texHandleBk);
                glBegin(GL_QUADS);{
                glNormal3f(0, 0, 1);
                glTexCoord2f(1, 0);
                glVertex3f(0, -stepVal, 0);
                glNormal3f(0, 0, 1);
                glTexCoord2f(1, 1);
                glVertex3f(2*stepVal, -stepVal, 0);
                glNormal3f(0, 0, 1);
                glTexCoord2f(0, 1);
                glVertex3f(2*stepVal, stepVal, 0);
                glNormal3f(0, 0, 1);
                glTexCoord2f(0, 0);
                glVertex3f(0, stepVal, 0);
                }; glEnd();
            };glPopMatrix();
            //right
            glPushMatrix();{
                glBindTexture(GL_TEXTURE_2D, texHandleRt);
                glBegin(GL_QUADS);{
                glNormal3f(1, 0, 0);
                glTexCoord2f(1, 0);
                glVertex3f(2*stepVal, -stepVal, 0);
                glNormal3f(1, 0, 0);
                glTexCoord2f(1, 1);
                glVertex3f(2*stepVal, -stepVal, 2*stepVal);
                glNormal3f(1, 0, 0);
                glTexCoord2f(0, 1);
                glVertex3f(2*stepVal, stepVal, 2*stepVal);
                glNormal3f(1, 0, 0);
                glTexCoord2f(0, 0);
                glVertex3f(2*stepVal, stepVal, 0);
                }; glEnd();
            };glPopMatrix();
            //front
            glPushMatrix();{
                glBindTexture(GL_TEXTURE_2D, texHandleFt);
                glBegin(GL_QUADS);{
                glNormal3f(0, 0, -1);
                glTexCoord2f(1, 0);
                glVertex3f(0, -stepVal, 2*stepVal);
                glNormal3f(0, 0, -1);
                glTexCoord2f(1, 1);
                glVertex3f(2*stepVal, -stepVal, 2*stepVal);
                glNormal3f(0, 0, -1);
                glTexCoord2f(0, 1);
                glVertex3f(2*stepVal, stepVal, 2*stepVal);
                glNormal3f(0, 0, -1);
                glTexCoord2f(0, 0);
                glVertex3f(0, stepVal, 2*stepVal);
                }; glEnd();
            };glPopMatrix();
            //left
            glPushMatrix();{
                glBindTexture(GL_TEXTURE_2D, texHandleLf);
                glBegin(GL_QUADS);{
                glNormal3f(-1, 0, 0);
                glTexCoord2f(1, 0);
                glVertex3f(0, -stepVal, 0);
                glNormal3f(-1, 0, 0);
                glTexCoord2f(1, 1);
                glVertex3f(0, -stepVal, 2*stepVal);
                glNormal3f(-1, 0, 0);
                glTexCoord2f(0, 1);
                glVertex3f(0, stepVal, 2*stepVal);
                glNormal3f(-1, 0, 0);
                glTexCoord2f(0, 0);
                glVertex3f(0, stepVal, 0);
                }; glEnd();
            };glPopMatrix();
            //top
            glPushMatrix();{
                glBindTexture(GL_TEXTURE_2D, texHandleUp);
                glBegin(GL_QUADS);{
                glNormal3f(0, -1, 0);
                glTexCoord2f(1, 0);
                glVertex3f(0, stepVal, 0);
                glNormal3f(0, -1, 0);
                glTexCoord2f(1, 1);
                glVertex3f(2*stepVal, stepVal, 0);
                glNormal3f(0, -1, 0);
                glTexCoord2f(0, 1);
                glVertex3f(2*stepVal, stepVal, 2*stepVal);
                glNormal3f(0, -1, 0);
                glTexCoord2f(0, 0);
                glVertex3f(0, stepVal, 2*stepVal);
                }; glEnd();
            };glPopMatrix();
        
    glDisable(GL_TEXTURE_2D);
    };glPopMatrix();
}
// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawGrid() {
    int gridX = 100;
    int gridY = 100;
    float spacing = 0.5f;
    /*
     *  We will get to why we need to do this when we talk about lighting,
     *  but for now whenever we want to draw something with an OpenGL
     *  Primitive - like a line, quad, point - we need to disable lighting
     *  and then reenable it for use with the GLUT 3D Primitives.
     */
    glEnable( GL_LIGHTING );

    /** TODO #3: DRAW A GRID IN THE XZ-PLANE USING GL_LINES **/
    // glBegin(GL_LINES);
    // glColor3f(1, 1, 1);
    // for (int i = -50; i < 50; i++) {
    //     glVertex3f(-50, 0, i);
    //     glVertex3f(50, 0, i);
    //     glVertex3f(i, 0, -50);
    //     glVertex3f(i, 0, 50);
    // }
    // glEnd();
    // GLfloat color[4] = { .61424, .07568, .07568, 1 };
    // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
    Material *mat = new Material(GOL_MATERIAL_BRASS);
    setCurrentMaterial(mat);
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
    Material *newMat = new Material(GOL_MATERIAL_WHITE);
    setCurrentMaterial(newMat);
    /*
     *  As noted above, we are done drawing with OpenGL Primitives, so we
     *  must turn lighting back on.
     */
    glDisable( GL_LIGHTING );
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
                //glColor3f(getRand(), getRand(), getRand());
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
        //glColor3f(.7, .3, .3);
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
        //glColor3f(.3,.9,.1);
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
        
        //glColor3f(255,0.0,0.0);
        glPushMatrix();{
            glTranslatef(0.5, 1, 0); //rotate the wheels around the axel
            glRotatef(wheelRotation, 0, 0, 1);
            glutSolidTorus(1, 2, 50, 20); 
            //DON'T THINK THE WHEEL IS MOVING!?!? ME NEITHER
            //SO I PUT THIS RANDOM ASS TRIANGLE IN HERE TO PROVE IT UNCOMMENT IF YOU WANT PROOF
            // glBegin(GL_TRIANGLES); //begin random triangle //UNCOMMENT
            // glPushMatrix();{ //UNCOMMENT
            // glColor3f(.7,.3,.3); //UNCOMMENT
            //  glVertex3f(5, 2*2, 0); //UNCOMMENT
            //  glVertex3f(10, 2*2, 5); //UNCOMMENT
            //  glVertex3f(0, 2*2, 5); //UNCOMMENT
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
            //  glVertex3f(x/2, height*2, 0);
            //  glVertex3f(x, height*2, z/2);
            //  glVertex3f(0, height*2, z/2);
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
    
    
    //glColor3f(.7,.3,.7);
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
        //glColor3f(.3,.3,.7);
        glTranslatef(0,0,1);
        drawBody();
    };glPopMatrix();
    glPushMatrix();{
        //glColor3f(.7,.3,.7);
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
        //glColor4f(.2, .2, .8, .4);
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
        //  // glRotatef(-fanRotate, 0,1,-1);
        //  glTranslatef(1,1,1);
        //  drawSolidSphere();
        // };glPopMatrix();
        // //Six Sphere
        // glPushMatrix();{
        //  // glRotatef(-fanRotate, -1,1,0);
        //  glTranslatef(-1,-1,-1);
        //  drawSolidSphere();
        // };glPopMatrix();
        // //Seven Sphere
        // glPushMatrix();{
        //  // glRotatef(-fanRotate, 0,-1,1);
        //  glTranslatef(1,-1,-1);
        //  drawSolidSphere();
        // };glPopMatrix();
        // //Eight Sphere
        // glPushMatrix();{
        //  //glRotatef(-fanRotate, 1,-1,0);
        //  glTranslatef(-1,-1,1);
        //  drawSolidSphere();
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
       // drawCity();
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
MyPoint evaluateBezierCurve( MyPoint p0, MyPoint p1, MyPoint p2, MyPoint p3, float t ) {
    // TODO #08: Compute a point along a Bezier curve
    float bezX = pow(1-t, 3)*p0.getX() + 3*pow(1-t, 2)*t*p1.getX() + 3*(1-t)*pow(t, 2)*p2.getX() + pow(t, 3)*p3.getX();
    float bezY = pow(1-t, 3)*p0.getY() + 3*pow(1-t, 2)*t*p1.getY() + 3*(1-t)*pow(t, 2)*p2.getY() + pow(t, 3)*p3.getY();
    float bezZ = pow(1-t, 3)*p0.getZ() + 3*pow(1-t, 2)*t*p1.getZ() + 3*(1-t)*pow(t, 2)*p2.getZ() + pow(t, 3)*p3.getZ();

    return MyPoint(bezX, bezY, bezZ);
}

// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution. 
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve( MyPoint p0, MyPoint p1, MyPoint p2, MyPoint p3, int resolution ) {
    // TODO #07: Draw a Bezier curve
    glPushMatrix();
    glLineWidth(lineThickness);
    //glColor3f(0,0,1);
    glBegin(GL_LINE_STRIP);
    for(float i = 0; i < resolution; i+=.4){
        float time = float(i) / float(resolution);
        MyPoint eval = evaluateBezierCurve(p0, p1, p2, p3, time);
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
        //glColor3f(.2, .8, .2);
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
                MyPoint new_Point(input_Points.at(0),input_Points.at(1),input_Points.at(2));
                controlPoints.push_back(new_Point);
            }
        }
    }
    file.close();
    return true;
}
//loads the particle system parameters
bool loadParticleSystems(char* filename){
    ifstream file(filename);
    if(!file.good()){
        return false;
    }
    if(file.is_open()){
        while(file.good()){
            // int type;
            // double x;
            // double y;
            // double z;
            // double angle;
            // double minV;
            // double maxV;
            // double minL;
            // double maxL;
            // int spawn;
            // file >> type >> x >> y >> z >> angle >> minV >> maxV >> minL >> maxL >> spawn;
            // particleSystems.push_back(new ParticleSystem(type,x,y,z,angle,minV,maxV,minL,maxL,spawn));
            // vector<double> particles;
            // while(getline(lineStream, value, ',')){
            //     particles.push_back(atof(value.c_str()));
            // }
            // if(file.good()){
            //     particleSystems.push_back(&particles);
            // }

        }
    }
    file.close();
    return true;

}
//create the particle systems
void createParticleSystems(){
    // for(int i = 0; i < particleSystemsFiles.size(); i ++){
    //     // ParticleSystem tmp = ParticleSystem(particleSystemsFiles.at(i).at(0),
    //     //     particleSystemsFiles.at(i).at(1),
    //     //     particleSystemsFiles.at(i).at(2),
    //     //     particleSystemsFiles.at(i).at(3),
    //     //     particleSystemsFiles.at(i).at(4),
    //     //     particleSystemsFiles.at(i).at(5),
    //     //     particleSystemsFiles.at(i).at(6),
    //     //     particleSystemsFiles.at(i).at(7),
    //     //     particleSystemsFiles.at(i).at(8),
    //     //     particleSystemsFiles.at(i).at(9));
    //     // particleSystems.push_back(tmp);
    // }
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
                    //glColor3f(1,1,0);
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

//End Assignment 4 code ///////////////////////////////////////////////////////////


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

    if(glutGetModifiers()==GLUT_ACTIVE_CTRL){
        CTRL_PRESSED = true;
    }    
    else{
        CTRL_PRESSED = false;
    }
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}


//TODO: THIS MAY BE DIFFERENT FROM ASSIGNMENT
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

//TODO: ALSO DIFFERENT FROM ASSIGNMENT
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

    srand( time(NULL) );    // seed our random number generator
    generateEnvironmentDL();
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for scene rendering.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(void)  {
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDrawBuffer(GL_BACK);
    
    // compute where on the sphere our camera is located
    Point cameraPosition;
    computeArcballPosition(cameraTPR, cameraPosition);
    
    // now compute where our sphere is located relative to the look at point
    Point eyePosition = cameraPosition + (cameraLookAt - Point(0,0,0));
    
    //update the modelview matrix based on the camera's position
    glMatrixMode(GL_MODELVIEW);         //make sure we aren't changing the projection matrix!
    glLoadIdentity();
    gluLookAt( eyePosition, cameraLookAt, Vector(0,1,0) );

    //lookat car
    // gluLookAt( cameraX+characterX, cameraY+characterY, cameraZ+characterZ,      // camera is located at (10,10,10)
    //             //cameraX, cameraY, cameraZ,
    //             characterX,  characterY,  characterZ,       // camera is looking at (0,0,0)
    //             //characterX, characterY, characterZ,
    //             0,  1,  0); 
    
    glUseProgram(0);
    drawSkybox();
    //TODO: UPDATE FROM ASSIGNMENT
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

    glPushMatrix();{
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        Material *charMat = new Material(GOL_MATERIAL_REDPLASTIC);
        setCurrentMaterial(charMat);
        drawCharacter();
    };glPopMatrix();
    glPushMatrix();{
        glTranslatef(2,1,2);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, brickTex);
        glFrontFace( GL_CW );
        glutSolidTeapot( 1.5 );
        glFrontFace( GL_CCW );
    };glPopMatrix();


    // this is some code to enable a default light for the scene;
    // feel free to play around with this, but we won't talk about
    // lighting in OpenGL for another couple of weeks yet.
    float lPosition[4] = { 10, 10, 10, 1 };
    glLightfv( GL_LIGHT0,GL_POSITION,lPosition );
    
    // draw everything of interest

    glPushMatrix(); {
    	// TODO #11: use our shader for the first sphere, but not the second sphere
		

		//glutSolidSphere( 5.0f, 32.0f, 32.0f );
        glPushMatrix(); {
            glEnable(GL_LIGHTING);
            glUseProgram(shaderProgramHandle);
        // TODO #16: get our elapsed time and pass it to the shader
            float elapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000;
            glUniform1f(uniformTimeLoc, elapsedTime);
            glTranslatef(0,5,0);
            obj->draw();
        }; glPopMatrix();

		glUseProgram(0);
		glTranslatef( 0, 0, -10 );
    	//glutSolidSphere( 5.0f, 32.0f, 32.0f );
        glDisable(GL_LIGHTING);
    }; glPopMatrix();

    //all particle stuff
    for(unsigned int i = 0; i < particleSystems.size(); i ++){
        particleSystems.at(i)->draw();
    }
    //push the back buffer to the screen
    glutSwapBuffers();
}

//
//  void normalKeys(unsigned char key, int x, int y)
//
//      GLUT keyboard callback for regular characters.
//
// void normalKeys(unsigned char key, int x, int y)  {
//     //kindly quit if the user requests!
//     if(key == 'q' || key == 'Q')
//         exit(0);
    
//     // move camera forward - shrink the radius (remember Z = Radius for spherical point)
//     else if(key == 'z' || key == 'Z')
//         cameraTPR.setZ( cameraTPR.getZ() - 0.1f );
    
//     // move camera backward - increase the radius
//     else if(key == 'x' || key == 'X')
//         cameraTPR.setZ( cameraTPR.getZ() + 0.1f );
// }
void normalKeysDown(unsigned char key, int x, int y) {
    float length = sqrt(pow(cameraX, 2) + pow(cameraY, 2) + pow(cameraZ, 2));
    if(key == 'q' || key == 'Q' || key == 27)
        exit(0);
    // move camera forward - shrink the radius (remember Z = Radius for spherical point)
    else if(key == 'z' || key == 'Z')
        cameraTPR.setZ( cameraTPR.getZ() - 0.5f );
    
    // move camera backward - increase the radius
    else if(key == 'x' || key == 'X')
        cameraTPR.setZ( cameraTPR.getZ() + 0.5f );

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
    //  CTRL_PRESSED = !(CTRL_PRESSED);
    // }

    // if (key == 'wd' && key == 'd') {
    //  transportRotation += 2;
    //  if(characterX < 49 && characterX > -49 && characterZ < 49 && characterZ > -49){
    //      characterZ += cameraStepValue*sin(transportRotation*M_PI/180);
    //      characterX -= cameraStepValue*cos(transportRotation*M_PI/180);
    //      cameraZ += cameraStepValue*sin(transportRotation*M_PI/180);
    //      cameraX -= cameraStepValue*cos(transportRotation*M_PI/180);
    //      transportRotation += 2;
    //      wheelRotation += 1;
    //  }
    //  else{
    //      characterZ -= cameraStepValue*sin(transportRotation);
    //      characterX += cameraStepValue*cos(transportRotation);
    //      cameraZ -= cameraStepValue*sin(transportRotation);
    //      cameraX += cameraStepValue*cos(transportRotation);
    //  }
    // }
    //glutPostRedisplay();      // redraw our scene from our new camera POV
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
    //  CTRL_PRESSED = !(CTRL_PRESSED);
    // }
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
    if(timerValue>=1.0){
        timerValue =0;
        WHICH_POINTS = !(WHICH_POINTS);
    }
    if(WHICH_POINTS){
        onBez = evaluateBezierCurve(controlPoints.at(0), controlPoints.at(1), controlPoints.at(2), controlPoints.at(3), timerValue);
        // glPushMatrix();{
        //  glTranslatef(characterX+sin(transportRotation*M_PI/180),characterY-2,characterZ+cos(transportRotation*M_PI/180));
        //  glRotatef(transportRotation, 0, 1, 0);
        //  glScalef(.25,.25,.25);
        //  drawSolidSphere(onBez.getX(),onBez.getY(),onBez.getZ());
        //  };glPopMatrix();
    }
    else{
        onBez = evaluateBezierCurve(controlPoints.at(3), controlPoints.at(4), controlPoints.at(5), controlPoints.at(6), timerValue);
        // glPushMatrix();{
        //  glTranslatef(characterX+sin(transportRotation*M_PI/180),characterY-2,characterZ+cos(transportRotation*M_PI/180));
        //  glRotatef(transportRotation, 0, 1, 0);
        //  glScalef(.25,.25,.25);
        //  drawSolidSphere(onBez.getX(),onBez.getY(),onBez.getZ());
        //  };glPopMatrix();
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
        //  characterZ += cameraStepValue*sin(transportRotation);
        //  characterX -= cameraStepValue*cos(transportRotation);
        //  cameraZ += cameraStepValue*sin(transportRotation);
        //  cameraX -= cameraStepValue*cos(transportRotation);
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
        //  characterZ -= cameraStepValue*sin(transportRotation);
        //  characterX += cameraStepValue*cos(transportRotation);
        //  cameraZ -= cameraStepValue*sin(transportRotation);
        //  cameraX += cameraStepValue*cos(transportRotation);
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
	//shaderProgramHandle = createShaderProgram((char *)vertexShaderFile.c_str(), (char *)fragmentShaderFile.c_str());
    shaderProgramHandle = createShaderProgram("./shaders/customShader.v.glsl", "./shaders/customShader.f.glsl");
	//createShaderProgram();

	// TODO #15: get our uniform location
	uniformTimeLoc = glGetUniformLocation(shaderProgramHandle, "time");
	
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
    if(argc != 6){
        cout << "Proper sequence is .exe controlpoints.csv object.obj framentFile.glsl vertexFile.glsl particleSystems.csv" << endl;
        return(-1);
    }
    bool file_Good = loadControlPoints(argv[1]);
    if(!file_Good){
        cout << "Your file input was not good, please try again!" << endl;
        exit(-1);
    }

    
    // create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );
    glutInitWindowPosition( 50, 50 );
    glutInitWindowSize( windowWidth, windowHeight );
    glutCreateWindow( "Cyclopalypse Now Assignment 5" );

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

    createMenus();
    //do some basic OpenGL setup
    initScene();
    printf( "[INFO]: OpenGL Scene set up\n" );
    
    // set up our shaders (the files are hardcoded in)
    setupShaders();
    printf( "[INFO]: Shader compilation complete.\n" );


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
    //glutKeyboardFunc(normalKeys);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);

    obj = new Object(argv[2]);
    fragmentShaderFile = argv[4];
    vertexShaderFile = argv[3];
    particleSystemFile = argv[5];

    //Test particle systems
    particleSystems.push_back(new ParticleSystem(1, 2.0, 2.0, 2.0, 10.0, 2.0, 4.0, 10.0, 20.0, 2));

    //register texture
    string brickFile = "./brick.ppm";
    readPPM(brickFile, imageWidth, imageHeight, brickArray);
    // TODO #2a: Register PPM Texture
    registerOpenGLTexture(brickArray, imageWidth, imageHeight, brickTex);

//Register our skybox
    texHandleBk = SOIL_load_OGL_texture("./skybox/ely_cloudtop/cloudtop_bk.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS| SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT);
    texHandleDn = SOIL_load_OGL_texture("./skybox/ely_cloudtop/cloudtop_dn.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS| SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT);
    texHandleFt = SOIL_load_OGL_texture("./skybox/ely_cloudtop/cloudtop_ft.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS| SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT);
    texHandleLf = SOIL_load_OGL_texture("./skybox/ely_cloudtop/cloudtop_lf.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS| SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT);
    texHandleRt = SOIL_load_OGL_texture("./skybox/ely_cloudtop/cloudtop_rt.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS| SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT);
    texHandleUp = SOIL_load_OGL_texture("./skybox/ely_cloudtop/cloudtop_up.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS| SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);

    // and enter the GLUT loop, never to exit.
    glutMainLoop();

    return(0);
}
