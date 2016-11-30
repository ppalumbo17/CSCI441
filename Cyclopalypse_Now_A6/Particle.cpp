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

		// Vector normal = Vector(0,0,1);
	 //    // Vector dir = cameraXYZ-forestCenter;
	 //    Vector dir = Vector(forestCenter.getX()-cameraXYZ.getX(), 0.0, forestCenter.getZ()-cameraXYZ.getZ());
	 //    normal.normalize();
	 //    dir.normalize();
	 //    // Vector tar = forestCenter-normal;
	 //    float rotation = acos(dot(dir,normal))*180/M_PI;
	 //    Vector axis = cross(normal,dir);

		int spriteSize = 20;
		glPushMatrix();
		glTranslatef(position.getX(), position.getY(), position.getZ());
		// glBegin(GL_QUADS);{
		// 	glNormal3f(0,0,1);
		// 	glTexCoord2f(0,0.01);
		// 	glVertex3f(0, 0, 0);

		// 	glNormal3f(0,0,1);
		// 	glTexCoord2f(0,1);
		// 	glVertex3f(0, spriteSize, 0);

		// 	glNormal3f(0,0,1);
		// 	glTexCoord2f(0, 1);
		// 	glVertex3f(spriteSize, spriteSize, 0);

		// 	glNormal3f(0,0,1);
		// 	glTexCoord2f(0,0.01);
		// 	glVertex3f(spriteSize, 0, 0);
		// }glEnd();
		// glRotatef(rotation, 0, axis.getY(), 0);
		// glRotatef(rotation, 0, 1, 0);
		drawTexturedSprite(spriteSize, spriteSize);
		glPopMatrix();

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

	void Particle::drawTexturedSprite( float spriteXWidth, float spriteYWidth ) {
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