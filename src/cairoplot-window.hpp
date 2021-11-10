/* cairoplot-window.h
 *
 * Copyright 2019 AlexB67
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "plotter.hpp"
//#include <gtkmm/builder.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/menubutton.h>
#include <giomm/menu.h>
#include <gtkmm/entry.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/switch.h>
#include <gtkmm/sizegroup.h>
#include <gtkmm/separator.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/colorbutton.h>
#include <gtkmm/stackswitcher.h>
#include <gtkmm/scale.h>
#include <glibmm/i18n.h>


class CairoplotWindow : public Gtk::ApplicationWindow
{
public:
	CairoplotWindow(const Glib::RefPtr<Gtk::Application>& app);

private:

	Glib::RefPtr<Gtk::Application> m_app;
	Gtk::StackSwitcher switcher;
	Gtk::Stack stack;
	Gtk::HeaderBar headerbar;
	Glib::RefPtr<Gio::Menu> winmenu;
	Gtk::MenuButton menubutton;
	CGraph::CairoGraph *graph{nullptr};
	CGraph::CairoGraph *graph2{nullptr};
	Gtk::Frame graphframe;
	Gtk::Frame graphframe2;
	Gtk::Frame graphcontrolframe;
	Gtk::Frame graphcontrolframe2;
	Gtk::Grid  maingrid;
	Gtk::Grid  controlgrid;
	Gtk::Grid  grid;
	Gtk::Grid  maingrid2;
	Gtk::Grid  grid2;
	Gtk::Grid  controlgrid2;
	Gtk::Scale scale2;
	Gtk::Separator *sep;
	Gtk::Separator sep2;
	Gtk::Switch *setscifiy;
	Gtk::Switch *setscifix;
	Gtk::Switch *hidelegend;
	Gtk::Switch *preferdark;
	Gtk::Label headerbarlabel;
	Gtk::Label selectgraphlabel;
	Gtk::Label selectthemelabel;
	Gtk::Label start_velocity_label;
	Gtk::Label gravitational_constant_label;
	Gtk::Label *start_timelabel;
	Gtk::Label *setscifiylabel;
	Gtk::Label *setscifixlabel;
	Gtk::Label *linecolourlabel;
	Gtk::Label *selectlinestylelabel;
	Gtk::Label *hidelegend_label;
	Gtk::Label *graphboxstyle_label;
	Gtk::Label  scalelabel2;
	Gtk::Label  animate_label2;
	Gtk::Button button_start2;
	Gtk::Button button_stop2;
	Gtk::Button button_faster;
	Gtk::Button button_slower;
	Gtk::Button button_reset;
	Gtk::SpinButton *gravitational_constant;
	Gtk::SpinButton *start_velocity;
	Gtk::SpinButton *start_time;
	Gtk::ComboBoxText *selectgraph;
	Gtk::ComboBoxText *selecttheme;
	Gtk::ComboBoxText *selectlinestyle;
	Gtk::ComboBoxText *graphboxstyle;
	Gtk::ColorButton *linecolour;
	std::unique_ptr<Gtk::AboutDialog> aboutdialog;
	std::unique_ptr<Gtk::MessageDialog> help_dialog;
	Glib::RefPtr<Gtk::SizeGroup> sizegroup;
	sigc::connection scalecon2;
	sigc::connection timer2;
	int interval{30};
	bool single_series{true};
	bool is_animation{false};
	bool stop_was_clicked{false};
	void make_plot();
	bool make_plot2();
	void create_header_and_menus();
	void on_animation_clicked();
	void on_stop_clicked();
	void on_faster_clicked();
	void on_slower_clicked();
	void animation_stop_func();
	void on_reset_clicked();
	void close();
	void about();
};
