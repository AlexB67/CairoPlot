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
#include <iomanip>
#include <glibmm/ustring.h>
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
	graphframe.set_label_align(Gtk::Align::CENTER);
	graphframe.set_hexpand(true);

	graph = Gtk::make_managed<CGraph::CairoGraph>();
	graph2 = Gtk::make_managed<CGraph::CairoGraph>();
	selectgraph = Gtk::make_managed<Gtk::ComboBoxText>();
	selectgraph->set_margin_top(10);
	selecttheme = Gtk::make_managed<Gtk::ComboBoxText>();
	start_velocity = Gtk::make_managed<Gtk::SpinButton>();
	gravitational_constant = Gtk::make_managed<Gtk::SpinButton>();
	start_time = Gtk::make_managed<Gtk::SpinButton>();
	sep = Gtk::make_managed<Gtk::Separator>();
	setscifiy = Gtk::make_managed<Gtk::Switch>();
	setscifiy->set_halign(Gtk::Align::END);
	setscifiy->set_valign(Gtk::Align::CENTER);
	setscifiy->set_active(false);
	setscifiylabel = Gtk::make_managed<Gtk::Label>("Y axis exp.", Gtk::Align::END);
	setscifix = Gtk::make_managed<Gtk::Switch>();
	setscifix->set_halign(Gtk::Align::END);
	setscifix->set_valign(Gtk::Align::CENTER);
	setscifix->set_active(false);
	hidelegend = Gtk::make_managed<Gtk::Switch>();
	hidelegend->set_halign(Gtk::Align::END);
	hidelegend->set_valign(Gtk::Align::CENTER);
	hidelegend->set_active(false);
	hidelegend_label = Gtk::make_managed<Gtk::Label>("Hide legend", Gtk::Align::END);
	setscifixlabel = Gtk::make_managed<Gtk::Label>("X axis exp.", Gtk::Align::END);
	linecolourlabel = Gtk::make_managed<Gtk::Label>("Line colour", Gtk::Align::END);
	start_timelabel = Gtk::make_managed<Gtk::Label>("", Gtk::Align::END);
	graphboxstyle_label = Gtk::make_managed<Gtk::Label>("Graph style", Gtk::Align::END);
	linecolour = Gtk::make_managed<Gtk::ColorButton>();
	selectlinestylelabel = Gtk::make_managed<Gtk::Label>("Line style", Gtk::Align::END);
	selectlinestyle = Gtk::make_managed<Gtk::ComboBoxText>();
	graphboxstyle = Gtk::make_managed<Gtk::ComboBoxText>();

	//graphframe.add(graph->create_graph());
	graphframe.set_child(graph->create_graph());
	graph->get_graph_grid().set_margin(10);
	graph->set_title(_("Displacement versus time"));
	
	selectlinestyle->insert(0, "Solid line");
	selectlinestyle->insert(1, "Dashed line");
	selectlinestyle->insert(2, "Circles");
	selectlinestyle->insert(3, "Dots");

	graphboxstyle->insert(0, "Box and grid");
	graphboxstyle->insert(1, "Axes and grid");
	graphboxstyle->insert(2, "Box only");
	graphboxstyle->insert(3, "Axes only");

	graphboxstyle->signal_changed().connect((sigc::track_obj([this](){
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
	}, *this)));



	selectlinestyle->signal_changed().connect((sigc::track_obj([this](){

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

	}, *this)));

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

	// sizegroup = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_HORIZONTAL);
	sizegroup = Gtk::SizeGroup::create(Gtk::SizeGroup::Mode::HORIZONTAL);
	sizegroup->add_widget(*selectgraph);
	sizegroup->add_widget(*selecttheme);
	sizegroup->add_widget(*start_velocity);
	sizegroup->add_widget(*start_time);
	sizegroup->add_widget(*gravitational_constant);
	sizegroup->add_widget(*selectlinestyle);

	selectgraph->signal_changed().connect((sigc::track_obj([this](){
		if (0 == selectgraph->get_active_row_number()) single_series = true;
		else single_series = false;
		make_plot();
	}, *this)));

	linecolour->signal_color_set().connect((sigc::track_obj([this](){

		Gdk::RGBA col = linecolour->get_rgba();
		graph->set_line_colour(0, col);
		graph->update_graph();
	}, *this)));

	selecttheme->signal_changed().connect((sigc::track_obj([this](){

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
		graph2->set_theme("Default"); // exclude
		graph2->update_graph();
	}, *this)));

	// TODO Move this to plotter lib
	Gtk::Settings::get_default()->property_gtk_theme_name().signal_changed().connect((sigc::track_obj([this](){

		const Glib::ustring& active_theme = Gtk::Settings::get_default()->property_gtk_theme_name().get_value();

		if (active_theme.find("dark") != Glib::ustring::npos || active_theme.find("Dark") != Glib::ustring::npos)
			preferdark->set_sensitive(false);
		else
		{
			preferdark->set_sensitive(true);
			preferdark->set_active(true);
		}
		
		graph->set_theme("Default");
		graph->update_graph();
		graph2->set_theme("Default");
		graph2->update_graph();

	}, *this)));

	// create a graph and connect signals

	gravitational_constant->signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot));
	start_velocity->signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot));
	start_time->signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot));

	setscifiy->property_active().signal_changed().connect((sigc::track_obj([this](){
		graph->use_scientific_notation(setscifix->get_active(), setscifiy->get_active());
		graph->update_graph();
	}, *this)));

	setscifix->property_active().signal_changed().connect((sigc::track_obj([this](){
		graph->use_scientific_notation(setscifix->get_active(), setscifiy->get_active());
		graph->update_graph();
	}, *this)));

	hidelegend->property_active().signal_changed().connect(sigc::mem_fun(*this, &CairoplotWindow::make_plot)); // lazy compete redraw

	// add the graph

	gravitational_constant_label.set_markup(_("<i>g</i> / m s<sup>-2</sup>"));
	gravitational_constant_label.set_halign(Gtk::Align::END);
	start_velocity_label.set_markup(_("<i>v</i><sub>o</sub> / m s<sup>-1</sup>"));
	start_velocity_label.set_halign(Gtk::Align::END);
	start_timelabel->set_markup(_("<i>t</i><sub>o</sub> / s"));

	gravitational_constant->set_digits(4);
	gravitational_constant->set_numeric(true);
	gravitational_constant->set_range(0.01, 50.0);
	gravitational_constant->set_increments(0.1, 0.5);
	gravitational_constant->set_value(9.8100);
	gravitational_constant->set_halign(Gtk::Align::CENTER);

	start_velocity->set_digits(4);
	start_velocity->set_numeric(true);
	start_velocity->set_range(0.0, 1000.0);
	start_velocity->set_increments(0.1, 1.0);
	start_velocity->set_value(100.0000);
	start_velocity->set_halign(Gtk::Align::CENTER);

	start_time->set_digits(4);
	start_time->set_numeric(true);
	start_time->set_range(0.0, 1000.0);
	start_time->set_increments(0.1, 1.0);
	start_time->set_value(10.0000);
	start_time->set_halign(Gtk::Align::CENTER);

	grid.set_column_spacing(10);
	grid.set_row_spacing(10);
	grid.set_margin(10);
	grid.attach(graphframe, 0, 0, 4, 1);

	controlgrid.set_row_spacing(20);
	controlgrid.set_column_spacing(10);
	controlgrid.set_margin(10);
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
	graphcontrolframe.set_label_align(Gtk::Align::CENTER);
	graphcontrolframe.set_margin(10);
	graphcontrolframe.set_child(controlgrid);

	maingrid.attach(grid, 0, 0);
	maingrid.attach(graphcontrolframe, 1, 0);
	

	stack.add(maingrid, _("Plot 1"), _("Plot 1"));
	stack.add(maingrid2, _("Plot 2"), _("Plot 2"));
	switcher.set_stack(stack);
	selecttheme->set_active(5);
	selectlinestyle->set_active(0);
	graphboxstyle->set_active(0);
	selectgraph->set_active(0);
	selectgraph->grab_focus();

	set_child(stack);
	
	// plot 2
	maingrid2.set_margin(10);
	scale2.set_orientation(Gtk::Orientation::VERTICAL);
	scale2.set_vexpand(true);
	scale2.set_range(3 * M_PI, 7 * M_PI);
	scale2.set_show_fill_level(true);
	scale2.set_tooltip_markup
	(_("The time (in dimensionless units of <i>t</i><sub>C</sub> |<i>V</i> | ) the perturbation is switched off."));

	scalecon2 = scale2.signal_value_changed().connect((sigc::track_obj([this](){
		if (!is_animation) make_plot2();
	}, *this)));

	button_start2.signal_clicked().connect(sigc::mem_fun(*this, &CairoplotWindow::on_animation_clicked));
	button_stop2.signal_clicked().connect(sigc::mem_fun(*this, &CairoplotWindow::on_stop_clicked));
	button_faster.signal_clicked().connect(sigc::mem_fun(*this, &CairoplotWindow::on_faster_clicked));
	button_slower.signal_clicked().connect(sigc::mem_fun(*this, &CairoplotWindow::on_slower_clicked));
	button_reset.signal_clicked().connect(sigc::mem_fun(*this, &CairoplotWindow::on_reset_clicked));
	//view-refresh-symbolic
	
	graph2->get_graph_grid().set_margin(10);
	button_start2.set_expand(false);
	button_start2.set_icon_name("media-playback-start-symbolic");
	button_start2.set_tooltip_text(_("Play the animation."));
	
	button_faster.set_expand(false);
	button_faster.set_icon_name("media-seek-forward-symbolic");
	button_faster.set_tooltip_text(_("Speed up the animation."));
	
	button_slower.set_expand(false);
	button_slower.set_icon_name("media-seek-backward-symbolic");
	button_slower.set_tooltip_text(_("Slow down the animation."));

	button_stop2.set_expand(false);
	button_stop2.set_icon_name("media-playback-pause-symbolic");
	button_stop2.set_sensitive(false);
	button_stop2.set_tooltip_text(_("Pause the animation."));

	button_reset.set_expand(false);
	button_reset.set_icon_name("media-playback-stop-symbolic");
	button_reset.set_sensitive(true);
	button_reset.set_tooltip_text(_("Stop the animation and restore defaults."));

	scalelabel2.set_markup(_("<i>t</i><sub>C</sub> |<i>V</i> |"));
	scalelabel2.set_tooltip_markup
	(_("The time (in dimensionless units of <i>t</i><sub>C</sub> |<i>V</i> | ) the perturbation is switched off."));
	
	grid2.attach(graph2->create_graph(), 0, 0, 10, 7);
	controlgrid2.set_row_spacing(10);
	controlgrid2.set_column_spacing(10);
	controlgrid2.set_margin(10);

	sep2.set_vexpand(false);
	animate_label2.set_label(_("Animate"));

	controlgrid2.attach(scale2, 0, 0, 2, 1);
	controlgrid2.attach(scalelabel2, 0, 1, 2, 1);
	controlgrid2.attach(sep2, 0, 2, 2, 1);
	controlgrid2.attach(animate_label2, 0, 3, 2, 1);
	controlgrid2.attach(button_slower, 0, 4, 1, 1);
	controlgrid2.attach(button_faster, 1, 4, 1, 1);
	controlgrid2.attach(button_stop2, 0, 5, 1, 1);
	controlgrid2.attach(button_start2, 1, 5, 1, 1);
	controlgrid2.attach(button_reset, 0, 6, 2, 1);

	graphcontrolframe2.set_label(_("Manipulate"));
	graphcontrolframe2.set_label_align(Gtk::Align::CENTER);
	graphcontrolframe2.set_margin_start(10);
	graphcontrolframe2.set_child(controlgrid2);

	graphframe2.set_label(_("Graph"));
	graphframe2.set_label_align(Gtk::Align::CENTER);
	graphframe2.set_hexpand(true);
	graphframe2.set_child(grid2);

	maingrid2.attach(graphframe2, 0, 0);
	maingrid2.attach(graphcontrolframe2, 1, 0);
	
	graph2->get_graph_grid().set_margin(10);
	graph2->set_title(_("Rabi oscillations: A 2 level system at 3 energy separations 𝜔<sub>21</sub>"));
	make_plot2();

    auto note = Gio::Notification::create(_("cairoplot"));
    note->set_body("Welcome to Cairo plotting.");
    m_app->send_notification(_("Cairo Graphs"), note);
}

