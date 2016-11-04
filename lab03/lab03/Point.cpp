
#include "Point.h"

//multiply a point by a float
Point operator*(Point p, float f) {
	return Point(p.getX() * f, p.getY() * f, p.getZ() * f);
}
//multiply a float by a point
Point operator*(float f, Point p) {
	return Point(p.getX() * f, p.getY() * f, p.getZ() * f);
}
//add two Points together
Point operator+(Point a, Point b) {
	return Point(a.getX() + b.getX(), a.getY() + b.getY(), a.getZ() + b.getZ());
}
