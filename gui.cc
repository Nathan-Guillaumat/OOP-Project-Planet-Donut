#include <iostream>
#include <vector>
#include <string>
#include <cairomm/context.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "graphic_gui.h"
#include "gui.h"
#include "simulation.h"
#include "constantes.h"
using namespace std;

#define GTK_COMPATIBILITY_MODE

#ifdef GTK_COMPATIBILITY_MODE
namespace Gtk
{
  template<class T, class... T_Args>
  auto make_managed(T_Args&&... args) -> T*
  {
    return manage(new T(std::forward<T_Args>(args)...));
  }
}
#endif


static Simulation simulation; // --------la simulation


MyArea::MyArea(): 
_v_box(Gtk::ORIENTATION_VERTICAL, 10 ),
empty(false){}

MyArea::~MyArea(){}

void MyArea::clear()
{
  empty = true;
  refresh();
}

void MyArea::draw(bool toggle_range, bool toggle_link)
{
	portee = toggle_range;
	liens = toggle_link;
	empty = false;
	refresh();
}

void MyArea::draw()
{
	empty = false;
	refresh();
}

void MyArea::refresh()
{
  auto win = get_window();
  if(win)
  {
	  Gdk::Rectangle r(0,0, get_allocation().get_width(), get_allocation().get_height());
	  win->invalidate_rect(r,false);
  }
  tree_view_update();
}

void MyArea::setWParam(WP x) 
{ 
	prm_area_ref = x; 
	prm_area     = prm_area_ref;
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  tree_view_update();

  if(not empty)
  {  
	  adjust_frame();
	  
	  cr->translate(prm_area.WIDTH/2, prm_area.HEIGHT/2);
	  cr->scale(prm_area.WIDTH/(prm_area.XMAX - prm_area.XMIN), 
				-prm_area.HEIGHT/(prm_area.YMAX - prm_area.YMIN));
	  cr->translate(-(prm_area.XMIN + prm_area.XMAX)/2, -(prm_area.YMIN + prm_area.YMAX)/2);
	  
	  graphic_set_context(cr);
	  graphic::bord();
	  
	  simulation.dessin_gisements();
	  simulation.dessin_bases(portee, liens);
  }
  else
  {
	  cout << "Empty !" << endl;
  }

  return true;
}

void MyArea::adjust_frame()
{
	 Gtk::Allocation allocation = get_allocation();
	 const int width = allocation.get_width();
	 const int height = allocation.get_height();
	 
	 prm_area.HEIGHT = height;
	 prm_area.WIDTH  = width;
	 
	double new_aspect_ratio((double)width/height);
	if( new_aspect_ratio > prm_area_ref.ASP)
	{
		prm_area.YMAX = prm_area_ref.YMAX ;
		prm_area.YMIN = prm_area_ref.YMIN ;	

		double delta(prm_area_ref.XMAX - prm_area_ref.XMIN);
		double mid((prm_area_ref.XMAX + prm_area_ref.XMIN)/2);

		prm_area.XMAX = mid + 0.5*(new_aspect_ratio/prm_area_ref.ASP)*delta ;
		prm_area.XMIN = mid - 0.5*(new_aspect_ratio/prm_area_ref.ASP)*delta ;
	}
	else
	{
		prm_area.XMAX = prm_area_ref.XMAX ;
		prm_area.XMIN = prm_area_ref.XMIN ;
	  	  
		double delta(prm_area_ref.YMAX - prm_area_ref.YMIN);
		double mid((prm_area_ref.YMAX + prm_area_ref.YMIN)/2);

		prm_area.YMAX = mid + 0.5*(prm_area_ref.ASP/new_aspect_ratio)*delta ;
		prm_area.YMIN = mid - 0.5*(prm_area_ref.ASP/new_aspect_ratio)*delta ;		  	  
	}
}

int MyArea::ConvertX(double coord, double width)
{
 double prefactor = width / ( prm_area.XMAX - prm_area.XMIN ); 
 return (coord - prm_area.XMIN ) * prefactor;	
}

int MyArea::ConvertY(double coord, double height)
{
 double prefactor = height / ( prm_area.YMAX - prm_area.YMIN ); 
 return ( prm_area.YMAX - coord ) * prefactor;	
}