void CairoplotWindow::create_header_and_menus()
{
	set_titlebar(headerbar);
	headerbar.set_show_title_buttons(true);
	headerbarlabel.set_markup_with_mnemonic(_("Cairoplot \n<sub>Plot 2D Graphs with gtkmm4 demo</sub>"));
	headerbar.set_title_widget(headerbarlabel);
	menubutton.set_icon_name("open-menu-symbolic");
   	menubutton.set_tooltip_text(_("Opens the menu."));
	headerbar.pack_start(switcher);
	headerbar.pack_end(menubutton);
	preferdark = Gtk::make_managed<Gtk::Switch>();
	preferdark->set_tooltip_text("Enable or disable the \"Prefer dark theme\" option (if a light theme is active)." \
	                             " If a dark theme is the default this will be disabled.");

	headerbar.pack_end(*preferdark);
	preferdark->set_active(true);

	const Glib::ustring& active_theme = Gtk::Settings::get_default()->property_gtk_theme_name().get_value();
	if (active_theme.find("dark") != Glib::ustring::npos || active_theme.find("Dark") != Glib::ustring::npos)
		preferdark->set_sensitive(false);

	preferdark->property_active().signal_changed().connect((sigc::track_obj([this](){
		Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme().set_value(preferdark->get_active());
		graph->set_theme("Default");
		graph->update_graph();
		graph2->set_theme("Default");
		graph2->update_graph();
		selecttheme->set_active(5);
	}, *this)));

	winmenu = Gio::Menu::create();
	winmenu->append(_("_About"), "win.about");
	winmenu->append(_("_Help"), "win.help");
	winmenu->append(_("_Quit"), "win.quit");

	menubutton.set_menu_model(winmenu);

	m_app->set_accel_for_action("win.quit", "<Ctrl>q");

	add_action("help", (sigc::track_obj([this](){
    	help_dialog->show();
		help_dialog->present();
	}, *this)));

	add_action("quit", sigc::mem_fun(*this, &CairoplotWindow::close));

	about();
	add_action("about", (sigc::track_obj([this](){
		aboutdialog->show();
		aboutdialog->present();
	}, *this)));

	Glib::ustring message
	= _("To zoom in hold down the left mouse button and drag out a rectanglular area, then release. Right click resets. "  
		"Use the controls on the right to change graph properties.\n\nUse the start and stop buttons on the second graph to animate. "
		"Use the fast forward and back buttons to speed up or slow down respectively");
		Glib::ustring title
	= _("Graph Zoom and Animations");

	help_dialog = std::make_unique<Gtk::MessageDialog>(title, true, Gtk::MessageType::INFO, Gtk::ButtonsType::CLOSE, false);
	help_dialog->set_transient_for(*this);
	help_dialog->set_secondary_text(message);
	help_dialog->set_modal(true);
	help_dialog->signal_response().connect(
    sigc::hide((sigc::track_obj([this](){help_dialog->hide();}, *this))));
}

