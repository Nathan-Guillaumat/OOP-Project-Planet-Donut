#ifndef BASE_H
#define BASE_H

#include "geomod.h"
#include "robot.h"
#include "gisement.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>

class Base 
{
	private :
		Point2D position;
		double ressource;
		std::vector <Robot*> listerob ;
		std::vector <Robot*> listeremote;
		std::vector <Robot*> listeautonomous;
		std::vector <Robot*> listeconnect; // comprend même ceux qui ne sont pas de la base
		std::vector <Gisement*> gisements_connus;
		std::vector <bool> rob_remote;
		unsigned int ndP, ndF, ndT, ndC;
		int couleur;
		double const survie = 200;
		int com_count;
		std::vector<Point2D> network;
		int pro;

	public :
		Base (double x, double y, double ressource);
		Base (std::vector<std::string> ligne, int numBase = 0);
		Base(Base const& base2);
		~Base();

		void initrobot (std::vector <std::string> ainit);
		void ajouterrobot(Robot* rob);
		void intersection_base(Base* base2);
		void testrobcom();
		
		Point2D getposition();
		int get_ndP();
		int get_ndF();
		int get_ndT();
		int get_ndC();
		double get_ressource();
		
		void ecriture_rob(std::string);
		void dessin(bool portee, bool liens);
		std::vector<Robot*> get_rob(){return listerob;}
		void parcours(Robot*);
		void update_voisin(Base* &);
		void update_voisin();
		void connexion();
		void maintenance();
		void creation();
		void update_remote(std::vector<Gisement>& listeGisements);
		void update_autonomous(std::vector<Gisement>& gisements);
		void def_transport(std::vector<Gisement>& gisements, Robot* & robrem, int& compteurT);
		
		void creer_prosp();
		void creer_trans();
		void creer_for();
		void creer_com();
		void la5g();
		void suppr();
		
		bool test_extraction(Robot*);
		
		void p_att(Robot* &, std::vector<Gisement>&);
		void tremote_att(Robot* &, std::vector<Gisement>& , int& compteurT);
		void fremote_att(Robot* &, std::vector<Gisement>& );
		void fremote_att_base(Robot* &, std::vector<Gisement>& );
		void cremote_att(Robot* &);
		
		void premote_natt(Robot* &, std::vector<Gisement>&);
		void tremote_natt(Robot* &);
		void fremote_natt(Robot* &);
		void cremote_natt(Robot* &);
		
		void tsurbase(std::vector<Gisement>& gisements, Robot* & robrem, int& compteurT);
		void pauto_natt(Robot* &, std::vector<Gisement>&);
		void fauto_natt(Robot* &, std::vector<Gisement>&);
		void tauto_natt(Robot* &);
};

#endif