void MyArea::tableau()
{
  _v_box.add(_scrolled_window);
  _scrolled_window.set_size_request(-1, 200);  // replace 500 by -1 for the project
  _scrolled_window.add(_tree_view);

  _scrolled_window.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
							  Gtk::PolicyType::POLICY_AUTOMATIC);
  _scrolled_window.set_hexpand();

  _tree_view.append_column("nbP", _columns._col_nbP);
  _tree_view.append_column("nbF", _columns._col_nbF);
  _tree_view.append_column("nbT", _columns._col_nbT);
  _tree_view.append_column("nbC", _columns._col_nbC);
  _tree_view.append_column_numeric("Amount resource", _columns._col_resource, "%.2f");

  auto cell = Gtk::make_managed<Gtk::CellRendererProgress>();
  int cols_count = _tree_view.append_column("Mission completeness", *cell);
  auto progress_col = _tree_view.get_column(cols_count - 1);
  if(progress_col)
	progress_col->add_attribute(cell->property_value(),
								_columns._col_resource_percentage);
}


void MyArea::tree_view_update()
{
  Glib::RefPtr<Gtk::ListStore> ref_tree_model = Gtk::ListStore::create(_columns);
  _tree_view.set_model(ref_tree_model);

	vector<SimData> tab(simulation.get_ndB());
	simulation.donnees_tableau(tab);
	
	for(size_t i = 0 ;  i <tab.size() ; ++i)
	{
		
	  auto row = *(ref_tree_model->append());
	  row[_columns._col_nbP] = tab[i].ndP;
	  row[_columns._col_nbF] = tab[i].ndF;
	  row[_columns._col_nbT] = tab[i].ndT;
      row[_columns._col_nbC] = tab[i].ndC;
	  row[_columns._col_resource] = tab[i].ressource;
	  if(tab[i].ressource >= finR)
		row[_columns._col_resource_percentage] = 100;
	  else
		row[_columns._col_resource_percentage] = (100*tab[i].ressource)/(finR);
	}
}

//--------------------------------------

Gui::Gui(string fichier) :
  m_Box_General(Gtk::ORIENTATION_VERTICAL,10),
  m_Box_Butndraw(Gtk::ORIENTATION_HORIZONTAL,10),
  m_Box_Buttons(Gtk::ORIENTATION_VERTICAL, 10),
  m_Label_General ("General"),
  m_Label_Toggle ("Toggle Display"),
  m_Button_Exit("exit"),
  m_Button_Open("open"),
  m_Button_Save("save"),
  m_Button_Start("start"),
  m_Button_Step("step"),
  m_Button_Toggle_Link("toggle link"),
  m_Button_Toggle_Range("toggle range"),
  fichier(fichier),
  started(0), toggle_range(0), toggle_link(0)
{
	WP prm; 

   prm.XMIN = -dim_max;
   prm.XMAX = dim_max;
   prm.YMIN = -dim_max;
   prm.YMAX = dim_max;

   prm.HEIGHT = 400; 
   prm.ASP = ( prm.XMAX-prm.XMIN ) / ( prm.YMAX-prm.YMIN );
   prm.WIDTH = prm.HEIGHT*prm.ASP ;
   m_Area.setWParam(prm);
   
  simulation.lecture (fichier); // -------------------------- lecture du fichier

  set_title("Planet Donut");
  set_border_width(10);
    m_Area.tableau();
  add(m_Box_General);
  m_Area.draw(toggle_range, toggle_link);

  box();
  signal();

  show_all_children();
}

Gui::Gui() :
  m_Box_General(Gtk::ORIENTATION_VERTICAL,10),
  m_Box_Butndraw(Gtk::ORIENTATION_HORIZONTAL,10),
  m_Box_Buttons(Gtk::ORIENTATION_VERTICAL, 10),
  m_Label_General ("General"),
  m_Label_Toggle ("Toggle Display"),
  m_Button_Exit("exit"),
  m_Button_Open("open"),
  m_Button_Save("save"),
  m_Button_Start("start"),
  m_Button_Step("step"),
  m_Button_Toggle_Link("toggle link"),
  m_Button_Toggle_Range("toggle range"),
  started(0), toggle_range(0), toggle_link(0)
{
	WP prm;

   prm.XMIN = -dim_max;
   prm.XMAX = dim_max;
   prm.YMIN = -dim_max;
   prm.YMAX = dim_max;

   prm.HEIGHT = 400;
   prm.ASP = ( prm.XMAX-prm.XMIN ) / ( prm.YMAX-prm.YMIN );
   prm.WIDTH = prm.HEIGHT*prm.ASP ;
   m_Area.setWParam(prm);
  
  set_title("Planet Donut");
  set_border_width(10);
  m_Area.tableau();
  add(m_Box_General);
  
  box();
  signal();

  show_all_children();
}

