#include "plotter.hpp"
#include <iomanip>
#include <iostream>

using namespace CairoGraphConstants;

CGraph::CairoGraph::CairoGraph()
{   
  
    set_content_width(580);
    set_content_height(512);

    set_draw_func(sigc::mem_fun(*this, &CairoGraph::on_draw));

    grid        = Gtk::make_managed<Gtk::Grid>();
    cursor_grid = Gtk::make_managed<Gtk::Grid>();
    xvalue      = Gtk::make_managed<Gtk::Entry>();
    yvalue      = Gtk::make_managed<Gtk::Entry>();
    xvaluelabel = Gtk::make_managed<Gtk::Label>();
    yvaluelabel = Gtk::make_managed<Gtk::Label>();

    auto display = Gdk::Display::get_default();
    cross_hair_cursor = Gdk::Cursor::create("crosshair"); // graph cursor
    
    bg_colour1.set_rgba(0.0, 0.0, 0.0, 1.0); //  default background gradient
    bg_colour2.set_rgba(0.0, 0.0, 0.0, 1.0); // default background gradient
    axes_colour.set_rgba(1.0, 1.0, 1.0, 0.80);
    graphboxstyle = CairoGraphBoxStyle::BOX_GRID; // default graph style

    auto motionctrl = Gtk::EventControllerMotion::create();
    motionctrl->signal_motion().connect(sigc::mem_fun(*this, &CairoGraph::on_motion_notify_event));
    add_controller(motionctrl);

    auto buttonctrl =  Gtk::GestureClick::create();
    buttonctrl->set_button(1);
    buttonctrl->signal_pressed().connect(sigc::mem_fun(*this, &CairoGraph::on_button_press_event));
    buttonctrl->signal_released().connect(sigc::mem_fun(*this, &CairoGraph::on_button_release_event));
    add_controller(buttonctrl);

    auto resetctrl =  Gtk::GestureClick::create();
    resetctrl->set_button(3);
    resetctrl->signal_pressed().connect(sigc::mem_fun(*this, &CairoGraph::reset_event));
    add_controller(resetctrl);
    
    set_size_request(start_height, start_height);
}

void CGraph::CairoGraph::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int, int)
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
        // instead of redrawing everything
        cr->save();
        cr->scale(1.0 / w, 1.0 / h);

        //canvas.clear();
        canvas = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, w, h); // 4.0
        auto context = Cairo::Context::create(canvas);
        cr->restore();
        gradient = Cairo::LinearGradient::create(0.0, 0.0, 0.0, 1.0);

        // Set grandient colors
        gradient->add_color_stop_rgba(0.0, bg_colour1.get_red(), bg_colour1.get_green(), bg_colour1.get_blue(), 
                                      bg_colour1.get_alpha());
        gradient->add_color_stop_rgba(1.0, bg_colour2.get_red(), bg_colour2.get_green(), bg_colour2.get_blue(), 
                                      bg_colour2.get_alpha());

        // draw gradient
        cr->set_source(gradient);
        cr->rectangle(0.0, 0.0, 1.0, 1.0);
        cr->fill();

        // Draw axes and gridlines
        cr->save();
        cr->scale(1.0 / w, 1.0 / h);
        cr->set_line_join(Cairo::Context::LineJoin::ROUND);
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
            cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 
                                0.75 * axes_colour.get_alpha());
            cr->move_to(w * OFFSET_X, h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
            cr->move_to(w * (OFFSET_X + 0.5 * GRAPH_WIDTH), h * OFFSET_Y);
            cr->line_to(w * (OFFSET_X + 0.5 * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
            cr->move_to(w * OFFSET_X, h * (OFFSET_Y + 0.5 * GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X + GRAPH_WIDTH), h * (OFFSET_Y + 0.5 * GRAPH_HEIGHT));
            cr->stroke();

            // create minor gridlines
            cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 
                                0.60 * axes_colour.get_alpha());
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

        cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), axes_colour.get_alpha());
        cr->set_line_width(BOX_LINEWIDTH);

        double ticksy = TICKS_LENGTH;
        double ticksx = TICKS_LENGTH;
        if ( w > h ) ticksy = h * ticksx / w;
        if ( h > w ) ticksx = w * ticksy / h;

        double step = 0.0;

        // Draw major tickmarks Y axis
        for (short i = 0; i < 5; ++i)
        {
            cr->move_to(w * OFFSET_X, h * (OFFSET_Y + step * GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X - ticksy), h * (OFFSET_Y + step * GRAPH_HEIGHT));
            step += 0.25;
        }

        // Draw major tickmarks X axis
        step = 0.0;
        for (short i = 0; i < 5; ++i)
        {
            cr->move_to(w * (OFFSET_X + step * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT));
            cr->line_to(w * (OFFSET_X + step * GRAPH_WIDTH), h * (OFFSET_Y + GRAPH_HEIGHT + ticksx));
            step += 0.25;
        }

        cr->stroke();
        cr->restore();

        //  plot data
        draw_series(cr);

         // Axes and labels
        if (numpoints.size() > 0) create_tickmark_labels(cr);

        create_labels(cr);

        // draw a border for system themes
        if ("Default" == current_theme)
        {
            cr->set_source_rgba(border_colour.get_red(), border_colour.get_green(), 
                                border_colour.get_blue(), border_colour.get_alpha());
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

    if (true == selection_mode)
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

    //return true; 3.0
}

