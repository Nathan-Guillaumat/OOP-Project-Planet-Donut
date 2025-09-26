#ifndef GRAPHIC_H
#define GRAPHIC_H

namespace graphic
{	
	void arc(double centre_x, double centre_y, double rayon, 
					 double angle_debut, double angle_fin, int couleur,
					 double epaisseur, bool remplissage);

	void point(double centre_x, double centre_y, int couleur);
	
	void ligne(double centreD_x, double centreD_y, double centreF_x, double centreF_y, double epaisseur, int couleur);
	
	void bord();
};

#endif
