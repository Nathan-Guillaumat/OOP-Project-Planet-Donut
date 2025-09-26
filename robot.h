#ifndef ROBOT_H
#define ROBOT_H

#include <string>
#include <vector>
#include "gisement.h"
#include "geomod.h"
#include "constantes.h"

class Robot 
{
	protected :
		unsigned uid;
		double dp;
		Point2D position;
		Point2D but;
		bool atteint;
		Point2D base;
		std::vector <Robot*> voisin;
		bool visited;
		bool etat;
		
	
	public :
		Robot();
		Robot (unsigned uid, double dp, double x, double y, double xb, double yb, 
			   bool atteint, Point2D);
		Robot (const std::string ligne, Point2D);
		virtual ~Robot(){}

		unsigned getuid();
		Point2D getposition();
		bool getvisited();
		void setvisited(bool vis);
		bool getatteint();
		void setatteint(bool att){atteint=att;}
		Point2D getbase(){return base;}
		bool getremote();
		double getdp();
		void setremote(bool rem);
		void setposition(Point2D);
		void setposition(double, double);
		void setbut(Point2D b){but.x=b.x; but.y = b.y;}
		void setdp(double d){dp=d;}
		
		void add_rob(Robot*);
		void suppr_voisin();
		void deplacement();
		void add_scaled_vector(double scaling);
		Point2D getbut();
		void setbut(double bx, double by);
		
		virtual void ecriture(std::string);
		virtual void dessin(int couleur, bool portee, bool liens);
		virtual bool maintenance();
		virtual void protocole_remote();
		virtual void protocole_autonomous();
		virtual char getnature();
		virtual bool test_maintenance();
		virtual void setretour(bool re);
		virtual bool getretour();
		virtual Gisement* getGisement();
		virtual void setGisement(Gisement*);
		virtual bool getAvecGisement();
		virtual void setAvecGisement(bool val);
		virtual double getdmax();
		virtual void settrouver(Gisement);
		virtual bool getfound();
		virtual void passage(double&, Point2D&, double&);
		virtual void setfound(bool found);
};


class Prospection : public Robot
{
	private :
		double dmax=maxD_prosp;
		bool retour;
		bool found;
		Point2D centreg;
		double rayong;
		double capaciteg;
		char nature ='P';

	public :
		Prospection (unsigned uid, double dp, double x, double y, double xb, 
					 double yb, bool atteint, bool retour, bool found, Point2D b, 
					 double xg, double yg, double rayong, double capaciteg);
		Prospection (unsigned uid, double dp, double x, double y, double xb, 
					 double yb, bool atteint, bool retour, bool found, Point2D);
		Prospection (std::string ligne, Point2D);
		virtual ~Prospection(){}
		
		void init(std::string line);
		void ecriture(std::string);
		bool maintenance();
		void protocole_remote();
		void protocole_autonomous();
		bool test_maintenance();
		void settrouver(Gisement);
		void setfound(bool);
		bool getfound();
		char getnature() override; 
		double getdmax();
		void passage(double&, Point2D&, double&);
};

class Forage : public Robot
{
	private :
		double dmax=maxD_forage;
		Gisement* gisement;
		char nature = 'F';
		bool avecGisement;

	public :
		Forage (unsigned uid, double dp, double x, double y, double xb, double yb, 
				bool atteint, Point2D);
		Forage(std::string ligne, Point2D);
		virtual ~Forage(){}
		
		void setGisement(Gisement* gis);
		void init(std::string line);
		void ecriture (std::string);
		void protocole_remote();
		void protocole_autonomous();
		char getnature();
		double getdmax();
		Gisement* getGisement();
		bool getAvecGisement();
		void setAvecGisement(bool val);
};


class Transport : public Robot
{
	private :
		double dmax=maxD_transp;
		Gisement* gisement;
		bool retour;
		char nature = 'T';
		bool avecGisement;

	public :
		Transport (unsigned uid, double dp, double x, double y, double xb, double yb, 
				   bool atteint, bool retour, Point2D);
		Transport(std::string ligne, Point2D);
		virtual ~Transport(){}
		void init(std::string line);
		void ecriture (std::string);
		bool maintenance();
		void protocole_remote();
		void protocole_autonomous();
		void setGisement(Gisement* gis);
		char getnature();
		bool getretour();
		void setretour(bool ret);
		double getdmax();
		Gisement* getGisement();
		bool getAvecGisement();
		void setAvecGisement(bool val);
		
};


class Communication : public Robot
{
	private :
		double dmax=maxD_com;
		char nature = 'C';

	public :
		Communication (unsigned uid, double dp, double x, double y, double xb, 
					   double yb, bool atteint, Point2D);
		Communication (std::string ligne, Point2D);
		virtual ~Communication(){}
		void init(std::string line);
		void ecriture  (std::string);
		void dessin(int couleur, bool portee, bool liens) override;
		void protocole_remote();
		void protocole_autonomous();
		char getnature();
		double getdmax();
};

#endif

