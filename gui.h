#ifndef GUI_H
#define GUI_H

#include <gtkmm/drawingarea.h>
#include <gtkmm.h>
#include <vector>
#include <string>

struct WP // window parameters
{
   double XMIN; // field parameters
   double XMAX;
   double YMIN;
   double YMAX;
   int HEIGHT;
   int WIDTH;
   double ASP;
};

class MyArea : public Gtk::DrawingArea
{
public:
	MyArea();
	virtual ~MyArea();
  
	void clear();
	void draw(bool toggle_range, bool toggle_link);
	void draw();
	Gtk::Box _v_box;
	void setWParam(WP x);
	void tableau();
 
protected:

	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
	int ConvertX(double coord, double width); 
	int ConvertY(double coord, double height);
	void tree_view_update();
	void on_button_clicked();
	Gtk::Button m_button;
  
	class Model_columns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		Model_columns()
		{
			add(_col_id);
			add(_col_nbP);
			add(_col_nbF);
			add(_col_nbT);
			add(_col_nbC);
			add(_col_resource);
			add(_col_resource_percentage);
		}

		Gtk::TreeModelColumn<int> _col_id;
		Gtk::TreeModelColumn<int> _col_nbP;
		Gtk::TreeModelColumn<int> _col_nbF;
		Gtk::TreeModelColumn<int> _col_nbT;
		Gtk::TreeModelColumn<int> _col_nbC;
		Gtk::TreeModelColumn<double> _col_resource;
		Gtk::TreeModelColumn<int> _col_resource_percentage;
	};

	Model_columns _columns;
	Gtk::ScrolledWindow _scrolled_window;
	Gtk::TreeView _tree_view;
  
private:
	bool empty;
	bool portee;
	bool liens;
	void refresh();
	WP prm_area_ref;
	WP prm_area; 
	void adjust_frame();
};


class Gui : public Gtk::Window
{
public:
  Gui(std::string fichier);
  Gui();
  virtual ~Gui();
  bool test_fichier();
 
protected:
  //Button and Signal handlers:
  void on_button_clicked_exit();
  void on_button_clicked_open();
  void on_button_clicked_save();
  void on_button_clicked_start();
  void on_button_clicked_step();
  void on_button_clicked_link();
  void on_button_clicked_range();
  bool on_key_press_event(GdkEventKey * key_event);

  MyArea m_Area;
  Gtk::Box m_Box_General, m_Box_Butndraw, m_Box_Buttons, m_Box_Draw;
  Gtk ::Label m_Label_General ,m_Label_Toggle;
  Gtk::Button m_Button_Exit;
  Gtk::Button m_Button_Open;
  Gtk::Button m_Button_Save;
  Gtk::Button m_Button_Start;
  Gtk::Button m_Button_Step;
  Gtk::Button m_Button_Toggle_Link;
  Gtk::Button m_Button_Toggle_Range;
  Gtk ::Separator m_Separator1, m_Separator2 ; 
  void signal();
  void box();
  
  bool on_idle();
  std::string fichier;
  std::string fichier_string;
  
private:
	bool started;
	bool toggle_range;
	bool toggle_link;
};
#endif