void CairoplotWindow::about()
{
	aboutdialog = std::make_unique<Gtk::AboutDialog>();
	aboutdialog->set_transient_for(*this);
	aboutdialog->set_hide_on_close();
	aboutdialog->set_modal(true);

	// aboutdialog.set_logo(Gdk::Pixbuf::create_from_resource("/org/gnome/plotter/plotter.png", 128, 128, true));
	
	aboutdialog->set_logo(Gdk::Texture::create_for_pixbuf(Gdk::Pixbuf::create_from_resource
	("/org/gnome/plotter/plotter.png", 128, 128, true)));
	
	aboutdialog->set_program_name(_("Cairo plot"));
	aboutdialog->set_version("0.3.0");
	aboutdialog->set_copyright("Alexander Borro");
	aboutdialog->set_comments(_("Plotting 2D graphs using gtkmm-4.0.4 and Cairomm-1.16."));
	aboutdialog->set_license("GPL v3.0    http://www.gnu.org/licenses");
	aboutdialog->set_website("http://www.gtkmm.org");
	aboutdialog->set_website_label("gtkmm website");

	std::vector<Glib::ustring> list_authors;
	list_authors.emplace_back("Alexander Borro");
	aboutdialog->set_authors(list_authors);
}

void CairoplotWindow::on_animation_clicked()
{
	timer2.disconnect();
	button_start2.set_sensitive(false);
	button_stop2.set_sensitive(true);
	is_animation = true;
	scalecon2.block();
	graph2->set_sensitive(false);
	sigc::slot<bool()> slot = sigc::bind(sigc::mem_fun(*this, &CairoplotWindow::make_plot2));
	timer2 = Glib::signal_timeout().connect(slot, interval);
}

