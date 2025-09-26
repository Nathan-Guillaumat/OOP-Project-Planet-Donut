#ifndef GISEMENT_H
#define GISEMENT_H

#include <string>
#include "geomod.h"

class Gisement
{
	private:
		Point2D centre;
		double rayon;
		double gisementR;
		bool etat;
		bool foreur;
	public:
		Gisement(double x, double y, double rayon, double capacite);
		Gisement(std::string ligne);
		Gisement();
		~Gisement() {}
		Gisement* operator=(Gisement*);
		Point2D getCentre() const;
		double getRayon() const;
		double getCapacite() const;
		void setCapacite(double cap);
		void actualiserR();
		void intersection(Gisement& g1);
		void dessin();
		void setEtat(bool e);
		bool getEtat();
		void setForeur(bool p);
		bool getForeur();
};

#endif
