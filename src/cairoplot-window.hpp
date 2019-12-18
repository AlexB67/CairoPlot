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
#include <gtkmm/builder.h>
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
#include <gtkmm/switch.h>
#include <gtkmm/sizegroup.h>
#include <gtkmm/separator.h>
#include <gtkmm/colorbutton.h>
#include <glibmm/i18n.h>

class CairoplotWindow : public Gtk::ApplicationWindow
{
public:
	CairoplotWindow(const Glib::RefPtr<Gtk::Application>& app);

private:
	Glib::RefPtr<Gtk::Application> m_app;
	Gtk::HeaderBar headerbar;
	Glib::RefPtr<Gio::Menu> winmenu;
	Gtk::MenuButton menubutton;
	CGraph::CairoGraph *graph;
	Gtk::Frame graphframe;
	Gtk::Frame graphcontrolframe;
	Gtk::Grid  maingrid;
	Gtk::Grid  controlgrid;
	Gtk::Grid  grid;
	Gtk::Separator *sep;
	Gtk::Switch *setscifiy;
	Gtk::Switch *setscifix;
	Gtk::Switch *hidelegend;
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
	Gtk::SpinButton *gravitational_constant;
	Gtk::SpinButton *start_velocity;
	Gtk::SpinButton *start_time;
	Gtk::ComboBoxText *selectgraph;
	Gtk::ComboBoxText *selecttheme;
	Gtk::ComboBoxText *selectlinestyle;
	Gtk::ComboBoxText *graphboxstyle;
	Gtk::ColorButton *linecolour;
	Glib::RefPtr<Gtk::SizeGroup> sizegroup;
	bool single_series = true;
	void make_plot();
	void create_header_and_menus();
	void close();
	void about();
};
