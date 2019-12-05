#include "plotter.hpp"

using namespace CarioGraphConstants;

void CairoGraph::set_axes_labels(const Glib::ustring &xlabel, const Glib::ustring &ylabel, Glib::ustring fontfamily)
{
    xvaluelabel->set_markup(xlabel);
    yvaluelabel->set_markup(ylabel);
    xmarkuplabel = xlabel;
    ymarkuplabel = ylabel;
    labels_fontfamily = fontfamily;
}

void CairoGraph::set_background_style(const Gdk::RGBA colour1, const Gdk::RGBA colour2)
{

    bg_colour1 = colour1;
    bg_colour2 = colour2;
    queue_draw_area(0, 0, get_allocation().get_width(), get_allocation().get_height());
}

void CairoGraph::set_background_style(const Gdk::RGBA colour1)
{
    bg_colour1 = colour1;
    bg_colour2 = colour1;
    queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
}

void CairoGraph::set_axes_colour(const Gdk::RGBA colour)
{
    axes_colour = colour;
    queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
}

void CairoGraph::set_line_colour(const size_t seriesnum, Gdk::RGBA colour)
{
    if (seriesnum > seriescolour.size() - 1) return;

    seriescolour[seriesnum] = colour;
}

void CairoGraph::add_line_colours(const std::vector<Gdk::RGBA>& colours)
{
    seriescolour.clear();
   for (auto &i : colours) seriescolour.emplace_back(i);
}

void CairoGraph::set_line_style(const size_t seriesnum, const CairoGraphLineStyle style)
{
    if (seriesnum > serieslinestyle.size() - 1) return;

    serieslinestyle[seriesnum] = style;
}

void CairoGraph::add_line_styles(const std::vector<CairoGraphLineStyle>& styles)
{
   // set the line style of each plot in the series
   serieslinestyle.clear();
   for (auto &i : styles) serieslinestyle.emplace_back(i);
}

void CairoGraph::set_graph_box_style(const CairoGraphBoxStyle style)
{
    graphboxstyle = style;
}

void CairoGraph::force_scientific_notation(const bool scientificx, const bool scientificy)
{
    forcescientificx = scientificx;
    forcescientificy = scientificy;
}

void CairoGraph::set_tick_label_format_x(const bool showpoint, const int precision)
{
    if (precision > 6) precisionx = 6;

    showpointx = showpoint;
    precisionx = precision;
}

void CairoGraph::set_tick_label_format_y(const bool showpoint, const int precision)
{
    if (precision > 6) precisiony = 6;

    showpointy = showpoint;
    precisiony = precision;
}

void CairoGraph::add_single_legend(const Glib::ustring&legend, CairoGraphPos pos, const bool showlinecolour)
{
    graph_legend.clear();
    graph_legend = legend;
    legend_pos   = pos;
    legend_show_colour = showlinecolour;
}

void CairoGraph::show_legend(const bool show)
{
    showlegend = show;
}

void CairoGraph::add_multi_legends(const std::vector<Glib::ustring> &legends, const double offsetx, const double offsety)
{
    graph_legends.clear();
    for ( auto &i: legends) graph_legends.emplace_back(i);
    legend_offsetx = offsetx * GRAPH_WIDTH;
    legend_offsety = offsety * GRAPH_HEIGHT;
}

void CairoGraph::add_text(/* const Glib::ustring& text, const double x, const double y */)
{
    // TO DO
   //  double mx, my;
   //  mx = OFFSET_X + x / GRAPH_WIDTH;
   //  my = OFFSET_Y + y / GRAPH_HEIGHT;
     
}