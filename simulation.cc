/*!
  \file    simulation.cc
  \authors CRON léo
		   GUILLAUMAT Nathan
  \date    Avril 2021
  \brief   Implémentation du module "simulation"
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "simulation.h"
#include "message.h"
#include "gisement.h"
#include "base.h"
#include "geomod.h"
#include "constantes.h"

using namespace std;

Simulation::~Simulation()
{
	for(size_t i(0); i<listeBases.size() ; ++i) delete listeBases[i];
}

void Simulation::lecture(char* nom_fichier)
{
	listeGisements.clear();
    for (size_t i(0); i<listeBases.size() ; ++i) {delete listeBases[i];}
    listeBases.clear();
	geomod::setMaxsize(dim_max);
	ifstream fichier(nom_fichier);
	while(getline(fichier >> ws, ligne))
	{
		if(ligne[0]=='#') continue;
		if(ligne[0]=='\n') continue;
		decodage();
	}
	fichier.close();
	verifGisementBase();
	cout << message::success();
}

void Simulation::lecture(string nom_fichier)
{
	listeGisements.clear();
    for (size_t i(0); i<listeBases.size() ; ++i) {delete listeBases[i];}
    listeBases.clear();
	geomod::setMaxsize(dim_max);
	ifstream fichier(nom_fichier);
	while(getline(fichier >> ws, ligne))
	{
		if(ligne[0]=='#') continue;
		if(ligne[0]=='\n') continue;
		decodage();
	}
	fichier.close();
	verifGisementBase();
	cout << message::success();
}

void Simulation::ecriture(string nom_fichier)
{
	ofstream fichier(nom_fichier);
	fichier << "#" << nom_fichier << " MODIFIEEEEEE" << endl;
	//Gisements
	fichier << "# nb Gisement" << endl << nbG << endl;
	fichier << "# Gisement : "<< endl;
	for (auto tab : listeGisements)
	{
		fichier << (tab.getCentre()).x << " " << (tab.getCentre()).y << " " 
				<< tab.getRayon() << " " << tab.getCapacite() << endl;
	}
	fichier << endl;
	//Bases
	fichier << "# nb Base " << endl << nbB << endl;
	fichier <<"# Bases " << endl;
	fichier.close();
	for (auto base : listeBases)
	{
		ofstream fichier2(nom_fichier, ios::app);
		fichier2 << (base->getposition()).x << " " << (base->getposition()).y << " " 
				<< base->get_ressource() << " " << base->get_ndP() << " " << base->get_ndF() 
				<< " " << base->get_ndT() << " " << base->get_ndC() << endl;
		fichier2.close();
		base->ecriture_rob(nom_fichier);
		
		ofstream fichier3(nom_fichier, ios::app);
		fichier3 << endl; 
		fichier3.close();
	}
}

void Simulation::decodage()
{
	enum Etat_lecture {NB_GISEMENT, INIT_GISEMENT, NB_BASE, INIT_BASE};
	istringstream data(ligne);
	static int etat(NB_GISEMENT);
	static int compteurG(0);
	static int compteurBase(0);
	switch(etat)
	{
	case NB_GISEMENT: {
		data >> nbG;
		if (nbG!=0)
			etat = INIT_GISEMENT;
		else etat = NB_BASE;
		break;
	}
	case INIT_GISEMENT: {
		Gisement g1(ligne);
		if (nbG >0){
		for(auto gisement : listeGisements)
			g1.intersection(gisement);
		listeGisements.push_back(g1);
		++compteurG;}
		if(compteurG == nbG){
			etat = NB_BASE;
			compteurG=0;
		}
		break;
	}
	case NB_BASE: {
		data >> nbB;
		etat = INIT_BASE;
		break;
	}
	case INIT_BASE: {
		initbase(etat, compteurBase, nbB);
		break;
	}
	}
}

void Simulation::initbase(int& etat, int& compteurBase, int& nbB)
{
	enum Etat_lectureB {BASE, ROBOT};
		istringstream data(ligne);
		double ndP, ndF, ndT, ndC;
		static int totRob(0);
		static int etatB(BASE);
		static int compteurRob(0);
		switch(etatB)
		{
		case BASE: {
			++compteurBase;
			data >> ndP >> ndP >> ndP >> ndP >> ndF >> ndT >> ndC;
			totRob = ndP + ndF + ndT + ndC;
			if (ndC == 0) {
				tabLigne.push_back(ligne);
				Base b1(tabLigne);}
			tabLigne.push_back(ligne);
			etatB = ROBOT;
			break;
		}
		case ROBOT: {
			tabLigne.push_back(ligne);
			++compteurRob;
			if(compteurRob == totRob){
				Base b1(tabLigne);
				for(auto base : listeBases)
					b1.intersection_base(base);
				listeBases.push_back(new Base(tabLigne, listeBases.size()));
				tabLigne.clear();
				etatB = BASE;
				compteurRob=0;
				totRob=0;
				if (compteurBase == nbB){
					compteurBase = 0;
					etat = 0;}
			}
			break;
		}
		}
}

void Simulation::verifGisementBase()
{
	for(auto gisement : listeGisements)
	{
		for(auto base : listeBases)
		{
			if (geomod::intercercle(base->getposition(), gisement.getCentre(), 
									rayon_base, gisement.getRayon()))
			{
				cout << message::base_field_superposition((base->getposition()).x,
						(base->getposition()).y, (gisement.getCentre()).x,
				(gisement.getCentre()).y);
				exit(0);
			}
		}
	}
}

void Simulation::donnees_tableau (vector <SimData> &tab)
{
	for (size_t i(0); i<listeBases.size() ; ++i)
	{
		tab[i].ndP=listeBases[i]->get_ndP();
		tab[i].ndF=listeBases[i]->get_ndF();
		tab[i].ndT=listeBases[i]->get_ndT();
		tab[i].ndC=listeBases[i]->get_ndC();
		tab[i].ressource=listeBases[i]->get_ressource();
	}
}

int Simulation::get_ndB() {return nbB;}

void Simulation::dessin_gisements()
{
	for(auto gis : listeGisements)
		gis.dessin();
}

void Simulation::dessin_bases(bool portee, bool liens)
{
	for(auto base : listeBases)
		base->dessin(portee, liens);
}

void Simulation::boucle()
{
	for (size_t i(0); i<listeBases.size(); ++i)
	{
		listeBases[i]->suppr();
		if(listeBases.size() == 1) listeBases[i]->update_voisin();
		for(size_t j(0); j<listeBases.size(); ++j)
		{
			if(i!=j) listeBases[i]->update_voisin(listeBases[j]); 
		}
		listeBases[i]->connexion();
		listeBases[i]->maintenance();
		listeBases[i]->creation();
		listeBases[i]->update_remote(listeGisements);
		listeBases[i]->update_autonomous(listeGisements);
	}
	for (size_t i(0); i< listeBases.size() ; ++i) 
	{
		if ((listeBases[i]->get_ressource())<= 0)
		{
			delete listeBases[i];
			listeBases.erase(listeBases.begin()+i);
		}
	}
}
