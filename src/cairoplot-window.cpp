/* cairoplot-window.cpp
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

#include "cairoplot-window.hpp"
#include <iostream>
#include <gtkmm/settings.h>
#include <gtkmm/messagedialog.h>

CairoplotWindow::CairoplotWindow(const Glib::RefPtr<Gtk::Application>& app)
	: Glib::ObjectBase("CairoplotWindow")
	, Gtk::ApplicationWindow()
	, m_app(app)
{	
	Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme().set_value(true);

	create_header_and_menus();

	graphframe.set_label(_("Graph"));
	graphframe.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	graphframe.set_hexpand(true);

	graph = Gtk::make_managed<CGraph::CairoGraph>();
	selectgraph = Gtk::make_managed<Gtk::ComboBoxText>();
	selectgraph->set_margin_top(10);
	selecttheme = Gtk::make_managed<Gtk::ComboBoxText>();
	start_velocity = Gtk::make_managed<Gtk::SpinButton>();
	gravitational_constant = Gtk::make_managed<Gtk::SpinButton>();
	start_time = Gtk::make_managed<Gtk::SpinButton>();
	sep = Gtk::make_managed<Gtk::Separator>();
	setscifiy = Gtk::make_managed<Gtk::Switch>();
	setscifiy->set_halign(Gtk::ALIGN_END);
	setscifiy->set_valign(Gtk::ALIGN_CENTER);
	setscifiy->set_active(false);
	setscifiylabel = Gtk::make_managed<Gtk::Label>("Y axis exp.", Gtk::ALIGN_END);
	setscifix = Gtk::make_managed<Gtk::Switch>();
	setscifix->set_halign(Gtk::ALIGN_END);
	setscifix->set_valign(Gtk::ALIGN_CENTER);
	setscifix->set_active(false);
	hidelegend = Gtk::make_managed<Gtk::Switch>();
	hidelegend->set_halign(Gtk::ALIGN_END);
	hidelegend->set_valign(Gtk::ALIGN_CENTER);
	hidelegend->set_active(false);
	hidelegend_label = Gtk::make_managed<Gtk::Label>("Hide legend", Gtk::ALIGN_END);
	setscifixlabel = Gtk::make_managed<Gtk::Label>("X axis exp.", Gtk::ALIGN_END);
	linecolourlabel = Gtk::make_managed<Gtk::Label>("Line colour", Gtk::ALIGN_END);
	start_timelabel = Gtk::make_managed<Gtk::Label>("", Gtk::ALIGN_END);
	graphboxstyle_label = Gtk::make_managed<Gtk::Label>("Graph style", Gtk::ALIGN_END);
	linecolour = Gtk::make_managed<Gtk::ColorButton>();
	selectlinestylelabel = Gtk::make_managed<Gtk::Label>("Line style", Gtk::ALIGN_END);
	selectlinestyle = Gtk::make_managed<Gtk::ComboBoxText>();
	graphboxstyle = Gtk::make_managed<Gtk::ComboBoxText>();

	graphframe.add(graph->create_graph());
	graph->get_graph_grid().set_border_width(10);
	graph->set_title(_("Displacement versus time"));
	
	selectlinestyle->insert(0, "Solid line");
	selectlinestyle->insert(1, "Dashed line");
	selectlinestyle->insert(2, "Circles");
	selectlinestyle->insert(3, "Dots");

	graphboxstyle->insert(0, "Box and grid");
	graphboxstyle->insert(1, "Axes and grid");
	graphboxstyle->insert(2, "Box only");
	graphboxstyle->insert(3, "Axes only");

	graphboxstyle->signal_changed().connect([this](){
		switch(graphboxstyle->get_active_row_number())
		{
			case 0:
				graph->set_graph_box_style(CairoGraphBoxStyle::BOX_GRID);
				break;
			case 1:
				graph->set_graph_box_style(CairoGraphBoxStyle::AXES_GRID);
				break;
			case 2:
				graph->set_graph_box_style(CairoGraphBoxStyle::BOX_ONLY);
				break;
			case 3:
				graph->set_graph_box_style(CairoGraphBoxStyle::AXES_ONLY);
				break;
			default:
				break;
		}

		graph->update_graph();
	});



	selectlinestyle->signal_changed().connect([this](){

		switch(selectlinestyle->get_active_row_number())
		{
			case 0:
				if ( single_series) graph->set_line_style(0, CairoGraphLineStyle::SOLID_LINE);
				else for (size_t i = 0; i < 3; ++i) graph->set_line_style(i, CairoGraphLineStyle::SOLID_LINE);
				break;
			case 1:
				if ( single_series) graph->set_line_style(0, CairoGraphLineStyle::DASHED_LINE);
				else for (size_t i = 0; i < 3; ++i) graph->set_line_style(i, CairoGraphLineStyle::DASHED_LINE);
				break;
			case 2:
				if ( single_series) graph->set_line_style(0, CairoGraphLineStyle::CIRCLE);
				else for (size_t i = 0; i < 3; ++i) graph->set_line_style(i, CairoGraphLineStyle::CIRCLE);
				break;
			case 3:
				if ( single_series) graph->set_line_style(0, CairoGraphLineStyle::DOTS);
				else for (size_t i = 0; i < 3; ++i) graph->set_line_style(i, CairoGraphLineStyle::DOTS);
				break;
			default:
				break;
		}

		graph->update_graph();

	});

	Gdk::RGBA col; col.set_rgba(0.0, 1.0, 1.0, 1.0);
	linecolour->set_rgba(col);

	selectgraphlabel.set_label(_("Select graph"));
	selectthemelabel.set_label(_("Select theme"));

	selectgraph->insert(0, _("Single line series"));
	selectgraph->insert(1, _("Multi line series"));

	selecttheme->insert(0, _("Fade to black"));
	selecttheme->insert(1, _("Black"));
	selecttheme->insert(2, _("Midnight blue"));
	selecttheme->insert(3, _("Herculean blue"));
	selecttheme->insert(4, _("Dark"));
	selecttheme->insert(5, _("Desktop theme"));
	selecttheme->set_tooltip_text("Determines the graph colours. The default uses desktop theme colours.");

	sizegroup = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_HORIZONTAL);
	sizegroup->add_widget(*selectgraph);
	sizegroup->add_widget(*selecttheme);
	sizegroup->add_widget(*start_velocity);
	sizegroup->add_widget(*start_time);
	sizegroup->add_widget(*gravitational_constant);
	sizegroup->add_widget(*selectlinestyle);

	selectgraph->signal_changed().connect([this](){
		if (0 == selectgraph->get_active_row_number()) single_series = true;
		else single_series = false;
		make_plot();
	});

	linecolour->signal_color_set().connect([this](){

		Gdk::RGBA col = linecolour->get_rgba();
		graph->set_line_colour(0, col);
		graph->update_graph();
	});

	selecttheme->signal_changed().connect([this](){

		Gdk::RGBA col; col.set_rgba(0.0, 1.0, 1.0, 1.0);
		linecolour->set_rgba(col);

		switch(selecttheme->get_active_row_number())
		{
			case 0:
				graph->set_theme("Fade to black");
				break;
			case 1:
				graph->set_theme("Black");
				break;
			case 2:
				graph->set_theme("Midnight blue");
				break;
			case 3:
				graph->set_theme("Herculean blue");
				break;
			case 4:
				graph->set_theme("Dark");
				break;
			case 5:
				graph->set_theme("Default");
				break;
			
			default:
				break;
		}

		if (single_series == true) graph->set_line_colour(0, col);
		
		graph->update_graph();
	});
	

	// create a graph and connect signals

	gravitational_constant->signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot));
	start_velocity->signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot));
	start_time->signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot));

	setscifiy->property_active().signal_changed().connect([this](){
		graph->use_scientific_notation(setscifix->get_active(), setscifiy->get_active());
		graph->update_graph();
	});

	setscifix->property_active().signal_changed().connect([this](){
		graph->use_scientific_notation(setscifix->get_active(), setscifiy->get_active());
		graph->update_graph();
	});

	hidelegend->property_active().signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot)); // lazy compete redraw

	// add the graph

	gravitational_constant_label.set_markup(_("<i>g</i> / m s<sup>-2</sup>"));
	gravitational_constant_label.set_halign(Gtk::ALIGN_END);
	start_velocity_label.set_markup(_("<i>v</i><sub>o</sub> / m s<sup>-1</sup>"));
	start_velocity_label.set_halign(Gtk::ALIGN_END);
	start_timelabel->set_markup(_("<i>t</i><sub>o</sub> / s"));

	gravitational_constant->set_digits(4);
	gravitational_constant->set_numeric(true);
	gravitational_constant->set_range(0.01, 50.0);
	gravitational_constant->set_increments(0.1, 0.5);
	gravitational_constant->set_value(9.8100);
	gravitational_constant->set_halign(Gtk::ALIGN_CENTER);

	start_velocity->set_digits(4);
	start_velocity->set_numeric(true);
	start_velocity->set_range(0.0, 1000.0);
	start_velocity->set_increments(0.1, 1.0);
	start_velocity->set_value(100.0000);
	start_velocity->set_halign(Gtk::ALIGN_CENTER);

	start_time->set_digits(4);
	start_time->set_numeric(true);
	start_time->set_range(0.0, 1000.0);
	start_time->set_increments(0.1, 1.0);
	start_time->set_value(10.0000);
	start_time->set_halign(Gtk::ALIGN_CENTER);

	grid.set_column_spacing(10);
	grid.set_row_spacing(10);
	grid.set_border_width(10);
	grid.attach(graphframe, 0, 0, 4, 1);

	controlgrid.set_row_spacing(20);
	controlgrid.set_column_spacing(10);
	controlgrid.set_border_width(10);
	controlgrid.attach(selectgraphlabel, 0, 0);
	controlgrid.attach(*selectgraph, 1, 0);
	controlgrid.attach(selectthemelabel, 0, 1);
	controlgrid.attach(*selecttheme, 1, 1);
	controlgrid.attach(*setscifixlabel, 0, 2);
	controlgrid.attach(*setscifix, 1, 2);
	controlgrid.attach(*setscifiylabel, 0, 3);
	controlgrid.attach(*setscifiy, 1, 3);
	controlgrid.attach(*hidelegend_label, 0, 4);
	controlgrid.attach(*hidelegend, 1, 4);
	controlgrid.attach(*linecolourlabel, 0, 5);
	controlgrid.attach(*linecolour, 1, 5);
	controlgrid.attach(*selectlinestylelabel, 0, 6);
	controlgrid.attach(*selectlinestyle, 1, 6);
	controlgrid.attach(*graphboxstyle_label, 0, 7);
	controlgrid.attach(*graphboxstyle, 1, 7);
	controlgrid.attach(*sep, 0, 8, 2, 1);
	controlgrid.attach(start_velocity_label, 0, 9);
	controlgrid.attach(*start_velocity, 1, 9);
	controlgrid.attach(*start_timelabel, 0, 10);
	controlgrid.attach(*start_time, 1, 10);
	controlgrid.attach(gravitational_constant_label, 0, 11);
	controlgrid.attach(*gravitational_constant, 1, 11);

	graphcontrolframe.set_label(_("Graph controls"));
	graphcontrolframe.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
	graphcontrolframe.set_border_width(10);
	graphcontrolframe.add(controlgrid);

	maingrid.attach(grid, 0, 0);
	maingrid.attach(graphcontrolframe, 1, 0);
	add(maingrid);

	show_all_children();
	selecttheme->set_active(3);
	selectlinestyle->set_active(0);
	graphboxstyle->set_active(0);
	selectgraph->set_active(0);
	selectgraph->grab_focus();

    auto note = Gio::Notification::create(_("cairoplot"));
    note->set_body("Welcome to Cairo plotting");
    m_app->send_notification(_("Cairo Graphs"), note);
}

void CairoplotWindow::create_header_and_menus()
{
	set_titlebar(headerbar);
	headerbar.set_show_close_button(true);
	headerbar.set_title(_("2D Graphs Demo - gtkmm & cariomm"));
	menubutton.set_image_from_icon_name("open-menu-symbolic", Gtk::ICON_SIZE_BUTTON, true);
   	menubutton.set_tooltip_text(_("Opens the menu."));
	headerbar.pack_end(menubutton);
	headerbar.show();
	winmenu = Gio::Menu::create();

	winmenu->append(_("_About"), "win.about");
	winmenu->append(_("_Help"), "win.help");
	winmenu->append(_("_Quit"), "win.quit");

	menubutton.set_menu_model(winmenu);

	m_app->set_accel_for_action("win.quit", "<Ctrl>q");


	add_action("help", [this](){
		Glib::ustring message;
		message = _("To zoom in hold down the left mouse button and drag out a rectanglular area, then release. Right click resets. "  
					"Use the controls on the right to change graph properties.");
		Gtk::MessageDialog message_dialog(message, true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE, true);
    	message_dialog.run();
	});

	add_action("quit", sigc::mem_fun(*this, &CairoplotWindow::close));
	add_action("about", sigc::mem_fun(*this,&CairoplotWindow::about));
}

void CairoplotWindow::about()
{
	Gtk::AboutDialog aboutdialog;
	aboutdialog.set_transient_for(*this);

	aboutdialog.set_logo(Gdk::Pixbuf::create_from_resource("/org/gnome/plotter/plotter.png", 128, 128, true));
	aboutdialog.set_program_name(_("Cairo plot"));
	aboutdialog.set_version("0.0.5");
	aboutdialog.set_copyright("Alexander Borro");
	aboutdialog.set_comments(_("Plotting 2D graphs using Cairomm."));
	aboutdialog.set_license("GPL v3.0    http://www.gnu.org/licenses");
	aboutdialog.set_website("http://www.gtkmm.org");
	aboutdialog.set_website_label("gtkmm website");

	std::vector<Glib::ustring> list_authors;
	list_authors.push_back("Alexander Borro");
	aboutdialog.set_authors(list_authors);
	const int id = aboutdialog.run();
	aboutdialog.present();

	switch (id)
	{
	case Gtk::RESPONSE_OK:
	case Gtk::RESPONSE_CLOSE:
	case Gtk::RESPONSE_CANCEL:
	case Gtk::RESPONSE_DELETE_EVENT:
		aboutdialog.hide();
		break;
	default:
		break;
	}
}

void CairoplotWindow::make_plot()
{
	double velocity = start_velocity->get_value();
	double accel = gravitational_constant->get_value();
	double j = start_time->get_value();

	if ( true == single_series) 
	{
		for (size_t i = 0; i < size; ++i)
		{
			t[i]  = j;
			a[i]  = velocity + 0.5 * accel * t[i] * t[i];
			j +=  0.25 * 100.0 / 99.0;
		}
	}
	else // multi series
	{
		for (size_t k = 0; k < numplots; ++k)
		{
			j = start_time->get_value();

			for (size_t i = 0; i < size; ++i)
			{	
				double tmp  = accel;
				if (k == 1) tmp = 3.71;
				if (k == 2) tmp = 5.55;
				xseries[k][i] = j;
				yseries[k][i] =  velocity + 0.5 * tmp * xseries[k][i] * xseries[k][i];
				j +=  0.25 * 100.0 / 99.0;
			}
		}
	}

	graph->set_tick_label_format_x(false, 2);
	graph->set_tick_label_format_y(true, 0);
	graph->set_axes_labels(_("<i>t</i> / s"), _("<i>s</i> / m"));

	if ( true == single_series ) 
	{
		Glib::ustring legend = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(" + Glib::ustring::format(accel) + ")<i>t</i><sup>2</sup>";
		graph->add_series(t, a, linecolour->get_rgba(), static_cast<CairoGraphLineStyle>(selectlinestyle->get_active_row_number()), false);
		
		if (false == hidelegend->get_active()) graph->add_single_legend(legend, CairoGraphPos::LEGEND_TOP, true);
	}
	else 
	{
		std::vector<Glib::ustring> legends(numplots);
		legends[0] = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(" + Glib::ustring::format(accel) + ")<i>t</i><sup>2</sup>";
		legends[1] = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(3.71)<i>t</i><sup>2</sup>";
		legends[2] = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(5.55)<i>t</i><sup>2</sup>";

		graph->add_multi_series(xseries, yseries, false);
		
		if (false == hidelegend->get_active()) graph->add_multi_legends(legends);
	}

	graph->update_graph();
	selectlinestyle->set_active(0);
}

void CairoplotWindow::close()
{	
	hide();
}