void CairoplotWindow::on_stop_clicked()
{
	stop_was_clicked = true;
}

void CairoplotWindow::on_faster_clicked()
{
	if (interval <= 10)
	{
		button_slower.set_sensitive(true);
		button_faster.set_sensitive(false);
		interval = 10;
	}
	else
	{
		interval -= 4;
		button_slower.set_sensitive(true);
		timer2.disconnect();
		sigc::slot<bool()> slot = sigc::bind(sigc::mem_fun(*this, &CairoplotWindow::make_plot2));
		timer2 = Glib::signal_timeout().connect(slot, interval);
	}
}

void CairoplotWindow::on_slower_clicked()
{
	if (interval >= 50)
	{
		button_faster.set_sensitive(true);
		button_slower.set_sensitive(false);
		interval = 50;
	}
	else
	{
		interval += 4;
		button_faster.set_sensitive(true);
		timer2.disconnect();
		sigc::slot<bool()> slot = sigc::bind(sigc::mem_fun(*this, &CairoplotWindow::make_plot2));
		timer2 = Glib::signal_timeout().connect(slot, interval);
	}
}

void CairoplotWindow::animation_stop_func()
{
	button_start2.set_sensitive(true);
	button_stop2.set_sensitive(false);
	scalecon2.unblock();
	graph2->set_sensitive(true);
	graph2->set_sensitive(true);
	is_animation = false;
	stop_was_clicked = false;
}

