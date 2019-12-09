#include "plotter.hpp"
#include <iostream>

using namespace CarioGraphConstants;

bool CairoGraph::on_button_press_event(GdkEventButton *event)
{
    Gtk::Allocation allocation = get_allocation();
    auto scaled_x =  event->x / allocation.get_width();
    auto scaled_y =  event->y / allocation.get_height();
    
    if (event->type == GDK_BUTTON_PRESS && event->button == 1)
    {
        if (scaled_x > OFFSET_X && scaled_x < OFFSET_X + GRAPH_WIDTH &&
            scaled_y > OFFSET_Y && scaled_y < OFFSET_Y + GRAPH_HEIGHT)
        {
            start_x = scaled_x;
            start_y = scaled_y;
            plot.zoom_start_x = start_x;
            plot.zoom_start_y = start_y;
            selection_mode = true;
        }
    }

    return true;
}

bool CairoGraph::on_button_release_event(GdkEventButton *event)
{
    // to do, allow for more zoom levels, for now disable if greater than one level

    if (event->type == GDK_BUTTON_RELEASE && event->button == 1 && plot.zoom_count < 1)  
    {
        selection_mode = false; 
        plot.zoom_factor_x = fabs(end_x - start_x) / GRAPH_WIDTH;
        plot.zoom_factor_y = fabs(end_y - start_y) / GRAPH_HEIGHT;
        
        if (plot.zoom_factor_x < 0.0001) plot.zoom_factor_x = 0.0001;
        if (plot.zoom_factor_y < 0.0001) plot.zoom_factor_y = 0.0001;

        draw_zoom = true;
        plot.zoom_end_x = end_x;
        plot.zoom_end_y = end_y;
        
        queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
    }
    
    if (event->type == GDK_BUTTON_RELEASE && event->button == 3)
    {
        plot.zoom_factor_x = 1.0;
        plot.zoom_factor_y = 1.0;
        plot.zoom_count = 0;
        draw_zoom = false;
        selection_mode = false;
        queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
    }

    return true;
}

bool CairoGraph::on_motion_notify_event(GdkEventMotion *event)
{

    Gtk::Allocation allocation = get_allocation();
    auto scaled_x = event->x / allocation.get_width();
    auto scaled_y = event->y / allocation.get_height();

    if (scaled_x < OFFSET_X || scaled_x > OFFSET_X + GRAPH_WIDTH ||
        scaled_y < OFFSET_Y || scaled_y > OFFSET_Y + GRAPH_HEIGHT)
    {
        xvalue->set_text("");
        yvalue->set_text("");
        get_window()->set_cursor();
    }
    else
    {
        // Evaluate values from coordinates

        double x = plot.xmin + (scaled_x - OFFSET_X) * (plot.xmax - plot.xmin) / GRAPH_WIDTH;
        double y = plot.ymin + (GRAPH_HEIGHT + OFFSET_Y - scaled_y) * (plot.ymax - plot.ymin) / GRAPH_HEIGHT;
        xvalue->set_text(Glib::ustring::format(x));
        yvalue->set_text(Glib::ustring::format(y));
        get_window()->set_cursor(cross_hair_cursor);
    }

    if (plot.zoom_count > 1) return true; // to do allow for more zoom levels, for now disable if greater than one level

    if (scaled_x > OFFSET_X && scaled_x < OFFSET_X + GRAPH_WIDTH &&
        scaled_y > OFFSET_Y && scaled_y < OFFSET_Y + GRAPH_HEIGHT)
    {
        end_x = scaled_x;
        end_y = scaled_y;
        queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
    }

    return true;
}