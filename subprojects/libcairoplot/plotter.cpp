#include "plotter.hpp"
#include <iomanip>
#include <iostream>

using namespace CarioGraphConstants;

CGraph::CairoGraph::CairoGraph()
{   
    grid    = Gtk::make_managed<Gtk::Grid>();
    cursor_grid = Gtk::make_managed<Gtk::Grid>();
    xvalue  = Gtk::make_managed<Gtk::Entry>();
    yvalue  = Gtk::make_managed<Gtk::Entry>();
    xvaluelabel = Gtk::make_managed<Gtk::Label>();
    yvaluelabel = Gtk::make_managed<Gtk::Label>();

    add_events( Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);

    auto display = Gdk::Display::get_default();
    cross_hair_cursor = Gdk::Cursor::create(display, Gdk::CROSSHAIR); // graph cursor
    
    bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0); //  default background gradient
    bg_colour2.set_rgba(0.28, 0.28, 0.28, 1.0); // default background gradient
    axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);

    graphboxstyle = CairoGraphBoxStyle::BOX_GRID; // default graph style
    
    set_size_request(start_height, start_height);
}

void CGraph::CairoGraph::set_title(const Glib::ustring& title)
{
    graph_title.clear();
    graph_title = title;
}

bool CGraph::CairoGraph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    Gtk::Allocation allocation = get_allocation();
    const int w = allocation.get_width();
    const int h = allocation.get_height();

    cr->rectangle(0, 0, get_allocation().get_width(), get_allocation().get_height());
    cr->clip();

    cr->scale(w, h); // We will often work in a unit square (scaled coordinates) when useful

    // Create the linear gradient top to bottom

    if (selection_mode == false)
    {
        // we create a surface to write on and restore it when zooming
        // instead of redrawing eveything
        cr->save();
        cr->scale(1.0 / w, 1.0 / h);
        canvas.clear(); // needed ?
        canvas = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w, h);
        auto context = Cairo::Context::create(canvas);
        cr->restore();
        gradient = Cairo::LinearGradient::create(0.0, 0.0, 0.0, 1.0);

        // Set grandient colors
        gradient->add_color_stop_rgba(0.0, bg_colour1.get_red(), bg_colour1.get_green(), bg_colour1.get_blue(), bg_colour1.get_alpha());
        gradient->add_color_stop_rgba(1.0, bg_colour2.get_red(), bg_colour2.get_green(), bg_colour2.get_blue(), bg_colour2.get_alpha());

        // draw gradient
        cr->set_source(gradient);
        cr->rectangle(0.0, 0.0, 1.0, 1.0);
        cr->fill();

        // Draw axes and gridlines
        cr->save();
        cr->scale(1.0 / w, 1.0 / h);
        cr->set_line_join(Cairo::LINE_JOIN_ROUND);
        cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), axes_colour.get_alpha());
        cr->set_line_width(BOX_LINEWIDTH);

        if (CairoGraphBoxStyle::BOX_GRID == graphboxstyle || CairoGraphBoxStyle::BOX_ONLY == graphboxstyle)
        {
            cr->rectangle(w * OFFSET_X, h * OFFSET_Y, w * GRAPH_WIDTH, h * GRAPH_HEIGHT);
        }
        else
        {
            cr->move_to(w * OFFSET_X, h * OFFSET_Y);
            cr->line_to(w * OFFSET_X, h * (OFFSET_Y + GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
        }
        cr->stroke();

        if (graphboxstyle == CairoGraphBoxStyle::AXES_GRID || graphboxstyle == CairoGraphBoxStyle::BOX_GRID)
        {
            // Create major grid lines
            cr->set_dash(dashes1, 0.0);
            cr->set_line_width(0.4);
            cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 0.75 * axes_colour.get_alpha());
            cr->move_to(w * OFFSET_X, h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
            cr->move_to(w * (OFFSET_X + 0.5 * GRAPH_WIDTH), h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + 0.5 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
            cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.5 * GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + 0.5 * GRAPH_HEIGHT));
            cr->stroke();

            // create minor gridlines
            cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 0.60 * axes_colour.get_alpha());
            cr->set_dash(dashes2, 0.0);
            cr->move_to(w * (OFFSET_X + 0.25 * GRAPH_WIDTH), h * (OFFSET_Y));
            cr->line_to(w * (OFFSET_X + 0.25 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT)); // quarter graph
            cr->move_to(w * (OFFSET_X + 0.75 * GRAPH_WIDTH), h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + 0.75 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT)); // 3/4 graph etc.
            cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.25 * GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + 0.25 * GRAPH_HEIGHT));
            cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.75 * GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + 0.75 * GRAPH_HEIGHT));
            cr->stroke();
            cr->unset_dash();
        }

        // Draw major tickmarks Y axis
        cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), axes_colour.get_alpha());
        cr->set_line_width(BOX_LINEWIDTH);
        cr->move_to(w * OFFSET_X, h * OFFSET_Y);
        cr->line_to(w * (OFFSET_X - TICKS_LENGTH), h * OFFSET_Y);
        cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.25 * GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X - TICKS_LENGTH), h * (OFFSET_Y + 0.25 * GRAPH_HEIGHT));
        cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.50 * GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X - TICKS_LENGTH), h * (OFFSET_Y + 0.50 * GRAPH_HEIGHT));
        cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.75 * GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X - TICKS_LENGTH), h * (OFFSET_Y + 0.75 * GRAPH_HEIGHT));
        cr->move_to(w * OFFSET_X, h * (OFFSET_Y + GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X - TICKS_LENGTH), h * (OFFSET_Y + GRAPH_HEIGHT));

        // Draw major tickmarks X axis
        cr->move_to(w * OFFSET_X, h * (OFFSET_Y + GRAPH_HEIGHT));
        cr->line_to(w * OFFSET_X, h * (OFFSET_Y + GRAPH_HEIGHT + TICKS_LENGTH));
        cr->move_to(w * (OFFSET_X + 0.25 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X + 0.25 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT + TICKS_LENGTH));
        cr->move_to(w * (OFFSET_X + 0.50 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X + 0.50 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT + TICKS_LENGTH));
        cr->move_to(w * (OFFSET_X + 0.75 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X + 0.75 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT + TICKS_LENGTH));
        cr->move_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
        cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT + TICKS_LENGTH));
        cr->stroke();
        cr->restore();

        //  plot data
        draw_single_series(cr);
        draw_multi_series(cr);

        // Axes and labels
        create_tickmark_labels(cr);
        create_labels(cr);

        // draw a border for system themes
        if ("Default" == current_theme)
        {
            cr->set_source_rgba(border_colour.get_red(), border_colour.get_green(), border_colour.get_blue(), border_colour.get_alpha());
            cr->set_line_width(0.0030);
            cr->rectangle(0.0, 0.0, 1.0, 1.0);
            cr->stroke();
        }

        // Write everything to the canvas
        cr->save(); 
        cr->scale(1.0 / w, 1.0 / h);
        context->set_source(cr->get_target(), -allocation.get_x(), -allocation.get_y());
        context->paint();
        cr->restore();
    }

    if (true == selection_mode) // TO DO zoomstack to allow for more than one zoom level
    {
        // restore previously drawn canavas
        cr->scale(1.0 / w, 1.0 / h);
        cr->set_source(canvas, 0.0, 0.0);
        cr->paint();
        cr->scale(w, h);
        
        // Draw the selection rectangle 
        if (axes_colour.get_red() < 0.25 && axes_colour.get_green() < 0.25 && axes_colour.get_blue() < 0.25)
            cr->set_source_rgba(0.1, 0.1, 0.1, 0.25); // probably a light theme
        else
            cr->set_source_rgba(0.0, 1.0, 1.0, 0.25);
            
        cr->set_line_width(0.0005);
        cr->rectangle(start_x, start_y, end_x - start_x, end_y - start_y);
        cr->fill();
    }

    return true;
}

