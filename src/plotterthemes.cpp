#include "plotter.hpp"

const Glib::ustring& CairoGraph::get_theme_name() const 
{
     return current_theme;
}

void CairoGraph::set_theme(const Glib::ustring& theme)
{
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
    else if ("Adwaita-dark" ==  theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
     	bg_colour1.set_rgba(48.0 / 255.0, 48.0 / 255.0, 48.0 / 255.0, 1.0);
		bg_colour2.set_rgba(48.0 / 255.0, 48.0 / 255.0, 48.0 / 255.0, 1.0);
    }
    else if ("Adwaita" ==  theme)
    {
        current_theme = theme;
        axes_colour.set_rgba(0.0, 0.0, 0.0, 0.75);
     	bg_colour1.set("white");
		bg_colour2.set("white");
    }
    else
    {
        current_theme = "Fade to black";
        axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
        bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0);
        bg_colour2.set_rgba(0.28, 0.28, 0.28, 1.0);
    }


    if ("Adwaita" == current_theme)
    {
        for(size_t i = 0; i < seriesy.size(); ++i)
        {
                Gdk::RGBA colour;
                colour.set_rgba(0.0, 
                                0.5 * static_cast<double>(seriesy.size() - i) / seriesy.size(), 
                                0.5 * static_cast<double>(seriesy.size() - i) / seriesy.size(), 1.0);
                seriescolour[i] = colour;
        }
    }
    else
    {
        for(size_t i = 0; i < seriesy.size(); ++i)
        {
            Gdk::RGBA colour;
            colour.set_rgba(1.0 * static_cast<double>(i + 1) / seriesy.size(), 
                            0.5 + 0.5 * static_cast<double>(i + 1) / seriesy.size(), 
                            0.5 + 0.5 * static_cast<double>(i + 1) / seriesy.size(), 1.0);
            seriescolour[i] = colour;
        }
    }
}