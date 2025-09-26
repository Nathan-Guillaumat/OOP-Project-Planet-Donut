/*!
  \file    base.cc
  \authors CRON léo
		   GUILLAUMAT Nathan
  \date    Avril 2021
  \brief   Implémentation du module "base"
*/
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <fstream>
#include "base.h"
#include "robot.h"
#include "gisement.h"
#include "message.h"
#include "geomod.h"
#include "constantes.h"
using namespace std;

// Constructeur-Destructeur

Base::Base (double x, double y, double ressource = iniR)
	: position({x,y}), ressource(ressource)
	{}
	
Base::~Base()
{
	for (size_t i(0); i<listerob.size() ; ++i){delete listerob[i];}
	listerob.clear();
	gisements_connus.clear();
}

Base::Base (vector<string> ligne, int numBase) : couleur(numBase)
{
	istringstream data(ligne[0]);
	data >> position.x >> position.y >> ressource >> ndP >> ndF >> ndT >> ndC;
	position = geomod::normalisation(position);
	if(ndC<=0)
	{
		cout << message::missing_robot_communication(position.x, position.y); 
		exit(0); 
	}
	ligne.erase(ligne.begin());
	initrobot(ligne);
	int compteurx(0);
	int compteury(0);
	for(int i(0); i<49; ++i)
	{
		if(compteurx==7)
		{
			compteurx=0;
			++compteury;
		}
		network.push_back(geomod::normalisation({position.x + 
											(2*dim_max)/7 * compteurx,
											 position.y + 
											(2*dim_max)/7 * compteury}));
		++compteurx;
	}
	pro=1;
}

Base::Base(Base const& b2)
: position(b2.position), ressource(b2.ressource), listerob((b2.listerob).size()), 
  rob_remote(b2.rob_remote), ndP(b2.ndP), ndF(b2.ndF), ndT(b2.ndT), ndC(b2.ndC),
  couleur(b2.couleur)
  {
	for(size_t i(0); i<(b2.listerob).size(); ++i)
		listerob[i] = new Robot(*((b2.listerob)[i]));
  }


//initialisation de tout les robots

void Base::initrobot (vector <string> ainit)
{
	for (unsigned i(0) ; i<ndP ; ++i) 
		ajouterrobot(new Prospection (ainit[i], position));
	for (unsigned i(ndP) ; i<ndF+ndP ; ++i) 
		ajouterrobot(new Forage (ainit[i], position));
	for (unsigned i(ndF+ndP) ; i<ndF+ndP+ndT ; ++i) 
		ajouterrobot(new Transport (ainit[i], position));
	for (unsigned i(ndF+ndP+ndT) ; i<ndF+ndP+ndT+ndC ; ++i) 
		ajouterrobot(new Communication (ainit[i], position));
	testrobcom();
}

void Base::ajouterrobot(Robot* rob)
{
	if(rob!= nullptr)
	{
		for (size_t i(0); i< listerob.size(); ++i)
		{
			if (listerob[i]->getuid() == rob->getuid()) 
			{
				cout << message::identical_robot_uid(rob->getuid());
				exit(0);
			}
		}
		
	}
	listerob.push_back(rob);
}

// Tests

void Base::intersection_base(Base* base2)
{
	if(geomod::intercercle(position, base2->position, rayon_base, rayon_base))
	{
		cout << message::base_superposition(position.x, position.y, 
										   (base2->position).x, (base2->position).y);
		exit(0);
	}
}

void Base::testrobcom()
{
	for (size_t i(ndP+ndF+ndT); i< listerob.size(); ++i)
	{ 
		if ((listerob[i]-> getposition().x == position.x) && 
		    (listerob[i]-> getposition().y == position.y))
		return;
	}
	cout << message::missing_robot_communication(position.x, position.y);
	exit(0);
}

int Base::get_ndP() {return ndP;}
int Base::get_ndF() {return ndF;}
int Base::get_ndT() {return ndT;}
int Base::get_ndC() {return ndC;}
double Base::get_ressource() {return ressource;}

Point2D Base::getposition()
{
	return position;
}

void Base::ecriture_rob(string nom_fichier)
{
	ofstream fichier(nom_fichier, ios::app);
	fichier << "# Prospector" << endl; 
	fichier.close();
	for (unsigned i(0) ; i<ndP ; ++i) 
		listerob[i]->ecriture(nom_fichier);
	
	fichier.open(nom_fichier, ios::app);
	fichier << "# Foreur" << endl; 
	fichier.close();
	for (unsigned i(ndP) ; i<ndF+ndP ; ++i) 
		listerob[i]->ecriture(nom_fichier);
	
	fichier.open(nom_fichier, ios::app);
	fichier << "# Transport" << endl; 
	fichier.close();
	for (unsigned i(ndF+ndP) ; i<ndF+ndP+ndT ; ++i) 
		listerob[i]->ecriture(nom_fichier);
	
	fichier.open(nom_fichier, ios::app);
	fichier << "# Communication" << endl; 
	fichier.close();
	for (unsigned i(ndF+ndP+ndT) ; i<ndF+ndP+ndT+ndC ; ++i)
		listerob[i]->ecriture(nom_fichier);
}

