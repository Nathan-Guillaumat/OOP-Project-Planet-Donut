/*!
  \file    robot.cc
  \authors CRON Léo
		   GUILLAUMAT Nathan
  \date    Avril 2021
  \brief   Implémentation du module "robot"
*/
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "gisement.h"
#include "message.h"
#include "geomod.h"
#include "constantes.h"
#include "robot.h"
using namespace std;

// Constructeurs

Robot::Robot(){}

Robot::Robot(unsigned uid, double dp, double x, double y, double xb, double yb, 
			 bool atteint, Point2D b)
	: uid(uid), dp(dp), position({x,y}), but({xb,yb}), atteint(atteint), base(b)
	{
		visited=false;
		etat=false;
	}
	
Robot::Robot(const string ligne, Point2D b)
{
	std::istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> but.x >> but.y
		 >> boolalpha >> atteint;
	position = geomod::normalisation(position);
	b = geomod::normalisation(b);
	base.x=b.x;
	base.y=b.y;
	visited = false;
	etat = false;
}

Prospection::Prospection(unsigned uid, double dp, double x, double y, double xb, 
						 double yb, bool atteint, bool retour, bool found, Point2D b, double xg, 
						 double yg, double rayong, double capaciteg)
	: Robot(uid, dp, x, y, xb, yb, atteint,b), retour(retour), found(found), 
			centreg({xg, yg}), rayong(rayong), capaciteg(capaciteg)
	{}
	
Prospection::Prospection(unsigned uid, double dp, double x, double y, double xb, 
						 double yb, bool atteint, bool retour, bool found, Point2D b)
	: Robot(uid, dp, x, y, xb, yb, atteint, b), retour(retour), found(found)
	{}

Prospection::Prospection(const string ligne, Point2D b)
	: Robot()
{
	std::istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> but.x >> but.y 
		 >> boolalpha >> atteint >> boolalpha >> retour >> boolalpha >> found;
	position = geomod::normalisation(position);
	if(data >> centreg.x >> centreg.y >> rayong >> capaciteg)
	{
		centreg = geomod::normalisation(centreg);
	} else
	{
		centreg = {b.x,b.y};
		rayong = 0;
		capaciteg = 0;
	}
	b = geomod::normalisation(b);
	base.x=b.x;
	base.y=b.y;
	visited = false;
	etat = false;
}
	
	
Forage::Forage(unsigned uid, double dp, double x, double y, double xb, 
			   double yb, bool atteint, Point2D b)
	: Robot(uid, dp, x, y, xb, yb, atteint, b)
	{
		avecGisement = false;
	}
	
Forage::Forage(const string ligne, Point2D b)
	: Robot(ligne, b)
	{
		avecGisement = false;
	}


Transport::Transport(unsigned uid, double dp, double x, double y, double xb, 
					 double yb, bool atteint, bool retour, Point2D b)
	: Robot(uid, dp, x, y, xb, yb, atteint, b), retour(retour)
	{
		avecGisement = false;
	}
	
Transport::Transport(const string ligne, Point2D b)
	:Robot()
{
	istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> but.x >> but.y 
		 >> boolalpha >> atteint >> std::boolalpha >> retour ;
	position = geomod::normalisation(position);
	b = geomod::normalisation(b);
	base.x=b.x;
	base.y=b.y;
	avecGisement = false;
}


Communication::Communication (unsigned uid, double dp, double x, double y, 
							  double xb, double yb, bool atteint, Point2D b)
	: Robot(uid, dp, x, y, xb, yb, atteint, b)
	{position = geomod::normalisation({x,y});}

Communication::Communication(const string ligne, Point2D b)
	: Robot(ligne,b){}


// Lecture et initialisation des attributs si besoin pendant simulation

void Prospection::init(string ligne)
{
	istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> but.x >> but.y >> atteint
		 >> retour >> found;
	position = geomod::normalisation(position);
	if (data >> centreg.x >> centreg.y >> rayong >> capaciteg)
	{
		centreg = geomod::normalisation(centreg);
	}
}

