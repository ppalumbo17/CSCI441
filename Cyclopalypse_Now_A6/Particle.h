#ifndef _PARTICLE_
#define _PARTICLE_

#include "./include/Point.h"
#include "./include/Vector.h"
#include "math.h"
	class Particle{
	public:
		double lifetime;
		double age;
		Point position;
		double mass;
		Vector velocity;

		Particle();
		Particle(double l, double m, Vector v, Point p);

		void draw();

		void drawTexturedSprite(float spriteXWidth, float spriteYWidth);

		void update();
	};
#endif