void CGraph::CairoGraph::draw_multi_series(const Cairo::RefPtr<Cairo::Context> &cr)
{
    if(!seriesy.size() || !seriesy.size()) return;

    cr->set_line_width(0.0025);
    cr->save();
    cr->rectangle(OFFSET_X, OFFSET_Y, GRAPH_WIDTH, GRAPH_HEIGHT); 
    cr->clip();


    if (true == draw_zoom) // to do allow for more zoom levels currently one
    {
        if (plot.zoom_start_x > plot.zoom_end_x) std::swap(plot.zoom_start_x, plot.zoom_end_x);
        if (plot.zoom_start_y > plot.zoom_end_y) std::swap(plot.zoom_start_y, plot.zoom_end_y);
        
        cr->translate(OFFSET_X - plot.zoom_start_x  / plot.zoom_factor_x , OFFSET_Y - plot.zoom_start_y / plot.zoom_factor_y);

        plot.xmin = xmin + (plot.zoom_start_x - OFFSET_X) * (xmax - xmin) / GRAPH_WIDTH;
        plot.xmax = xmin + (plot.zoom_end_x - OFFSET_X) * (xmax - xmin) / GRAPH_WIDTH;
        plot.ymin = ymin + (GRAPH_HEIGHT + OFFSET_Y - plot.zoom_end_y) * (ymax - ymin) / GRAPH_HEIGHT;
        plot.ymax = ymin + (GRAPH_HEIGHT + OFFSET_Y - plot.zoom_start_y) * (ymax - ymin) / GRAPH_HEIGHT;
        plot.zoomed = true;
    }
    else
    {
        plot.xmin = xmin;
        plot.xmax = xmax;
        plot.ymin = ymin;
        plot.ymax = ymax;
    }

    for (size_t j = 0; j < seriesy.size(); ++j)
    {
        cr->begin_new_path();
        cr->move_to(seriesx[j][0] / plot.zoom_factor_x, seriesy[j][0] / plot.zoom_factor_y);

        cr->set_source_rgba(seriescolour[j].get_red(), seriescolour[j].get_green(),
                            seriescolour[j].get_blue(), seriescolour[j].get_alpha());

        if (CairoGraphLineStyle::DASHED_LINE == serieslinestyle[j])
            cr->set_dash(dashes3, 0);
        
        if (CairoGraphLineStyle::CIRCLE == serieslinestyle[j])
            cr->arc(seriesx[j][0] / plot.zoom_factor_x, seriesy[j][0] / plot.zoom_factor_y, 0.002, 0.0, 2 * M_PI);
        
        if (CairoGraphLineStyle::DOTS == serieslinestyle[0])
            cr->arc(seriesx[j][0] / plot.zoom_factor_x, seriesy[j][0] / plot.zoom_factor_y, 0.0005, 0.0, 2 * M_PI);

        for (size_t i = 1; i < seriesy[j].size(); ++i)
        {
            if (CairoGraphLineStyle::SOLID_LINE == serieslinestyle[j] || CairoGraphLineStyle::DASHED_LINE == serieslinestyle[0])
            {
                cr->line_to(seriesx[j][i] / plot.zoom_factor_x, seriesy[j][i] / plot.zoom_factor_y);
            }
            else if (CairoGraphLineStyle::CIRCLE == serieslinestyle[j])
            {
                cr->move_to(seriesx[j][i] / plot.zoom_factor_x, seriesy[j][i] / plot.zoom_factor_y);
                cr->arc(seriesx[j][i] / plot.zoom_factor_x, seriesy[j][i] / plot.zoom_factor_y, 0.002, 0.0, 2 * M_PI);
            }
            else if (CairoGraphLineStyle::DOTS == serieslinestyle[j])
            {
                cr->move_to(seriesx[j][i] / plot.zoom_factor_x, seriesy[j][i] / plot.zoom_factor_y);
                cr->arc(seriesx[j][i] / plot.zoom_factor_x, seriesy[j][i] / plot.zoom_factor_y, 0.0005, 0.0, 2 * M_PI);
            }
        }
        cr->stroke();
        cr->unset_dash();
    }
    cr->restore();
}