void Base::dessin(bool portee, bool liens)
{
	couleur = couleur%6;
	geomod::dessin_cercle(position, rayon_base*10, couleur, 10.0, false);
	for (auto& ptr_rob : listerob)
		ptr_rob->dessin(couleur, portee, liens);
}

void Base::connexion()
{
	listeremote.clear();
	listeautonomous.clear();
	listeconnect.clear();
	for (auto& rob : listerob){rob->setremote(false);}
	for (auto& rob1 : listerob)
	{
		if (geomod::testegalite(rob1->getposition(), position))
		{
			parcours(rob1);
		}
	}
	for (auto& rob : listerob)
	{
		bool test(true);
		for (auto& robr : listeremote)
		{
			if (rob->getuid()==robr->getuid()) {test=false;} 
		}
		if(test){listeautonomous.push_back(rob);}
	}
}

void Base::parcours(Robot* rob)
{
	if (rob->getvisited()) return;
	rob->setvisited(true);
	if (rob->getbase().x == position.x && rob->getbase().y == position.y)
		{
			rob->setremote(true);
			listeremote.push_back(rob);
		}
	listeconnect.push_back(rob);
	for (auto& robots : listerob)
	{
	if (geomod::testdanscercle(robots->getposition(), rob->getposition(), rayon_comm)) 
		{
			parcours(robots);
		}
	}
}

void Base::suppr()
{
	for(auto& rob :listerob)
	{
		rob->suppr_voisin();
	}
}

void Base::update_voisin(Base* & base)
{
	for (auto& rob1 : listerob)
	{
		rob1->setvisited(true);
		for (auto& rob2 : listerob)
		{
		if (geomod::testdanscercle(rob2->getposition(),rob1->getposition(), rayon_comm) 
			&& rob2->getvisited() == false)
			{
				rob1->add_rob(rob2);
			}
		}
		for (auto& rob2 : base->listerob)
		{
		if(geomod::testdanscercle(rob2->getposition(), rob1->getposition(),rayon_comm))
			{rob1->add_rob(rob2);}
		}
		rob1->setvisited(false);
	}
}

void Base::update_voisin()
{
	for (auto& rob1 : listerob)
	{
		rob1->setvisited(true);
		for (auto& rob2 : listerob)
		{
		if(geomod::testdanscercle(rob1->getposition(),rob2->getposition(), rayon_comm) 
		   && rob2->getvisited() == false)
			{
				rob1->add_rob(rob2);
			}
		}
		rob1->setvisited(false);
	}
}

void Base::maintenance()
{
	for (auto& rob : listerob)
	{
		if (geomod::testegalite(position, rob->getposition()) && rob->getdp()>0 
			&& rob->maintenance())
			{
				ressource -= cost_repair*(rob->getdp());
				rob->setdp(0);
			}
	}
}

void Base::creation()
{
	unsigned limite(0);
	if (ressource > 1100 && ndC < 49 && limite != max_robots)
	{
		limite=3;
		creer_com();
		creer_com();
		creer_com();
	}
	if (ressource > survie && ndC < 4 && limite != max_robots)
	{
		++limite;
		creer_com();
	}
	//stratégie de la Base pour la création de robot foreur
	if (ressource > survie && gisements_connus.size() > ndF && limite != max_robots)
	{
		++limite;
		creer_for();
	}
	//stratégie de la Base pour la création de robot transport
	if (ressource > survie && ndT < ndF && ndT < 8 && limite != max_robots)
	{
		++limite;
		creer_trans();
	}
	//stratégie de la Base pour la création de robot prospecteur
	if (ressource > survie && ndP < 10 && limite != max_robots)
	{
		++limite;
		creer_prosp();
	}
}

void Base::creer_prosp()
{
	unsigned int i(0);
	for (auto& rob : listerob)
	{
		if (rob->getuid() > i) i = rob->getuid();
	}
	unsigned uidmax(i+1);
	ressource = ressource - cost_prosp;
	listerob.push_back(new Prospection(uidmax, 0, position.x, position.y, position.x, 
										position.y, true, false, false, position));
	ndP+=1;
}

