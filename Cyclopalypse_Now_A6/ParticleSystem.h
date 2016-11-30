#ifndef _PARTICLE_SYSTEM_
#define _PARTICLE_SYSTEM_
#include "./Particle.h"
#include <vector>
#include "./include/Point.h"
#include "./include/Vector.h"
	class ParticleSystem{
		public:
			//enum type{FOUNTAIN, RAIN};
			

			ParticleSystem();
			ParticleSystem(int t, double x, double y, double z, double c, double minV, double maxV, double minL, double maxL, int s);

			void draw();
			void update(); //add and remove particles

			double getMinV();
			double getMaxV();
			double getMinL();
			double getMaxL();
			
		private:
			Point source;
			double cone;
			double minVelocity;
			double maxVelocity;
			double minLifespan;
			double maxLifespan;
			int spawn;
			std::vector< Particle* > particles;
			int timer =0;
	};

#endif