void CGraph::CairoGraph::draw_single_series(const Cairo::RefPtr<Cairo::Context> &cr)
{
    if(m_xvalues.size() < 2 || m_yvalues.size() < 2) return;

    cr->set_line_width(0.0025);
    cr->save();
    cr->rectangle(OFFSET_X, OFFSET_Y, GRAPH_WIDTH, GRAPH_HEIGHT); 
    cr->clip();

   if (true == draw_zoom) // to do allow for more zoom levels currently one
    {
        if (plot.zoom_start_x > plot.zoom_end_x) std::swap(plot.zoom_start_x, plot.zoom_end_x);
        if (plot.zoom_start_y > plot.zoom_end_y) std::swap(plot.zoom_start_y, plot.zoom_end_y);
        
        cr->translate(OFFSET_X - plot.zoom_start_x  / plot.zoom_factor_x , OFFSET_Y - plot.zoom_start_y / plot.zoom_factor_y);

        plot.xmin = xmin + (plot.zoom_start_x - OFFSET_X) * (xmax - xmin) / GRAPH_WIDTH;
        plot.xmax = xmin + (plot.zoom_end_x - OFFSET_X) * (xmax - xmin) / GRAPH_WIDTH;
        plot.ymin = ymin + (GRAPH_HEIGHT + OFFSET_Y - plot.zoom_end_y) * (ymax - ymin) / GRAPH_HEIGHT;
        plot.ymax = ymin + (GRAPH_HEIGHT + OFFSET_Y - plot.zoom_start_y) * (ymax - ymin) / GRAPH_HEIGHT;
        plot.zoomed = true;
    }
    else
    {
        plot.xmin = xmin;
        plot.xmax = xmax;
        plot.ymin = ymin;
        plot.ymax = ymax;
    }

    cr->set_source_rgba(seriescolour[0].get_red(), seriescolour[0].get_green(), 
                        seriescolour[0].get_blue(), seriescolour[0].get_alpha());

    cr->move_to(m_xvalues[0] / plot.zoom_factor_x, m_yvalues[0] / plot.zoom_factor_y);

    if (CairoGraphLineStyle::DASHED_LINE == serieslinestyle[0])
        cr->set_dash(dashes3, 0);

    if (CairoGraphLineStyle::CIRCLE == serieslinestyle[0])
        cr->arc(m_xvalues[0] / plot.zoom_factor_x, m_yvalues[0] / plot.zoom_factor_y, 0.002, 0.0, 2 * M_PI);

    if (CairoGraphLineStyle::DOTS == serieslinestyle[0])
        cr->arc(m_xvalues[0] / plot.zoom_factor_x, m_yvalues[0] / plot.zoom_factor_y, 0.0005, 0.0, 2 * M_PI);

    for (size_t i = 1; i < m_xvalues.size(); ++i)
    {
        if (CairoGraphLineStyle::SOLID_LINE == serieslinestyle[0] || CairoGraphLineStyle::DASHED_LINE == serieslinestyle[0])
        {
            cr->line_to(m_xvalues[i] / plot.zoom_factor_x, m_yvalues[i] / plot.zoom_factor_y);
        }
        else if (CairoGraphLineStyle::CIRCLE == serieslinestyle[0])
        {
            cr->move_to(m_xvalues[i] / plot.zoom_factor_x, m_yvalues[i] / plot.zoom_factor_y);
            cr->arc(m_xvalues[i] / plot.zoom_factor_x, m_yvalues[i] / plot.zoom_factor_y, 0.002, 0.0, 2 * M_PI);
        }
        else if (CairoGraphLineStyle::DOTS == serieslinestyle[0])
        {
            cr->move_to(m_xvalues[i] / plot.zoom_factor_x, m_yvalues[i] / plot.zoom_factor_y);
            cr->arc(m_xvalues[i] / plot.zoom_factor_x, m_yvalues[i] / plot.zoom_factor_y, 0.0005, 0.0, 2 * M_PI);
        }
    }
    cr->stroke();
    cr->restore();
}

