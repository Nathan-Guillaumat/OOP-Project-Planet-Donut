/*!
  \file    geomod.cc 
  \authors GUILLAUMAT Nathan
		   CRON Léo
  \date    Mars 2021
  \brief   Implémentation du module "geomod"
*/

#include <iostream> 
#include <cmath>
#include "geomod.h"
#include "graphic.h"

using namespace std;

namespace
{
	double epsilon_zero;
	double maxsize;
};

bool geomod::equal_zero ( double x) 
{
	if (abs(x) < epsilon_zero) 
	{
		return true ;
	}
	else 
	{ 
		return false; 
	}
}

double geomod::getMaxsize() {return maxsize;}
	
void geomod::setMaxsize (double x) 
{
	cout << x << endl; //--------------------------------------------------
	if (x < 0)
	{ 
		maxsize=abs(x);
	}
	else { maxsize = x; }
		
	epsilon_zero = pow(10,-10)*maxsize;
}

double geomod::getEpsilon_Zero() {return epsilon_zero;}

double geomod::normalisation (double vx)
{ 
	while (vx > maxsize) { return vx - 2*maxsize; }
	while (vx < (-maxsize)) { return vx + 2*maxsize; }
	return vx;
}

Point2D geomod::normalisation (Point2D p) 
{
	while (p.x > maxsize) { p.x = p.x - 2*maxsize; }
	while (p.x < (-maxsize)) { p.x = p.x + 2*maxsize; }
	while (p.y > maxsize) { p.y = p.y - 2*maxsize; }
	while (p.y < (-maxsize)) { p.y = p.y + 2*maxsize; }
	return p;
}

void geomod::normeMin (Point2D pA, Point2D pB, Vecteur& AB)
{
	for (int i(-1); i<2; ++i) 
	{
		for (int j(-1); j<2; ++j)
		{
			if(sqrt(pow(pB.x + i*2*maxsize - pA.x, 2) + 
			pow(pB.y + j*2*maxsize - pA.y, 2)) < 
			sqrt(pow(pB.x - pA.x, 2) + pow(pB.y - pA.y, 2))) 
			{
				pB.x += i*2*maxsize;
				pB.y += j*2*maxsize;
			}
		}
	}
	AB.A = pA;
	AB.B = pB;
}

double geomod::normeMin (Point2D pA, Point2D pB)
{
	for (int i(-1); i<2; ++i) 
	{
		for (int j(-1); j<2; ++j)
		{
			if(sqrt(pow(pB.x + i*2*maxsize - pA.x, 2) + 
			pow(pB.y + j*2*maxsize - pA.y, 2)) < 
			sqrt(pow(pB.x - pA.x, 2) + pow(pB.y - pA.y, 2))) 
			{
				pB.x += i*2*maxsize;
				pB.y += j*2*maxsize;
			}
		}
	}
return sqrt(pow(pB.x - pA.x, 2) + pow(pB.y - pA.y, 2));
}

bool geomod::testegalite (Point2D pointA, Point2D pointB)
{
	double test(0);
	
	test = normeMin (pointA, pointB); 
	
	if (test < epsilon_zero){return true;}
	else {return false;}
}

bool geomod::testdanscercle (Point2D point, Point2D centrecercle, double rayon)
{
	double n(0);
	
	n = normeMin (centrecercle, point); 
	
	if (n < (rayon-epsilon_zero)) {return true;}
	else {return false;}
}

bool geomod::intercercle (Point2D centre1, Point2D centre2, double r1, double r2)
{
	double n(0);
	n = normeMin (centre1, centre2);
	
	if (n< (r1+r2-epsilon_zero)) {return true;}
	else {return false;}
}

void geomod::dessin_point(Point2D position, double rayon, int couleur)
{
	Point2D centre2 {position.x, position.y};
	Point2D centre3 {position.x, position.y}; 
	Point2D centre4 {position.x, position.y}; 
	
	if(position.x+rayon > maxsize) { centre2.x -= 2*maxsize; }
	else if(position.x-rayon < -maxsize) { centre2.x += 2*maxsize; }
	
	if(position.y+rayon > maxsize) { centre3.y -= 2*maxsize; }
	else if(position.y-rayon < -maxsize) { centre3.y += 2*maxsize; }

		if((centre2.x != position.x) and (centre3.y != position.y))
	{ 
		centre4.x = centre2.x;
		centre4.y = centre3.y;
	}
	
	graphic::point(position.x, position.y, couleur);
	if(centre2.x != position.x) {
		graphic::point(centre2.x, centre2.y, couleur);
	}
	if(centre3.y != position.y) {
		graphic::point(centre3.x, centre3.y, couleur);
	}
	if(centre4.x != position.x) {
		graphic::point(centre4.x, centre4.y, couleur);
	}
}

