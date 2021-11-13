#include "plotter.hpp"
#include <gtkmm/cssprovider.h>
#include <gtkmm/settings.h>

const Glib::ustring& CGraph::CairoGraph::get_theme_name() const 
{   
    //! Get the current theme name 
    return current_theme;
}

void CGraph::CairoGraph::set_theme(const Glib::ustring& theme, bool automatic)
{
    //! Set the graph theme.
    //! theme: the theme name
    //! automatic: Whether to automatically assign line colours baseed on theme, this is true by default.
    //!
    //! if not specified the theme is name is Black
    //! If set to "Default", desktop theme colours will be used
    //!
    //! Supported themes are 
    //! "Fade to black", "Herculean blue", "Black", "Midnight blue", "Dark", "Default"

    if ("Fade to black" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.28, 0.28, 0.28, 1.0);
    }
    else if ("Herculean blue" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(16.0 / 255.0, 29.0 / 255.0, 53.0 / 255.0, 1.0);
        bg_colour2.set_rgba(5.0 / 255.0, 95.00 / 255.0, 134.0 / 255.0, 1.0);
    }
    else if ("Black" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.0, 0.0, 0.0, 1.0);
    }
    else if ("Midnight blue" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.1, 0.1, 0.1, 1.0);
        bg_colour2.set("midnight blue");
    }
    else if ("Dark" ==  theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
     	bg_colour1.set_rgba(19.0 / 255.0, 19.0 / 255.0, 25.0 / 255.0, 1.0);
		bg_colour2.set_rgba(46.0 / 255.0, 48.0 / 255.0, 58.0 / 255.0, 1.0);
    }
    else if ("Default" ==  theme)
    {
        // get the colours from the system theme
        current_theme = "Default";
        const auto context = xvalue->get_style_context();
        axes_colour.set(context->get_color().to_string());

        if(Gtk::Settings::get_default()->property_gtk_theme_name().get_value().lowercase() != "adwaita"
           && Gtk::Settings::get_default()->property_gtk_theme_name().get_value().lowercase() != "adwaita-dark")
        {
            bg_colour1.set_alpha(0.0);
            bg_colour2.set_alpha(0.0);
            border_colour.set_rgba(0.05, 0.05, 0.05, 0.333);
        }
        
        else if(Gtk::Settings::get_default()->property_gtk_theme_name().get_value().lowercase() == "adwaita-dark"
          || Gtk::Settings::get_default()->property_gtk_application_prefer_dark_theme().get_value() == true)
        {
            bg_colour1.set_rgba(0.176, 0.176, 0.176, 1.0);
            bg_colour2 = bg_colour1;
            border_colour.set_rgba(0.05, 0.05, 0.05, 1.0);
        }
        else if (Gtk::Settings::get_default()->property_gtk_theme_name().get_value().lowercase() == "adwaita")
        {
            bg_colour1.set_rgba(0.99, 0.99, 0.99, 1.0);
            bg_colour2 = bg_colour1;
            border_colour.set_rgba(0.05, 0.05, 0.05, 0.333);
        }
        // fall through mixing gtk4 && 3 is a mess, it can happen
        bg_colour1.set_alpha(0.0);
        bg_colour2.set_alpha(0.0);
        border_colour.set_rgba(0.05, 0.05, 0.05, 0.333);
    }
    else
    {
        current_theme = "Black";
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.0, 0.0, 0.0, 1.0);
    }


    if (false == automatic) return;

    set_series_colours();
    // automatic line colouring if requested
}

void CGraph::CairoGraph::set_series_colours()
{
    seriescolour.clear();

    if (axes_colour.get_red() < 0.25 && axes_colour.get_green() < 0.25 && axes_colour.get_blue() < 0.25)
    {   // prabably a light theme
        for (size_t i = seriescolour.size(); i < numpoints.size(); ++i)
        {
            Gdk::RGBA colour;
            colour.set_rgba(0.10 * static_cast<double>((numpoints.size() + 0.75 * i) / (numpoints.size() + i)),
                            0.45 * static_cast<double>((numpoints.size() - 0.5 * i) / (numpoints.size() + i)),
                            0.45 * static_cast<double>((numpoints.size() + 0.75 * i) / (numpoints.size() + i)), 1.0);
            seriescolour.push_back(colour);
        }
    }
    else
    {
        for (size_t i = seriescolour.size(); i < numpoints.size(); ++i)
        {
            Gdk::RGBA colour;
            colour.set_rgba(1.0 * static_cast<double>(i + 1) / numpoints.size(),
                            0.5 + 0.5 * static_cast<double>(i + 1) / numpoints.size(),
                            0.5 + 0.5 * static_cast<double>(i + 1) / numpoints.size(), 1.0);
            seriescolour.push_back(colour);
        }
    }
}