void CGraph::CairoGraph::add_multi_series(const std::vector<std::vector<double>> &xvalues, const std::vector<std::vector<double>> &yvalues)
{
    // map data to graph coordinates, setup plot data

    clear_graph();

    xmax = ymax = std::numeric_limits<double>::min();
    xmin = ymin = std::numeric_limits<double>::max();

    for (size_t j = 0; j < xvalues.size(); ++j)
    {   
        seriesx.emplace_back(xvalues[j]);
        seriesy.emplace_back(yvalues[j]);

        for (size_t i = 0; i < xvalues[j].size(); ++i)
        {
            if (xvalues[j][i] < xmin)
                xmin = xvalues[j][i];
            if (xvalues[j][i] > xmax)
                xmax = xvalues[j][i];
            if (yvalues[j][i] < ymin)
                ymin = yvalues[j][i];
            if (yvalues[j][i] > ymax)
                ymax = yvalues[j][i];
        }
    }

    plot.xmin = xmin;
    plot.xmax = xmax;
    plot.ymin = ymin;
    plot.ymax = ymax;

    for (size_t j = 0; j < seriesx.size(); ++j)
    {
        for (size_t i = 0; i < seriesx[j].size(); ++i)
        {
            seriesx[j][i] = OFFSET_X + ((xvalues[j][i] - plot.xmin) * GRAPH_WIDTH) / (plot.xmax - plot.xmin);
            seriesy[j][i] = OFFSET_Y + GRAPH_HEIGHT - ((yvalues[j][i] - plot.ymin) * GRAPH_HEIGHT) / (plot.ymax - plot.ymin);
        }
    }

    if(seriescolour.size() < seriesy.size()) // The user didn't set colours for one or more plots so auto assign
    {
        if ("Adwaita" == current_theme)
        {
            for(size_t i = seriescolour.size(); i < seriesy.size(); ++i)
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
            for(size_t i = seriescolour.size(); i < seriesy.size(); ++i)
            {
                Gdk::RGBA colour;
                colour.set_rgba(1.0 * static_cast<double>(i + 1) / seriesy.size(), 
                            0.5 + 0.5 * static_cast<double>(i + 1) / seriesy.size(), 
                            0.5 + 0.5 * static_cast<double>(i + 1) / seriesy.size(), 1.0);
                seriescolour.emplace_back(colour);
            }
        }
    }

    if (serieslinestyle.size() < seriesy.size()) // The user didn't set linestyles for one or more plots so auto assign
        for (size_t i = serieslinestyle.size(); i < seriesy.size(); ++i) serieslinestyle.emplace_back(CairoGraphLineStyle::SOLID_LINE);
}

