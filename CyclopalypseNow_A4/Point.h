
#ifndef POINT_H
#define POINT_H

class Point{
	public:
		float x, y, z;

		Point(); //default constructor
		Point(float myX, float myY, float myZ);

		float getX();
		float getY();
		float getZ();

		
};


Point operator*( Point p, float f ); // multiplies a Point by a float
Point operator*( float f, Point p ); // multiplies a float by a Point
Point operator+( Point a, Point b ); // adds two Points together
#endif