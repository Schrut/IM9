#ifndef _H_POINT3
#define _H_POINT3

#include <iostream>
#include <ostream>
using namespace std;

class point3 {

public:

	double x;
	double y;
	double z;

	//constructeurs
	point3(); // (0,0,0) par defaut
	point3(const double, const double, const double);


	bool operator==(const point3 &)const;

	point3& operator=(const point3 &);
	point3 operator+(const point3 &)const;
	point3& operator*=(const double); //produit par un scalaire
	point3 operator*(const double)const; //idem
	point3& operator/=(const double); //division par un scalaire
	point3 operator/(const double)const; //idem

	friend ostream& operator<<(ostream&, point3); //affichage éventuels
};

#endif