void Base::creer_trans()
{
	unsigned int i(0);
	for (auto& rob : listerob)
	{
		if (rob->getuid() > i) i = rob->getuid();
	}
	unsigned uidmax(i+1);
	ressource -= cost_transp;
	listerob.push_back(new Transport(uidmax, 0, position.x, position.y, position.x, 
										position.y, false, true, position));
	ndT+=1;
}

void Base::creer_for()
{
	unsigned int i(0);
	for (auto& rob : listerob)
	{
		if (rob->getuid() > i) i = rob->getuid();
	}
	unsigned uidmax(i+1);
	ressource -= cost_forage;
	listerob.push_back(new Forage(uidmax, 0, position.x, position.y, position.x, 
									position.y, false, position));
	ndF+=1;
}

void Base::creer_com()
{
	unsigned int i(0);
	for (auto& rob : listerob)
	{
		if (rob->getuid() > i) i = rob->getuid();
	}
	unsigned uidmax(i+1);
	ressource -= cost_com;
	listerob.push_back(new Communication(uidmax, 0, position.x, position.y, position.x, 
											position.y, false, position));
	ndC+=1;
}

void Base::update_remote(std::vector<Gisement>& gisements)
{	//stratégie de la Base des robots connectés
	int compteurT(0);
	for (auto& robrem : listeremote)
	{
		bool stop(false);
		if(robrem->getdmax()<robrem->getdp()){stop=true;}
		if(!stop) {
			if (robrem->getatteint() == true)
			{
				if(robrem->getnature() == 'T')
					{tremote_att(robrem, gisements, compteurT);}
				if (robrem->getnature() == 'F'){fremote_att(robrem, gisements);}	
				if (robrem->getnature() == 'C'){}
				if (robrem->getnature() == 'P'){p_att(robrem, gisements);}
			} 
			else 
			{
				if (robrem->getnature() == 'P'){premote_natt(robrem, gisements);}
				if (robrem->getnature() == 'T'){tremote_natt(robrem);}
				if  (robrem->getnature() == 'F'){fremote_natt(robrem);}
				if (robrem->getnature() == 'C'){cremote_att(robrem);}
			}
		}
	}
}

void Base::p_att(Robot*& robrem, std::vector<Gisement>& gisements)
{
	Point2D nouveau_but({0,0});
	nouveau_but.x=robrem->getbut().x+ (rand() % 1000);
	nouveau_but.y=robrem->getbut().y+ (rand() % 1000);
	nouveau_but.x=geomod::normalisation(nouveau_but.x);
	nouveau_but.y=geomod::normalisation(nouveau_but.y);
	cout << nouveau_but.x << ";" << nouveau_but.y << endl;
	robrem->setbut({nouveau_but.x, nouveau_but.y});
	robrem->setatteint(false);

	for (auto& gis : gisements)
	{
		if(geomod::testdanscercle(robrem->getposition(), gis.getCentre(), 
			gis.getRayon()))
		{
			robrem->settrouver(gis);
		}
	}
	robrem->deplacement();
}

void Base::premote_natt(Robot* & robrem, std::vector<Gisement>& gisements)
{
	if (robrem->getdmax() - robrem->getdp() <= geomod::normeMin(robrem->getposition(), 
		robrem->getbase()))
		{robrem->setbut(robrem->getbase());}
	
	for (auto& gis : gisements)
	{
	 if(geomod::testdanscercle(robrem->getposition(), gis.getCentre(), gis.getRayon()))
		{robrem->settrouver(gis);}
	}
	if(robrem->getfound())
	{
		double capacite(0);
		Point2D centre({0,0});
		double rayon(0);
		Gisement* g;
		robrem->passage(capacite, centre, rayon);
		for(auto& gis : gisements)
		{
			if(geomod::testegalite(centre, gis.getCentre()))
				g = &gis;
		}
		bool verif(true);
		for(auto gis : gisements_connus)
		{
			if(geomod::testegalite(gis->getCentre(), g->getCentre()))
				verif = false;
		}
		if(verif) gisements_connus.push_back(g);
	}
	if (robrem->getdmax()+10 - robrem->getdp() <= 
		geomod::normeMin(robrem->getposition(), robrem->getbase()))
		robrem->setbut(robrem->getbase());
	robrem->deplacement();
}

void Base::tremote_att(Robot* &robrem, std::vector<Gisement>& gisements,int& compteurT)
{
	++compteurT;
	if(robrem->getAvecGisement())
	{
		if((robrem->getGisement())->getCapacite() - deltaR<0) 
			robrem->setAvecGisement(false);
	}
	def_transport(gisements, robrem, compteurT);
}

