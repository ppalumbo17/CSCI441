//Peter Palumbo

float x, y, z;

Point() {
	x = 0;
	y = 0;
	z = 0;
}

Point(float newX, float newY, float newZ) {
	x = newX;
	y = newY;
	z = newZ;
}

Point operator*(Point p, float f); // multiplies a Point by a float
Point operator*(float f, Point p); // multiplies a float by a Point
Point operator+(Point a, Point b); // adds two Points together