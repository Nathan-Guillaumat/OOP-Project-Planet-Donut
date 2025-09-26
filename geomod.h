/*!
  \file    geomod.h 
  \authors GUILLAUMAT Nathan
		   CRON Léo
  \date    Mars 2021
  \brief   Interface du module "geomod"
*/

#ifndef GEOMOD_H
#define GEOMOD_H

	struct Point2D 
	{
		double x;
		double y;
	};

	struct Vecteur
	{
		Point2D A;
		Point2D B;
	};
	
namespace geomod 
{

	bool equal_zero (double x);
	double getMaxsize();
	void setMaxsize(double x);
	double getEpsilon_Zero();
	
	double normalisation (double vx);
	Point2D normalisation (Point2D p);
	
	void normeMin (Point2D pA, Point2D pB, Vecteur& AB); 
	double normeMin (Point2D pA, Point2D pB);
	
	bool testegalite (Point2D pointA, Point2D pointB);
	
	bool testdanscercle (Point2D point, Point2D centrecercle, double rayon);
	bool intercercle (Point2D centre1, Point2D centre2, double r1, double r2);
	
	void dessin_point(Point2D position, double rayon, int couleur);
	void dessin_arc(Point2D position, double rayon, double angle_d, double angle_f, 
					int couleur, double epaisseur);
	void dessin_cercle(Point2D centre, double rayon, int couleur, double epaisseur, 
						bool remplissage);
	void dessin_ligne(Point2D centreD, Point2D centreF, double epaisseur, 
						int couleur);
};
#endif