void Base::tremote_natt(Robot* & robrem)
{
	if(robrem->getAvecGisement())
	{
		if(geomod::testdanscercle(robrem->getposition(), 
		  (robrem->getGisement())->getCentre(), (robrem->getGisement())->getRayon()) 
		   && robrem->getretour()==false)
			robrem->setatteint(true);
		else
			robrem->deplacement();
	} 
	else 
	if(geomod::testegalite(robrem->getposition(), robrem->getbut()))
		robrem->setatteint(true);
	else robrem->deplacement();
}
		
void Base::fremote_att(Robot* &robrem, std::vector<Gisement>& gisements)
{
	if (geomod::testegalite(robrem->getposition(), position))
	{
		fremote_att_base(robrem, gisements);
	} else 
	if (!robrem->getAvecGisement())
	{
		for(auto& gis : gisements)
		{
			if(geomod::testdanscercle(robrem->getposition(), gis.getCentre(), 
			   gis.getRayon()))
			{
				Gisement* g;
				g = &gis;
				gis.setForeur(true);
				robrem->setGisement(g);
				robrem->setAvecGisement(true);
				gisements_connus.push_back(g);
			}
		}
		if(!robrem->getAvecGisement())
		{
			robrem->setbut(position);
			robrem->setatteint(false);
		}
	} else {
		if((robrem->getGisement())->getCapacite() - deltaR<0) 
			(robrem->getGisement())->setCapacite(0);
		else {
			if(test_extraction(robrem))
			{
				(robrem->getGisement())->setCapacite((robrem->getGisement())->
				 getCapacite() - deltaR);
			}
		}
	}
}

void Base::fremote_natt(Robot* & robrem)
{
	if(robrem->getAvecGisement())
	{
		if(geomod::testdanscercle(robrem->getposition(), 
		  (robrem->getGisement())->getCentre(), (robrem->getGisement())->getRayon()))
			robrem->setatteint(true);
		else
			robrem->deplacement();
	} else if(geomod::testegalite(robrem->getposition(), robrem->getbut()))
		robrem->setatteint(true);
	else robrem->deplacement();
}

void Base::fremote_att_base(Robot* &robrem, std::vector<Gisement>& gisements)
{
	if(gisements_connus.size() > 0) 
	{
		int numGis(0);
		for(size_t i(0); i < gisements_connus.size(); ++i)
		{
		    if(((gisements_connus[i]->getCapacite()>
				 gisements_connus[numGis]->getCapacite()) 
			     &&(gisements_connus[i]->getEtat() == false)) 
				 && gisements_connus[i]->getForeur() == false)
				numGis = i;
		}
		if ((gisements_connus[numGis]->getForeur())==false){
		robrem->setatteint(false);
		gisements_connus[numGis]->setForeur(true);
		robrem->setbut(gisements_connus[numGis]->getCentre());
		robrem->setGisement(gisements_connus[numGis]);
		robrem->setAvecGisement(true);
		}
	}
}

void Base::cremote_att(Robot* &robrem)
{	
	if ((robrem->getbut().x)== position.x && (robrem->getbut().y)== position.y){
	bool test(true);
	for(size_t i(0); i<network.size();++i)
	{
		for (auto& robcom : listerob)
		{
			if ((robcom->getbut().x) == network[i].x && 
				(robcom->getbut().y) == network[i].y && robcom->getnature() == 'C' && 
				 test && i!=0)
				{test=false;}
		}
		if (test && i!=0)
		{
			robrem->setbut(network[i]);
			robrem->setatteint(false);
		}
	}
}
	robrem->deplacement();
}

bool Base::test_extraction(Robot* foreur)
{
	for(auto& rob : listerob)
	{
		if ((rob->getnature() == 'T') && (!rob->getretour()) && rob->getAvecGisement() 
		&&(geomod::testdanscercle(rob->getposition(),
								 (foreur->getGisement())->getCentre(), 
								 (foreur->getGisement())->getRayon())))
		{
			rob->setretour(true);
			rob->setbut(position);
			rob->setatteint(false);
			return true;
		}
	}
	return false;
}

void Base::def_transport(vector<Gisement>& gisements, Robot* & robrem, int& compteurT)
{
	if (geomod::testegalite(robrem->getposition(), position))
	{
		if(gisements_connus.size() > 0)
		{
			tsurbase(gisements, robrem, compteurT);
		}
	} else if(!robrem->getAvecGisement())
	{
		for(auto& gis : gisements)
		{
			if(geomod::testdanscercle(robrem->getposition(), gis.getCentre(),
			                          gis.getRayon()))
			{
				robrem->setGisement(&gis);
				robrem->setAvecGisement(true);
			}
		} 
		if(!robrem->getAvecGisement())
		{
			robrem->setbut(position);
			robrem->setatteint(false);
		}
	}
}

