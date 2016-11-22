
#ifndef MYPOINT_H
#define MYPOINT_H

class MyPoint{
	public:
		float x, y, z;

		MyPoint(); //default constructor
		MyPoint(float myX, float myY, float myZ);

		float getX();
		float getY();
		float getZ();

		
};


MyPoint operator*( MyPoint p, float f ); // multiplies a MyPoint by a float
MyPoint operator*( float f, MyPoint p ); // multiplies a float by a MyPoint
MyPoint operator+( MyPoint a, MyPoint b ); // adds two Points together
#endif