void CGraph::CairoGraph::draw_series(const Cairo::RefPtr<Cairo::Context> &cr)
{
    if(!numpoints.size()) return;

    cr->set_line_width(lwidth * 0.0025);
    cr->save();
    cr->rectangle(OFFSET_X, OFFSET_Y, GRAPH_WIDTH, GRAPH_HEIGHT); 
    cr->clip();

    if (true == draw_zoom)
    {   
        if(plot.zoom_end_x < plot.zoom_start_x) std::swap(plot.zoom_start_x, plot.zoom_end_x);
        if(plot.zoom_end_y < plot.zoom_start_y) std::swap(plot.zoom_start_y, plot.zoom_end_y);

        cr->translate(OFFSET_X - plot.zoom_start_x  / plot.zoom_factor_x , OFFSET_Y - plot.zoom_start_y / plot.zoom_factor_y);

        plot.xmin = xmin + (plot.zoom_start_x - OFFSET_X) * (xmax - xmin) / GRAPH_WIDTH;
        plot.xmax = xmin + (plot.zoom_end_x - OFFSET_X) * (xmax - xmin) / GRAPH_WIDTH;
        plot.ymin = ymin + (GRAPH_HEIGHT + OFFSET_Y - plot.zoom_end_y) * (ymax - ymin) / GRAPH_HEIGHT;
        plot.ymax = ymin + (GRAPH_HEIGHT + OFFSET_Y - plot.zoom_start_y) * (ymax - ymin) / GRAPH_HEIGHT;
    }
    else
    {
        plot.xmin = xmin;
        plot.xmax = xmax;
        plot.ymin = ymin;
        plot.ymax = ymax;
    }

    for (size_t j = 0; j < numpoints.size(); ++j)
    {
        cr->begin_new_path();

        double x = x_to_graph_coords(m_px[j][0]) / plot.zoom_factor_x;
        double y = y_to_graph_coords(m_py[j][0]) / plot.zoom_factor_y;

        cr->move_to(x, y);

        cr->set_source_rgba(seriescolour[j].get_red(), seriescolour[j].get_green(),
                            seriescolour[j].get_blue(), seriescolour[j].get_alpha());

        if (CairoGraphLineStyle::DASHED_LINE == serieslinestyle[j])
            cr->set_dash(dashes3, 0);
        
        if (CairoGraphLineStyle::CIRCLE == serieslinestyle[j])
            cr->arc(x, y, 0.002, 0.0, 2 * M_PI);
        
        if (CairoGraphLineStyle::DOTS == serieslinestyle[0])
            cr->arc(x, y, 0.0005, 0.0, 2 * M_PI);

        for (size_t i = 1; i < numpoints[j]; ++i)
        {
            x = x_to_graph_coords(m_px[j][i]) / plot.zoom_factor_x;
            y = y_to_graph_coords(m_py[j][i]) / plot.zoom_factor_y;

            if (CairoGraphLineStyle::SOLID_LINE == serieslinestyle[j] || CairoGraphLineStyle::DASHED_LINE == serieslinestyle[0])
            {
                cr->line_to(x, y);
            }
            else if (CairoGraphLineStyle::CIRCLE == serieslinestyle[j])
            {
                cr->move_to(x, y);
                cr->arc(x, y, 0.002, 0.0, 2 * M_PI);
            }
            else if (CairoGraphLineStyle::DOTS == serieslinestyle[j])
            {
                cr->move_to(x, y);
                cr->arc(x, y, 0.0005, 0.0, 2 * M_PI);
            }
        }

        cr->stroke();
    }

    cr->restore();
}

void CGraph::CairoGraph::update_graph()
{
    //queue_draw_area(0, 0, get_allocation().get_width(), get_allocation().get_height());
    this->queue_draw();
}