void geomod::dessin_arc(Point2D position, double rayon, double angle_d, double angle_f, 
						int couleur, double epaisseur)
{
	Point2D centre2 {position.x, position.y};
	Point2D centre3 {position.x, position.y};
	Point2D centre4 {position.x, position.y};
	
	if(position.x+rayon > maxsize) { centre2.x -= 2*maxsize; }
	else if(position.x-rayon < -maxsize) { centre2.x += 2*maxsize; }
	
	if(position.y+rayon > maxsize) { centre3.y -= 2*maxsize; }
	else if(position.y-rayon < -maxsize) { centre3.y += 2*maxsize; }
	
	if((centre2.x != position.x) and (centre3.y != position.y))
	{ 
		centre4.x = centre2.x;
		centre4.y = centre3.y;
	}

	graphic::arc(position.x, position.y, rayon, angle_d, angle_f, couleur, epaisseur, 
				 false);

	if(centre2.x != position.x){
		graphic::arc(centre2.x, centre2.y, rayon, angle_d, angle_f, couleur, epaisseur, 
					 false);
	}

	if(centre3.y != position.y) {
		graphic::arc(centre3.x, centre3.y, rayon, angle_d, angle_f, couleur, epaisseur, 
					 false);
	}

	if(centre4.x != position.x){
		graphic::arc(centre4.x, centre4.y, rayon, angle_d, angle_f, couleur, epaisseur, 
					 false);
	}
}

void geomod::dessin_cercle(Point2D centre, double rayon, int couleur, double epaisseur, 
							bool remplissage)
{
	Point2D centre2 {centre.x, centre.y};
	Point2D centre3 {centre.x, centre.y};
	Point2D centre4 {centre.x, centre.y};
	
	if(centre.x+rayon > maxsize) { centre2.x -= 2*maxsize; }
	else if(centre.x-rayon < -maxsize) { centre2.x += 2*maxsize; }
	
	if(centre.y+rayon > maxsize) { centre3.y -= 2*maxsize; }
	else if(centre.y-rayon < -maxsize) { centre3.y += 2*maxsize; }
	
	if((centre2.x != centre.x) and (centre3.y != centre.y))
	{ 
		centre4.x = centre2.x;
		centre4.y = centre3.y;
	}

	graphic::arc(centre.x, centre.y, rayon, 0, 2*M_PI, couleur, epaisseur, 
				 remplissage);

	if(centre2.x != centre.x){
		graphic::arc(centre2.x, centre2.y, rayon, 0, 2*M_PI, couleur, epaisseur, 
					 remplissage);
	}

	if(centre3.y != centre.y){
		graphic::arc(centre3.x, centre3.y, rayon, 0, 2*M_PI, couleur, epaisseur, 
					 remplissage);
	}

	if(centre4.x != centre.x) {
		graphic::arc(centre4.x, centre4.y, rayon, 0, 2*M_PI, couleur, epaisseur, 
					 remplissage);
	}
}

void geomod::dessin_ligne(Point2D centreD, Point2D centreF, double epaisseur, 
							int couleur)
{
	Point2D centreD1 {centreD.x, centreD.y};
	Point2D centreF1 {centreF.x, centreF.y};
	
	Vecteur AB {centreD1, centreF1};
	Vecteur BA {centreF1, centreD1};
	
	geomod::normeMin(centreD1, centreF1, AB);
	geomod::normeMin(centreF1, centreD1, BA);
	
	graphic::ligne((AB.A).x, (AB.A).y, (AB.B).x, (AB.B).y, epaisseur, couleur);
	if((!geomod::testegalite(BA.B, AB.A)) or (!geomod::testegalite(BA.A, AB.B)))
		graphic::ligne((BA.A).x, (BA.A).y, (BA.B).x, (BA.B).y, epaisseur, couleur);
}

		
		