void Base::tsurbase(std::vector<Gisement>& gisements, Robot* & robrem, int& compteurT)
{
	if(robrem->getAvecGisement()) ressource += deltaR;
	int numGis(0);
	for(size_t i(0); i < gisements_connus.size(); ++i)
	{
	  if((gisements_connus[i]->getCapacite() > gisements_connus[numGis]->getCapacite()) 
		  && (gisements_connus[i]->getEtat() == false) 
		  && gisements_connus[i]->getForeur() == true)
			 numGis = i;
	}
	if ((gisements_connus[numGis] ->getForeur())==true)
	{
		if(compteurT < (gisements_connus[numGis]->getCapacite())/deltaR)
		{
			robrem->setbut(gisements_connus[numGis]->getCentre());
			robrem->setGisement(gisements_connus[numGis]);
			robrem->setAvecGisement(true);
			robrem->setatteint(false);
			robrem->setretour(false);
		} 
		else
		{
			gisements_connus[numGis]->setEtat(true);
			compteurT = 0;
			robrem->setretour(false);
			bool verif(false);
			for (auto gis : gisements_connus)
			{
				if(gis->getCapacite() > deltaR && gis->getForeur())
					verif = true;
			}
			if(verif)
			def_transport(gisements, robrem, compteurT);
		}
	}
}
		
void Base::update_autonomous(std::vector<Gisement>& gisements)
{
	for (auto& rob_autonomous : listeautonomous)
	{
		bool stop(false);
		if(rob_autonomous->getdmax()<rob_autonomous->getdp()){stop=true;}
		if(!stop) {
			if (rob_autonomous->getatteint() == true)
			{
				if (rob_autonomous->getnature() == 'P')
					{p_att(rob_autonomous, gisements);}
				if (rob_autonomous->getnature() == 'T')
				{
					rob_autonomous->setbut(position);
					rob_autonomous->setatteint(false);
				}
				if (rob_autonomous->getnature() == 'C')
					{rob_autonomous->deplacement();}
			} 
			else 
			{
				if (rob_autonomous->getnature() == 'P')
					{pauto_natt(rob_autonomous, gisements);}
				rob_autonomous->deplacement();
				if(geomod::testegalite(rob_autonomous->getposition(), 
									   rob_autonomous->getbut()))
						rob_autonomous->setatteint(true);
				if (rob_autonomous->getnature() == 'T'){tauto_natt(rob_autonomous);}
				if  (rob_autonomous->getnature() == 'F')
					{fauto_natt(rob_autonomous, gisements);}
				if (rob_autonomous->getnature() == 'C')
					{rob_autonomous->deplacement();}
			}
		}
	}
}

void Base::pauto_natt(Robot* & rob_autonomous, std::vector<Gisement>& gisements)
{
	for (auto& gis : gisements)
	{
		if(geomod::testdanscercle(rob_autonomous->getposition(), gis.getCentre(), 
								  gis.getRayon()))
		{
			rob_autonomous->settrouver(gis);
		}
	}
	if (rob_autonomous->getdmax()-10 - rob_autonomous->getdp() <= 
		geomod::normeMin(rob_autonomous->getposition(), rob_autonomous->getbase()))
	rob_autonomous->setbut(rob_autonomous->getbase());
}

void Base::fauto_natt(Robot* & rob_autonomous, std::vector<Gisement>& gisements)
{
	if(rob_autonomous->getAvecGisement())
		{
			if(geomod::testdanscercle(rob_autonomous->getposition(), 
									 (rob_autonomous->getGisement())->getCentre(), 
									 (rob_autonomous->getGisement())->getRayon()))
				rob_autonomous->setatteint(true);
		} else {
			for(auto& gis : gisements)
			{
				if(geomod::testdanscercle(rob_autonomous->getposition(), 
										  gis.getCentre(), gis.getRayon()))
				{
					gis.setForeur(true);
					rob_autonomous->setGisement(&gis);
					rob_autonomous->setAvecGisement(true);
				}
			}
		}
		rob_autonomous->deplacement();
}

void Base::tauto_natt(Robot* & rob_autonomous)
{
	if(rob_autonomous->getAvecGisement())
	{
		if(geomod::testdanscercle(rob_autonomous->getposition(), 
		  (rob_autonomous->getGisement())->getCentre(), 
		   rob_autonomous->getGisement()->getRayon()))
		rob_autonomous->setatteint(true);
	}
	rob_autonomous->deplacement();
}


