/*!
  \file    gisement.cc
  \authors CRON léo
		   GUILLAUMAT Nathan
  \date    Avril 2021
  \brief   Implémentation du module "gisement"
*/

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "gisement.h"
#include "message.h"
#include "geomod.h"
#include "constantes.h"

using namespace std;

// Constructeurs

Gisement::Gisement(double x, double y, double rayon, double capacite)
	: centre({x,y}), rayon(rayon), gisementR(capacite)
	{}

Gisement::Gisement(string ligne)
	{
		istringstream data(ligne);
		data >> centre.x >> centre.y >> rayon >> gisementR;
		centre = geomod::normalisation(centre);
	}

Gisement::Gisement()
	: centre({0, 0}), rayon(0), gisementR(0)
	{}
	
// Implementation des methodes

Gisement* Gisement::operator=(Gisement* g)
{
	centre = g->centre;
	rayon = g->rayon;
	gisementR = g->gisementR;
	return this;
}

Point2D Gisement::getCentre() const
{
	return centre;
}

double Gisement::getRayon() const
{
	return rayon;
}

double Gisement::getCapacite() const 
{ 
	return gisementR; 
}

void Gisement::actualiserR()
{
	if(gisementR > deltaR) gisementR -= deltaR;
}

void Gisement::setCapacite(double cap){gisementR=cap;}

void Gisement::setEtat(bool e) { etat = e; }
bool Gisement::getEtat() { return etat; }
void Gisement::setForeur(bool p) { foreur = p; }
bool Gisement::getForeur() { return foreur; }

void Gisement::intersection(Gisement& g1)
{
	Point2D centre1(centre);
	double r1(rayon);
	Point2D centre2(g1.centre);
	double r2(g1.rayon);
	if(geomod::intercercle(centre1, centre2, r1, r2))
	{
		cout << message::field_superposition(centre1.x, centre1.y, centre2.x, 
		                             centre2.y);
		exit(0);
	}
}

void Gisement::dessin()
{
	geomod::dessin_cercle(centre, rayon, 6, 10.0, true);
	
	double capacite_max(1000*(rayon/rayon_min)*(rayon/rayon_min));
	double arc_capacite((capacite_max - gisementR)*2*M_PI/capacite_max);
	geomod::dessin_arc(centre, rayon-15, 0, arc_capacite, 7, 30.0);
}
