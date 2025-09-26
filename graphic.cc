#include <iostream>
#include "simulation.h"
#include "graphic_gui.h"

using namespace std;

namespace
{
	const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);
	const array<array<double,3>,10> couleurTab={{
		{1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {1,0,1}, {0,1,1},
		{0,0,0}, {1,1,1}, {0.5,0,0.5}, {0.5,0.5,0.5} }};
	enum Couleur {ROUGE, VERT, BLEU, JAUNE, MAGENTA, CYAN,
				  NOIR, BLANC, VIOLET, GRIS};
};
													 
void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr)
{
	ptcr = &cr;
}

void graphic::arc(double centre_x, double centre_y, double rayon, double angle_debut, 
                 double angle_fin, int couleur, double epaisseur, bool remplissage)
{
	(*ptcr)->set_line_width(epaisseur);
	(*ptcr)->set_source_rgb(couleurTab[couleur][0],couleurTab[couleur][1],
							couleurTab[couleur][2]);
	(*ptcr)->arc(centre_x, centre_y, rayon, angle_debut, angle_fin);
	if(remplissage) {
		(*ptcr)->close_path();
		(*ptcr)->fill_preserve();
	}
	(*ptcr)->stroke();
}

void graphic::point(double centre_x, double centre_y, int couleur)
{
	(*ptcr)->set_line_width(10.0);
	(*ptcr)->set_source_rgb(couleurTab[couleur][0],couleurTab[couleur][1],
							couleurTab[couleur][2]);
	(*ptcr)->arc(centre_x, centre_y, 5, 0, 2*M_PI);
	(*ptcr)->stroke();
}

void graphic::ligne(double centreD_x, double centreD_y, double centreF_x, double centreF_y, double epaisseur, int couleur)
{
	(*ptcr)->set_line_width(epaisseur);
	(*ptcr)->set_source_rgb(couleurTab[couleur][0],couleurTab[couleur][1],
							couleurTab[couleur][2]);
	(*ptcr)->move_to(centreD_x, centreD_y);
	(*ptcr)->line_to(centreF_x, centreF_y);
	(*ptcr)->stroke();
}

void graphic::bord()
{
	(*ptcr)->set_line_width(3.0);
	(*ptcr)->set_source_rgb(couleurTab[GRIS][0],couleurTab[GRIS][1],
							couleurTab[GRIS][2]);
	(*ptcr)->move_to(-1000, -1000);
	(*ptcr)->line_to(1000, -1000);
	(*ptcr)->line_to(1000, 1000);
	(*ptcr)->line_to(-1000, 1000);
	(*ptcr)->line_to(-1000, -1000);
	(*ptcr)->stroke();
}