void CairoplotWindow::on_reset_clicked()
{
	animation_stop_func();
	button_slower.set_sensitive(true);
	button_faster.set_sensitive(true);

	if (timer2.connected()) timer2.disconnect();
	interval = 30;
	scale2.set_value(3 * M_PI);
}

bool CairoplotWindow::make_plot2()
{
	if (is_animation && stop_was_clicked)
	{
		animation_stop_func();
		return false;
	}

	constexpr size_t size = 501;
	constexpr size_t numplots = 3;

	std::vector<double> t(size);
	std::vector<double> s(size);
	std::vector<std::vector<double> > xseries(numplots, t);
	std::vector<std::vector<double> > yseries(numplots, s);

	// Example of multiplot with preset data stored in a vector of vectors
	const double V = 1.0;
	const std::vector<double> omega = {{0.0, 2.0, 3.0}};
	double t_cutoff = scale2.get_value();

	for (size_t k = 0; k < numplots; ++k)
	{
		double t = 0;
		double Omega = std::sqrt(4.0 * V * V + omega[k] * omega[k]);
		for (size_t i = 0; i < size; ++i)
		{
			xseries[k][i] = t;
			(t < t_cutoff) ? yseries[k][i] = 
			                 4.0 * V * V * std::pow(std::sin(Omega * t * 0.5), 2) / (Omega * Omega)
			               : yseries[k][i] = 
						     4.0 * V * V * std::pow(std::sin(Omega * t_cutoff * 0.5), 2) / (Omega * Omega);
			t += 0.05;
		}
	}

	graph2->set_tick_label_format_x(false, 2);
	graph2->set_tick_label_format_y(false, 2);
	graph2->set_axes_labels(_("<i>t </i>|<i>V</i> |"), _("<i>P (t)</i>"));

	std::vector<Glib::ustring> legends(numplots);
	legends[0] = "<i>λ</i> = 𝜔<sub>21</sub> / |<i>V</i> | = 0";
	legends[1] = "<i>λ</i> = 𝜔<sub>21</sub> / |<i>V</i> | = 2";
	legends[2] = "<i>λ</i> = 𝜔<sub>21</sub> / |<i>V</i> | = 3";

	graph2->add_multi_series(xseries, yseries); // Copy data for the example. 
											    // We can also pass by reference with make_copy = false

	graph2->add_multi_legends(legends, 0.7);

	std::vector<std::tuple<Glib::ustring, double, double, double, bool>> textobject(1U);
	std::get<0>(textobject[0]) = Glib::ustring("<i>t</i><sub>C</sub> |<i>V</i> | = ") +
	                             Glib::ustring::format(std::fixed, std::setprecision(2), scale2.get_value()); 
	std::get<1>(textobject[0]) = 0.85;
	std::get<2>(textobject[0]) = 1.20;
	std::get<3>(textobject[0]) = 1.0;
	std::get<4>(textobject[0]) = false;

	graph2->add_text_objects(textobject, _("Nimbus Roman"));
	graph2->update_graph();
	
	if (is_animation)
	{
		if (scale2.get_value() >= 7 * M_PI)
			scale2.set_value(3 * M_PI);
		else
			scale2.set_value(scale2.get_value() + 0.05);
	}
	
	return true;
}

