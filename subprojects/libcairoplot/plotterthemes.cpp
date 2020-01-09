#include "plotter.hpp"
#include <iostream>

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
    //!  "Fade to black", "Herculean blue", "Black", "Midnight blue", "Dark", "Default"

    if ( "Fade to black" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.28, 0.28, 0.28, 1.0);
    }
    else if ( "Herculean blue" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(16.0 / 255.0, 29.0 / 255.0, 53.0 / 255.0, 1.0);
        bg_colour2.set_rgba(5.0 / 255.0, 95.00 / 255.0, 134.0 / 255.0, 1.0);
    }
    else if ( "Black" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.0, 0.0, 0.0, 1.0);
    }
    else if ( "Midnight blue" == theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.1, 0.1, 0.1, 1.0);
        bg_colour2.set("midnight blue");
    }
    else if ( "Dark" ==  theme)
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
        auto context = xvalue->get_style_context();
        axes_colour.set(context->get_color().to_string());
     	bg_colour1.set(context->get_background_color().to_string());
		bg_colour2.set(context->get_background_color().to_string());

        // doesn't look good with many themes so reserve this for adwaita
        if( Gtk::Settings::get_default()->property_gtk_theme_name().get_value() == "Adwaita" || 
            Gtk::Settings::get_default()->property_gtk_theme_name().get_value() == "Adwaita-dark")
            border_colour = context->get_border_color();
        else border_colour.set_rgba(0.0, 0.0, 0.0, 0.33);
    }
    else
    {
        current_theme = "Black";
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.0, 0.0, 0.0, 1.0);
    }


    if (false == automatic) return;

    // automatic line colouring if requested

    if (axes_colour.get_red() < 0.25 && axes_colour.get_green() < 0.25 && axes_colour.get_blue() < 0.25)
    {   // prabably a light theme
        for (size_t i = seriescolour.size(); i < seriesy.size(); ++i)
        {
            Gdk::RGBA colour;
            colour.set_rgba(0.0,
                            0.5 * static_cast<double>(seriesy.size() - i) / seriesy.size(),
                            0.5 * static_cast<double>(seriesy.size() - i) / seriesy.size(), 1.0);
            seriescolour.emplace_back(colour);
        }
    }
    else
    {
        for (size_t i = seriescolour.size(); i < seriesy.size(); ++i)
        {
            Gdk::RGBA colour;
            colour.set_rgba(1.0 * static_cast<double>(i + 1) / seriesy.size(),
                            0.5 + 0.5 * static_cast<double>(i + 1) / seriesy.size(),
                            0.5 + 0.5 * static_cast<double>(i + 1) / seriesy.size(), 1.0);
            seriescolour.emplace_back(colour);
        }
    }
}