void CGraph::CairoGraph::add_series(const std::vector<double> &xvalues, const std::vector<double> &yvalues,
                            const Gdk::RGBA linecolour, const CairoGraphLineStyle style)
{
    // map data to graph coordinates, setup plot data

    if (xvalues.size() < 2 || yvalues.size() < 2) return;
    if (xvalues.size() != yvalues.size()) return;

    clear_graph();

    xmax = ymax = std::numeric_limits<double>::min();
    xmin = ymin = std::numeric_limits<double>::max();

    size_t i = 0;
    for (auto &iter : xvalues)
    {
        m_xvalues.emplace_back(iter);
        m_yvalues.emplace_back(yvalues[i]);
        
        if (xvalues[i] < xmin)
            xmin = xvalues[i];
        if (xvalues[i] > xmax)
            xmax = xvalues[i];
        if (yvalues[i] < ymin)
            ymin = yvalues[i];
        if (yvalues[i] > ymax)
            ymax = yvalues[i];
        ++i;
    }

    plot.xmin = xmin;
    plot.xmax = xmax;
    plot.ymin = ymin;
    plot.ymax = ymax;

    i = 0;
    for (auto &iter : m_xvalues)
    {
        iter = OFFSET_X + ((xvalues[i] - plot.xmin) * GRAPH_WIDTH) / (plot.xmax - plot.xmin);
        m_yvalues[i] = OFFSET_Y + GRAPH_HEIGHT - ((yvalues[i] - plot.ymin) * GRAPH_HEIGHT) / (plot.ymax - plot.ymin);
        ++i;
    }

    seriescolour.emplace_back(linecolour);
    serieslinestyle.emplace_back(style);
}

void CGraph::CairoGraph::clear_graph()
{
    seriescolour.clear();
    serieslinestyle.clear();
    graph_legend.clear();
    graph_legends.clear();
    m_xvalues.clear();
    m_yvalues.clear();
    seriesx.clear();
    seriesy.clear();
    seriescolour.clear();
    text_objects.clear();
    plot.zoom_factor_x = 1.0;
    plot.zoom_factor_y = 1.0;
    plot.zoomed = false;
    legend_offsetx = 0.0;
    legend_offsety = 0.0;
    legend_scale = 1.0;
    draw_zoom = false;
    selection_mode = false;

}

void CGraph::CairoGraph::update_graph()
{
     queue_draw_area(0, 0, get_allocation().get_width(), get_allocation().get_height());
}

void CGraph::CairoGraph::get_preferred_width_vfunc(int &minimum_width, int &natural_width) const
{
    minimum_width = 256;
    natural_width = 580;
}

void CGraph::CairoGraph::get_preferred_height_vfunc(int &minimum_height, int &natural_height) const
{
    minimum_height = 256;
    natural_height = 512;
}

void CGraph::CairoGraph::get_preferred_width_for_height_vfunc(int, int &minimum_width, int &natural_width) const
{
    minimum_width = 256;
    natural_width = 580;
}

void CGraph::CairoGraph::get_preferred_height_for_width_vfunc(int, int &minimum_height, int &natural_height) const
{
    minimum_height = 256;
    natural_height = 512;
}