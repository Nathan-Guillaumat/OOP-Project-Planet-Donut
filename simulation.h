#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include <fstream>
#include "base.h"
#include "gisement.h"

struct SimData
{
  unsigned ndP;
  unsigned ndF;
  unsigned ndT;
  unsigned ndC;
  double ressource;
};


class Simulation 
{
	private:
		std::vector<Gisement> listeGisements;
		std::vector<Base*> listeBases;
		std::vector<std::string> tabLigne;
		std::string ligne;
		int nbB;
		int nbG;
		void decodage();
		void verifGisementBase();
		void initbase(int& etat, int& compteurBase, int& nbB);
		
	public:
		void lecture(char*);
		void lecture(std::string);
		void ecriture(std::string);
		void boucle();
		void connexion();
		
		void donnees_tableau (std::vector <SimData> &tab);
		int get_ndB();
		void dessin_gisements();
		void dessin_bases(bool portee, bool liens);
		~Simulation();
};

#endif
