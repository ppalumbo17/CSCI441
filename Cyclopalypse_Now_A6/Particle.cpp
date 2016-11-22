#include "Particle.h"

#ifdef __APPLE__
	#include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
    #include <GL/glu.h>
#endif

	Particle::Particle(){
		lifetime=age=mass=0; velocity=Vector(0,0,0); position = Point(0,0,0);
	}
	Particle::Particle(double l, double m, Vector v, Point p){
		lifetime = l;
		mass = m;
		velocity = v;
		age = 0;
		position = p;
	}
	void Particle::draw(){
		glPushMatrix();
		glTranslatef(position.getX(), position.getY(), position.getZ());
		glBegin(GL_QUADS);{
			glNormal3f(0,0,1);
			glTexCoord2f(0,0.01);
			glVertex3f(0, 0, 0);

			glNormal3f(0,0,1);
			glTexCoord2f(0,1);
			glVertex3f(0, 1, 0);

			glNormal3f(0,0,1);
			glTexCoord2f(0, 1);
			glVertex3f(1, 1, 0);

			glNormal3f(0,0,1);
			glTexCoord2f(0,0.01);
			glVertex3f(1, 0, 0);
		}glEnd();

	}
	void Particle::update(){
		age+= 1.0/60.0;
		position.setX(position.getX() + (1.0/60.0)*velocity.getX()*age);
		position.setY(position.getY() + (1.0/60.0)*velocity.getY()*age-0.5*9.81*pow(age, 2));
		position.setZ(position.getZ() + (1.0/60.0)*velocity.getZ()*age);

		if(position.getY() < 0.5){
			position.setY(0.5);
		}
		
	}