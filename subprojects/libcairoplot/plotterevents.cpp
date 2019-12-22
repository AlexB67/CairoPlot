#include "plotter.hpp"
#include <iostream>

using namespace CarioGraphConstants;

bool CGraph::CairoGraph::on_button_press_event(GdkEventButton *event)
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
            end_x = start_x; // prevents drawing rogue selection box on first click
            end_y = start_y;

            // selection start point in zoomed frame
            double sx = plot.xmin + (start_x - OFFSET_X) * (plot.xmax - plot.xmin) / GRAPH_WIDTH;
            double sy = plot.ymax - (start_y - OFFSET_Y) * (plot.ymax - plot.ymin) / GRAPH_HEIGHT;

            // selection start point in unzoomed frame, we always zoom wrt unzoomed frame coordinates.
            plot.zoom_start_x = OFFSET_X + (sx - xmin)  / (xmax - xmin) * GRAPH_WIDTH;
            plot.zoom_start_y = OFFSET_Y + (ymax - sy)  / (ymax - ymin) * GRAPH_HEIGHT;
            selection_mode = true;
        }
    }

    return true;
}

bool CGraph::CairoGraph::on_button_release_event(GdkEventButton *event)
{
    if (event->type == GDK_BUTTON_RELEASE && event->button == 1)  
    {
        draw_zoom = true;
        
        // selection end point in zoomed frame
        double ex = plot.xmin + (end_x - OFFSET_X) * (plot.xmax - plot.xmin) /  GRAPH_WIDTH;
        double ey = plot.ymax - (end_y - OFFSET_Y) * (plot.ymax - plot.ymin) /  GRAPH_HEIGHT;
        
         // selection start point in unzoomed frame, we always zoom wrt unzoomed frame coordinates.
        plot.zoom_end_x = OFFSET_X + (ex - xmin)  / (xmax - xmin) * GRAPH_WIDTH;
        plot.zoom_end_y = OFFSET_Y + (ymax - ey)  / (ymax - ymin) * GRAPH_HEIGHT;

        plot.zoom_factor_x = fabs(plot.zoom_end_x - plot.zoom_start_x) / GRAPH_WIDTH;
        plot.zoom_factor_y = fabs(plot.zoom_end_y - plot.zoom_start_y) / GRAPH_HEIGHT; 
        selection_mode = false;      
        
        // save guard the user double click and too small a region selected
        if (plot.zoom_factor_x < 0.0001 || plot.zoom_factor_x < 0.0001) return true;
    }
    else if (event->type == GDK_BUTTON_RELEASE && event->button == 3)
    {
        plot.zoom_factor_x = 1.0;
        plot.zoom_factor_y = 1.0;
        draw_zoom = false;
        selection_mode = false;
    }

    queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
    
    return true;
}

bool CGraph::CairoGraph::on_motion_notify_event(GdkEventMotion *event)
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

    if (scaled_x > OFFSET_X && scaled_x < OFFSET_X + GRAPH_WIDTH &&
        scaled_y > OFFSET_Y && scaled_y < OFFSET_Y + GRAPH_HEIGHT && true == selection_mode)
    {
        end_x = scaled_x;
        end_y = scaled_y;
        queue_draw_area(0, 0,  get_allocation().get_width(), get_allocation().get_height());
    }

    return true;
}