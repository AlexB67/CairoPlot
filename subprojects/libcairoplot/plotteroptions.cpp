#include "plotter.hpp"

using namespace CarioGraphConstants;

void CGraph::CairoGraph::set_line_width(const double linewidth)
{
    //! Set the line width of the graph when the line style is CairoGraphLineStyle::SOLID_LINE or DASHED_LINE
    //!A vale of 1 is the default line width. > 1 increases, < 1 decreases line width.
    lwidth = linewidth;
}

void CGraph::CairoGraph::set_title(const Glib::ustring& title)
{
    //! Set the graph title, Pango markup is supported.

    graph_title.clear();
    graph_title = title;
}

void CGraph::CairoGraph::set_axes_labels(const Glib::ustring &xlabel, const Glib::ustring &ylabel, Glib::ustring fontfamily)
{
    //! Set the axes lables of the X and Y axes. Pango markupo is supported.
    //! xlabel X axis label.
    //! ylabel Y axes label.
    //! Pango font family. The default is Nimbus Roman ( if present on the system)

    xvaluelabel->set_markup(xlabel);
    yvaluelabel->set_markup(ylabel);
    xmarkuplabel = xlabel;
    ymarkuplabel = ylabel;
    labels_fontfamily = fontfamily;
}

void CGraph::CairoGraph::set_background_colour(const Gdk::RGBA colour1, const Gdk::RGBA colour2)
{
    //! Set a custom graph background gradient colour specified by colour1 and colour2.

    bg_colour1 = colour1;
    bg_colour2 = colour2;
    queue_draw_area(0, 0, get_allocation().get_width(), get_allocation().get_height());
}

void CGraph::CairoGraph::set_background_colour(const Gdk::RGBA colour1)
{
    //! Set a custom backgroud colour for the graph.
    bg_colour1 = colour1;
    bg_colour2 = colour1;
    queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
}

void CGraph::CairoGraph::set_axes_colour(const Gdk::RGBA colour)
{
    axes_colour = colour;
    queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
}

void CGraph::CairoGraph::set_line_colour(const size_t seriesnum, Gdk::RGBA colour)
{
    //! Update the line colour of plot seriesnum
    //! Only use when a graph has already been added and you want to change colour

    if (seriesnum > seriescolour.size() - 1) return;

    seriescolour[seriesnum] = colour;
}

void CGraph::CairoGraph::add_line_colours(const std::vector<Gdk::RGBA>& colours)
{
    //! Set the line colours of a multi series.
    //! If line colours are not specified colours will be assigned automatically.

    seriescolour.clear();
    for (auto &i : colours) seriescolour.emplace_back(i);
}

void CGraph::CairoGraph::set_line_style(const size_t seriesnum, const CairoGraphLineStyle style)
{
    //! Update the line style of plot seriesnum to CairoGraphLineStyle style
    //! Only use when a graph has already been added and you want ot change line style

    if (seriesnum > serieslinestyle.size() - 1) return;

    serieslinestyle[seriesnum] = style;
}

void CGraph::CairoGraph::add_line_styles(const std::vector<CairoGraphLineStyle>& styles)
{
   //! set the line style of each plot in the series
   //! Option are CairoGraphLineStyle::SOLID_LINE, DASHED_LINE, DOTS, CIRCLE 

   serieslinestyle.clear();
   for (auto &i : styles) serieslinestyle.emplace_back(i);
}

void CGraph::CairoGraph::set_graph_box_style(const CairoGraphBoxStyle style)
{
    //! The style of the graph box
    //! Options are enum class CairoGraphBoxStyle::AXES_GRID, BOX_GRID, AXES_ONLY, BOX_ONLY

    graphboxstyle = style;
}

void CGraph::CairoGraph::use_scientific_notation(const bool scientificx, const bool scientificy)
{
    //! Use scienfici notation for the x or y axis
    //! scientificx: if true X axes labels will be displayed in scientific notation
    //! scientificY: if true Y axes labels will be displayed in scientific notation

    forcescientificx = scientificx;
    forcescientificy = scientificy;
}

void CGraph::CairoGraph::set_tick_label_format_x(const bool showpoint, const int precision)
{
    //! Override the default behaviour and force the format of axes labels
    //! showpoint: always show a decimal point if true, the second argument will be ignored
    //! precision: Number of decimal places (6 is the maximum ) if showpoint is set to false

    if (precision > 6) precisionx = 6;

    showpointx = showpoint;
    precisionx = precision;
}

void CGraph::CairoGraph::set_tick_label_format_y(const bool showpoint, const int precision)
{
    //! Override the default behaviour and force the format of axes labels
    //! showpoint: always show a decimal point if true, the second argument will be ignored
    //! precision: Number of decimal places (6 is the maximum ) if showpoint is set to false

    if (precision > 6) precisiony = 6;

    showpointy = showpoint;
    precisiony = precision;
}

void CGraph::CairoGraph::add_single_legend(const Glib::ustring&legend, CairoGraphPos pos, const bool showlinecolour)
{
    //! Display a legend, used for single series plots.
    //! CairoGraphPos pos: Displayed at postion pos, LEGEND_TOP or LEGEND_BOTTOM
    //! bool showlinecolour: Whether to show the line colour indicator.

    graph_legend.clear();
    graph_legend = legend;
    legend_pos   = pos;
    legend_show_colour = showlinecolour;
}

void CGraph::CairoGraph::show_legend(const bool show)
{
    //! Whether to display legends (if legends have been supplied).
    //! Useful to toggle them on and off.

    showlegend = show;
}

void CGraph::CairoGraph::set_legend_scale(const double scale)
{
    //! Determined the size of legends: 1 is the default scale. 
    //! < 1 decreases scale 
    //! > 1 increases scale.

    legend_scale = scale;
}

void CGraph::CairoGraph::add_multi_legends(const std::vector<Glib::ustring> &legends, const double offsetx, const double offsety)
{
    //! used for multi series plots. A vector of legends for 0 to n plots.
    //! Displayed at offset x and y, w.r.t. graph top left corner.
    //! if text is empty for any entry it will be skipped, useful for displaying legends for specific plots in a multi series.

    graph_legends.clear();
    for ( auto &i: legends) graph_legends.emplace_back(i);
    legend_offsetx = offsetx * GRAPH_WIDTH;
    legend_offsety = offsety * GRAPH_HEIGHT;
}

void CGraph::CairoGraph::add_text_objects(const std::vector<std::tuple<Glib::ustring, double, double, double, bool> >& text)
{
    //! Add a text object to the graph
    //! string: text
    //! double x with respect to the graph box top left corner
    //! double y with respect to the graph box top left corner
    //! double scale text
    //! bool: Flag to show text if the graph is zoomed: true shows text at all zoom levels. False only shows text when NOT zoomed.

    for (auto &i : text) text_objects.emplace_back(i);  
}