void CairoplotWindow::make_plot()
{
	constexpr size_t size = 100;
	constexpr size_t numplots = 3;

	std::vector<double> t(size);
	std::vector<double> s(size);
	std::vector<std::vector<double> > xseries(numplots, t);
	std::vector<std::vector<double> > yseries(numplots, s);

	double velocity = start_velocity->get_value();
	double accel = gravitational_constant->get_value();
	double j = start_time->get_value();

	if ( true == single_series) 
	{
		// Example of a single plot point by point
		graph->init_plots(1);

		for (size_t i = 0; i < size; ++i)
		{
			double t_i = j;
			double s_i = velocity + 0.5 * accel * t_i * t_i;
			graph->add_point(0, t_i, s_i);
			j +=  0.25 * 100.0 / 99.0;
		}
	}
	else // multi series
	{
		// Example of multiplot with preset data stored in a vector of vectors
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
	graph->set_tick_label_format_y(false, 2);
	graph->set_axes_labels(_("<i>t</i> / s"), _("<i>s</i> / m"));

	if (true == single_series) 
	{
		// Gdk::RGBA col; col.set_rgba(linecolour->get_color().get_red(),
		// 							linecolour->get_color().get_blue(), 
		// 							linecolour->get_color().get_blue(),
		// 							1.0);
		Gdk::RGBA col; col.set_rgba(linecolour->get_rgba().get_red(),
		                            linecolour->get_rgba().get_blue(),
									linecolour->get_rgba().get_green(), 1.0);

		graph->set_line_colour(0, col);

		Glib::ustring legend = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(" + Glib::ustring::format(accel) + ")<i>t</i><sup>2</sup>";
		if (false == hidelegend->get_active()) graph->add_single_legend(legend, CairoGraphPos::LEGEND_TOP, true);
	}
	else 
	{
		std::vector<Glib::ustring> legends(numplots);
		legends[0] = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(" + Glib::ustring::format(accel) + ")<i>t</i><sup>2</sup>";
		legends[1] = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(3.71)<i>t</i><sup>2</sup>";
		legends[2] = "<i>s</i> = <i>v</i><sub>o</sub> + 0.5(5.55)<i>t</i><sup>2</sup>";

		graph->add_multi_series(xseries, yseries); 	// Copy data for the example. 
												  	// We can also pass by reference with make_copy = false

		if (false == hidelegend->get_active()) graph->add_multi_legends(legends);
	}

	graph->update_graph();
	selectlinestyle->set_active(0);
}

void CairoplotWindow::close()
{	
	hide();
}
