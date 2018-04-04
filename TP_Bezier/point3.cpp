#include "point3.h"

point3::point3(){
	x = 0.0; y = 0.0; z = 0.0;
}

point3::point3(const double X,const double Y,const double Z){
	x = X; y = Y; z = Z;
}

bool point3::operator==(const point3 &op) const {
	return( (x == op.x) && (y == op.y) && (z == op.z) ) ;
}

point3& point3::operator=(const point3 &op) {
	x = op.x; y = op.y; z = op.z;
return *this;
}

point3 point3::operator+(const point3 &op) const {
	return( point3( x + op.x, y + op.y, z + op.z) );
}

point3& point3::operator*=(const double op) {
	x *= op; y *= op; z *= op;
return *this;
}

point3 point3::operator*(const double op) const {
	return ( point3( x * op, y * op, z * op) );
}

point3& point3::operator/=(const double op) {
	x /= op; y /= op; z /= op;
return *this;
}

point3 point3::operator/(const double op) const {
	return ( point3( x / op, y / op, z / op) );
}

ostream&  operator<<(ostream& p, point3 op) {
	p << "(" << op.x <<", " << op.y << "," << op.z  << ")";
return(p);
}
