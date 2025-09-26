#include <iostream>
#include "simulation.h"
#include "gui.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>

int main(int argc, char* argv[]) {
   int a(1);
   auto app = Gtk::Application::create(a, argv, "org.gtkmm.example");

   if (argc==2)
   {
	   Gui fenetre(argv[1]);
	   fenetre.set_default_size(800, 800);
       fenetre.set_resizable(true);
       return app->run(fenetre);
   }
   if (argc==1) 
   {
	   Gui fenetre;
	   fenetre.set_default_size(800, 800);
	   fenetre.set_resizable(true);
	   return app->run(fenetre);
   }
}
