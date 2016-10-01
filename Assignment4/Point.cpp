#include "Point.h"
Point::Point(){ //default
	x = 0;
	y = 0;
	z = 0;
}
Point::Point(float myX, float myY, float myZ){
	x = myX;
	y = myY;
	z = myZ;
}
float Point::getX(){
	return x;
}
float Point::getY(){
	return y;
}
float Point::getZ(){
	return z;
}
Point operator*( Point p, float f ) // multiplies a Point by a float
{
	return Point( p.getX() * f, p.getY() * f, p.getZ() * f );
}
Point operator*( float f, Point p ) // multiplies a float by a Point
{
	return Point( f*p.getX(), f*p.getY(), f*p.getZ());
}
Point operator+( Point a, Point b ) // adds two Points together
{
	return Point( a.getX()+b.getX(), a.getY()+b.getY(), a.getZ()+b.getZ());
}