void Gui::box()
{
  // Box 
  m_Box_General.pack_start(m_Box_Butndraw);
  m_Box_General.pack_start(m_Area._v_box, false, false);
  
  m_Box_Butndraw.pack_start(m_Box_Buttons, false, false);
  m_Area.set_size_request(200,200);
  m_Box_Butndraw.pack_start(m_Area);
  
  m_Box_Buttons.pack_start(m_Label_General, false ,false);
  m_Box_Buttons.pack_start(m_Separator1, false ,false);
  
  m_Box_Buttons.pack_start(m_Button_Exit, false ,false);
  m_Box_Buttons.pack_start(m_Button_Open, false ,false);
  m_Box_Buttons.pack_start(m_Button_Save, false ,false);
  m_Box_Buttons.pack_start(m_Button_Start, false ,false);
  m_Box_Buttons.pack_start(m_Button_Step, false ,false);
  
  m_Box_Buttons.pack_start(m_Label_Toggle, false ,false);
  m_Box_Buttons.pack_start(m_Separator2, false ,false);
  
  m_Box_Buttons.pack_start(m_Button_Toggle_Link, false ,false);
  m_Box_Buttons.pack_start(m_Button_Toggle_Range, false ,false);
}

void Gui::signal()
{
  // Signal handler
  m_Button_Exit.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_exit) );

  m_Button_Open.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_open) );
              
  m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_save) );
    
  m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_start) );
              
  m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_step) );
              
  m_Button_Toggle_Link.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_link) );
  
  m_Button_Toggle_Range.signal_clicked().connect(sigc::mem_fun(*this,
              &Gui::on_button_clicked_range) );
             
  Glib::signal_idle().connect( sigc::mem_fun(*this, &Gui::on_idle) );
}

Gui::~Gui(){}

void Gui::on_button_clicked_exit()
{
  cout << "Je quitte" << endl;
  exit(EXIT_FAILURE); 
}

void Gui::on_button_clicked_open()
{
  cout << "J'ouvre un fichier" << endl;
   Gtk::FileChooserDialog dialog("Please choose a file",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  int result = dialog.run();

  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Open clicked." << std::endl;

      std::string filename = dialog.get_filename();
      simulation.lecture(filename);
      std::cout << "File selected: " <<  filename << std::endl;
      m_Area.draw(toggle_range, toggle_link);
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

void Gui::on_button_clicked_save()
{
  cout << "Je sauvegarde un fichier" << endl;
   Gtk::FileChooserDialog dialog("Please save a file",
          Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_transient_for(*this);

  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Save", Gtk::RESPONSE_OK);

  int result = dialog.run();

  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      std::cout << "Save clicked." << std::endl;

      std::string filename = dialog.get_filename();
      simulation.ecriture(filename);
      std::cout << "File selected: " <<  filename << std::endl;
      break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
  cout << "Je sauvegarde" << endl;
}

void Gui::on_button_clicked_start()
{
  if (started) m_Button_Start.set_label("start");
  else m_Button_Start.set_label("stop");
  cout << "Je commence la simulation idle" << endl;
  started=!started;
}
bool Gui::on_key_press_event(GdkEventKey * key_event)
{
	if(key_event->type == GDK_KEY_PRESS)
	{
		switch(gdk_keyval_to_unicode(key_event->keyval))
		{
			case 's':
				cout << "s key pressed !" << endl;
				started = !started ;
				return true;

			case '1':
				cout << "digit 1 key pressed !" << endl;
				simulation.boucle();
				m_Area.clear();
				m_Area.draw();
				return true;
			case 'q':
				cout << "Quit" << endl;
				exit(0);
				break;
		}
	}
	
	return Gtk::Window::on_key_press_event(key_event);
}

void Gui::on_button_clicked_step()
{
  simulation.boucle();
	m_Area.clear();
    m_Area.draw();
}

void Gui::on_button_clicked_link()
{
  cout << "J'affiche les liens" << endl;
  m_Area.clear();
  toggle_link = !toggle_link;
  m_Area.draw(toggle_range, toggle_link);
}

void Gui::on_button_clicked_range()
{
  cout << "J'affiche la portée " << endl;
  m_Area.clear();
  toggle_range = !toggle_range;
  m_Area.draw(toggle_range, toggle_link);
}

bool Gui::on_idle()
{
  if(started)
  {
	simulation.boucle();
	m_Area.clear();
    m_Area.draw();
  }
  return true;  
}
