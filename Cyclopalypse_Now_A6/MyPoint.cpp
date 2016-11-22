#include "MyPoint.h"
MyPoint::MyPoint(){ //default
	x = 0;
	y = 0;
	z = 0;
}
MyPoint::MyPoint(float myX, float myY, float myZ){
	x = myX;
	y = myY;
	z = myZ;
}
float MyPoint::getX(){
	return x;
}
float MyPoint::getY(){
	return y;
}
float MyPoint::getZ(){
	return z;
}
MyPoint operator*( MyPoint p, float f ) // multiplies a MyPoint by a float
{
	return MyPoint( p.getX() * f, p.getY() * f, p.getZ() * f );
}
MyPoint operator*( float f, MyPoint p ) // multiplies a float by a MyPoint
{
	return MyPoint( f*p.getX(), f*p.getY(), f*p.getZ());
}
MyPoint operator+( MyPoint a, MyPoint b ) // adds two Points together
{
	return MyPoint( a.getX()+b.getX(), a.getY()+b.getY(), a.getZ()+b.getZ());
}
