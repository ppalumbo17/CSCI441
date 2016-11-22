#include "ParticleSystem.h"
#include "Particle.h"
#ifdef __APPLE__
	#include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
    #include <GL/glu.h>
#endif
#include <math.h>
#include <vector>
#include <stdlib.h>
	ParticleSystem::ParticleSystem(){
		this->type = FOUNTAIN:
		this->source = Point(0,0,0);
		this->cone=minVelocity=maxVelocity=minLifespan=maxLifespan=spawn=0;
	}
	ParticleSystem::ParticleSystem(int t, double x, double y, double z, double c, double minV, double maxV, double minL, double maxL, int s){
		// switch(t){
		// 	case 1:
		// 		this->type = FOUNTAIN;
		// 		break;
		// 	case 2:
		// 		this->type = RAIN;
		// 		break;
		// }
		this->source = Point(x,y,z);
		this->cone = c;
		this->minVelocity = minV;
		this->maxVelocity = maxV;
		this->minLifespan = minL;
		this->maxLifespan = maxL;
		this->spawn = s;
	}
	void ParticleSystem::draw(){
		for(int i = 0; i< particles.size(); i++){
			particles.at(i)->draw();
		}
	}
	void ParticleSystem::update(){
		double life = rand() % (int)(maxLifespan + minLifespan);
		double degrees = rand() % 360;
		double vel = minVelocity + (rand() %(int)(maxVelocity-minVelocity));
		double angle = 90 - (rand() % (int)cone);
		double vX = sin(degrees*PI/180);
		double vY = sin(cone*PI/180)*vel;
		double vZ = sin(degrees*PI/180)*vel;

		if(spawn/60 >= 1){
			for(int i = 0; i < (spawn/60); i++){
				particles.push_back(new Particle(life, 1, Vector(vX, vY, vZ), Point(0,0,0)));
			}
		}
		else if(timer == 60){
			particles.push_back(new Particle(life, 1, Vector(vX, vY, vZ), Point(0,0,0)));
			timer = 0;
		}
		else{
			timer++;
		}
		for(unsigned int j = 0; j < particles.size(); j++){
			particles.at(j)->update();
		}

		vector< Particle *> newParticles;
		for(unsigned int k = 0; k < particles.size(); k++){
			if(particles.at(k)->age > particles.at(k)->lifetime){
				delete particles.at(k);
			}
			else{
				newParticles.push_back(particles.at(k));
			}
		}
		particles = newParticles;
	}