void Forage::init(const string ligne) 
{
	std::istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> but.x >> but.y >> atteint;
	position = geomod::normalisation(position);
}

void Transport::init(const string ligne) 
{
	istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> base.x >> base.y >> atteint
		 >> retour;
	position = geomod::normalisation(position);
}

void Communication::init(const string ligne)
{
	istringstream data(ligne);
	data >> uid >> dp >> position.x >> position.y >> but.x >> but.y >> atteint;
	position = geomod::normalisation(position);
}

// Méthodes 

unsigned Robot:: getuid(){return uid;}

Point2D Robot::getposition(){return position;}

void Prospection::ecriture(std::string nom_fichier)
{
	ofstream fichier(nom_fichier, ios::app);
	fichier << uid << " " << dp << " " << position.x << " " << position.y << " "  
			<< but.x << " " << but.y << " " << atteint << " " << retour << " " 
			<< found << " " << noboolalpha;
	if (found)
	{
	fichier << centreg.x << " " << centreg.y << " " << rayong << " " 
			<< capaciteg << endl;
	}
	fichier << endl;
	fichier.close();
}

void Forage::ecriture(std::string nom_fichier )
{
	ofstream fichier(nom_fichier, ios::app);
	fichier << uid << " " << dp << " " << position.x << " " << position.y << " " 
			<< but.x << " " << but.y << " " << atteint << endl;
	fichier.close();
}
	
void Transport::ecriture(std::string nom_fichier)
{
	ofstream fichier(nom_fichier, ios::app);
	fichier << uid << " " << dp << " " << position.x << " " << position.y << " " 
			<< but.x << " " << but.y << " " << atteint << " " << retour << endl;
	fichier.close();
}

void Communication::ecriture(std::string nom_fichier)
{
	ofstream fichier(nom_fichier, ios::app);
	fichier << uid << " " << dp << " " << position.x << " " << position.y << " " 
			<< but.x << " " << but.y << " " << atteint << endl;
	fichier.close();
}

void Robot::dessin(int couleur, bool portee, bool liens)
{
	geomod::dessin_point(position, 5.0, couleur);
	if(portee) geomod::dessin_cercle(position, rayon_comm, 9, 3.0, false );
	if(liens) {
		for(auto rob : voisin) {
			geomod::dessin_ligne(position, rob->getposition(), 3.0, 8);
		}
	} 
}

void Communication::dessin(int couleur, bool portee, bool liens)
{
	geomod::dessin_point(position, 5.0, couleur);
	geomod::dessin_arc(position, 30, M_PI_4, 3*M_PI_4, couleur, 20);
	if(portee) geomod::dessin_cercle(position, rayon_comm, 9, 3.0, false );
	if(liens) {
		for(auto rob : voisin) {
			geomod::dessin_ligne(position, rob->getposition(), 3.0, 8);
		}
	}
}

void Robot::add_rob(Robot* rob)
{
	voisin.push_back(rob);
}

void Robot::suppr_voisin()
{
	voisin.clear();
}

bool Robot::getatteint(){return atteint;}

bool Robot::getvisited(){return visited;}

void Robot::setvisited(bool vis){visited=vis;}

bool Robot::getremote(){return etat;}

void Robot::setremote(bool rem){etat=rem;}

void Robot::setposition(Point2D pos){position=pos;}
void Robot::setposition(double x, double y)
{
	position.x=x;
	position.y=y;
}

void Robot::ecriture(std::string){}

bool Robot::maintenance(){return false;};

void Robot::protocole_remote(){}

void Robot::protocole_autonomous(){}

char Robot::getnature(){return 'R';}

bool Robot::test_maintenance(){return false;}

void Robot::setretour(bool re){}

bool Robot::getretour() { return 0; }

Gisement* Robot::getGisement() { return 0; }

void Robot::setGisement(Gisement*) {}

bool Robot::getAvecGisement() { return false; }

void Robot::setAvecGisement(bool val) {}

