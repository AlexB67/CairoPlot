/* main.cpp
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

Gtk::Window *window;

static void
on_activate (Glib::RefPtr<Gtk::Application> app)
{
	window = app->get_active_window();

	if (!window) {
		window = new CairoplotWindow(app);
		window->property_application() = app;
		window->property_default_width() =  980;
		window->property_default_height() = 680;
		app->add_window(*window);
	}

	window->present();
}

int main (int argc, char *argv[])
{
	int ret;

	Glib::RefPtr<Gtk::Application> app =
		Gtk::Application::create("org.gnome.plotter", Gio::APPLICATION_FLAGS_NONE);

	app->signal_activate().connect(sigc::bind(&on_activate, app));

	ret = app->run(argc, argv);
	delete window;
	return ret;
}