Point2D Robot::getbut(){return but;}

void Robot::setbut(double bx, double by){but.x=bx; but.y = by;}

double Robot::getdmax(){return 0;}

void Robot::settrouver(Gisement){}

bool Robot::getfound(){return false;}

void Robot::passage(double&, Point2D&, double&){}

void Robot::setfound(bool found){}


void Robot::deplacement()
{
	double norme(geomod::normeMin(position, but));
	if(norme <= deltaD) 
	{
		position = but;
		atteint = true;
		dp+=deltaD;
	}
	else add_scaled_vector(deltaD/norme);
}

void Robot::add_scaled_vector(double scaling)
{
	position.x += scaling*(geomod::normalisation(but.x-position.x));
	position.y += scaling*(geomod::normalisation(but.y-position.y));
	position.x=geomod::normalisation(position.x);
	position.y=geomod::normalisation(position.y);
	dp+=deltaD;
}

double Robot::getdp(){return dp;}

bool Prospection::maintenance()
{
	return true;
}

bool Transport::maintenance() 
{
	if(getAvecGisement())
	{
		if (2*geomod::normeMin(gisement->getCentre(), position) > dmax-dp)
		{
			return true;
		} return false;
	} else return false;
}

void Prospection::protocole_remote()
{
	if (dmax-dp +10 == geomod::normeMin(base, position))
	{
		but.x = base.x;
		but.y=base.y;
	}
}

void Prospection::protocole_autonomous()
{
	if (dmax-dp +10 == geomod::normeMin(base, position))
	{
		but.x = base.x;
		but.y=base.y;
	}
}

void Forage::protocole_remote(){}

void Forage::protocole_autonomous(){}

void Transport::protocole_remote(){}

void Transport::protocole_autonomous(){}

void Communication::protocole_remote(){}

void Communication::protocole_autonomous(){}

bool Prospection::test_maintenance()
{
	if (dmax-dp +10 <= geomod::normeMin(base, position))
	{
		return true;
	}
	return false;
}

void Prospection::settrouver(Gisement gis)
{
	if ( (gis.getCentre()).x != centreg.x or (gis.getCentre()).y != centreg.y)
	{
		found = true;
		atteint=false;
		retour=true;
		if(!etat)
		{
			but.x=geomod::normalisation(base.x);
			but.y=geomod::normalisation(base.y);
		}
		centreg.x = (gis.getCentre()).x;
		centreg.y = (gis.getCentre()).y;
		rayong=gis.getRayon();
		capaciteg=gis.getCapacite();
	}
}

void Prospection::passage(double& capacite, Point2D& centre, double& rayon)
{
	capacite=capaciteg;
	centre.x = centreg.x;
	centre.y = centreg.y;
	but.x=geomod::normalisation(position.x+5);
	but.y=geomod::normalisation(position.y+0.625);
	rayon=rayong;
	found=false;
	atteint=true;
}

void Prospection::setfound(bool f){found=f;}

bool Prospection::getfound(){return found;}

char Prospection::getnature() {return nature;} 

double Prospection::getdmax(){return dmax;}


void Transport::setGisement(Gisement* gis)
{
	gisement=gis;
}

char Transport::getnature(){return nature;}

bool Transport::getretour(){return retour;}

void Transport::setretour(bool ret){retour=ret;}

double Transport::getdmax(){return dmax;}

Gisement* Transport::getGisement() { return gisement; }

bool Transport::getAvecGisement() { return avecGisement; }

void Transport::setAvecGisement(bool val = 0) {avecGisement = val; }

		
void Forage::setGisement(Gisement* gis)
{
	gisement=gis;
}

char Forage::getnature(){return nature;}

double Forage::getdmax(){return dmax;}

Gisement* Forage::getGisement() { return gisement; }

bool Forage::getAvecGisement() { return avecGisement; }

void Forage::setAvecGisement(bool val = 0) {avecGisement = val; }

char Communication::getnature(){return nature;}

double Communication::getdmax(){return dmax;}
