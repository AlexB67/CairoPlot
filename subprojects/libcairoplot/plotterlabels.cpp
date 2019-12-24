#include <iomanip>
#include <iostream>
#include "plotter.hpp"

using namespace CarioGraphConstants;

void CGraph::CairoGraph::create_tickmark_labels(const Cairo::RefPtr<Cairo::Context> &cr)
{
    constexpr int numlabels = 5;
    std::array<Glib::ustring, numlabels> labely, labelx;
    std::array<Glib::RefPtr<Pango::Layout>, numlabels> layouty, layoutyexp;
    std::array<Glib::RefPtr<Pango::Layout>, numlabels> layoutx, layoutxexp;
    std::array<Glib::ustring, numlabels> exponentx, exponenty;
    Gtk::Allocation allocation = get_allocation();
    Pango::FontDescription font;
    std::array<Glib::RefPtr<Pango::Layout>, numlabels> layout;
    font.set_family("Cantarell Regular");
    int label_width;
    int label_height; 
    int tmp_height;
    int tmp_width;
    double xpos;
    double ypos;

    const int w = allocation.get_width();
    const int h = allocation.get_height();
    font.set_size(static_cast<int>(4.0 * PANGO_SCALE * w / start_width));

    cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 1.0);

    if (true == forcescientificy)
    {
        labely[0] = Glib::ustring::format(std::scientific, plot.ymax);
        labely[1] = Glib::ustring::format(std::scientific, plot.ymin + 0.75 * (plot.ymax - plot.ymin));
        labely[2] = Glib::ustring::format(std::scientific, plot.ymin + 0.50 * (plot.ymax - plot.ymin));
        labely[3] = Glib::ustring::format(std::scientific, plot.ymin + 0.25 * (plot.ymax - plot.ymin));
        labely[4] = Glib::ustring::format(std::scientific, plot.ymin);
    }
    else if (precisiony > 0 && false == showpointy) // formatted to width
    {
        labely[0] = Glib::ustring::format(std::fixed, std::setprecision(precisiony),  plot.ymax);
        labely[1] = Glib::ustring::format(std::fixed, std::setprecision(precisiony),  plot.ymin + 0.75 * (plot.ymax - plot.ymin));
        labely[2] = Glib::ustring::format(std::fixed, std::setprecision(precisiony),  plot.ymin + 0.50 * (plot.ymax - plot.ymin));
        labely[3] = Glib::ustring::format(std::fixed, std::setprecision(precisiony),  plot.ymin + 0.25 * (plot.ymax - plot.ymin));
        labely[4] = Glib::ustring::format(std::fixed, std::setprecision(precisiony),  plot.ymin);
    }
    else if (true == showpointy) // nicely formatted
    {
        labely[0] = Glib::ustring::format(std::showpoint,  plot.ymax);
        labely[1] = Glib::ustring::format(std::showpoint,  plot.ymin + 0.75 * (plot.ymax - plot.ymin));
        labely[2] = Glib::ustring::format(std::showpoint,  plot.ymin + 0.50 * (plot.ymax - plot.ymin));
        labely[3] = Glib::ustring::format(std::showpoint,  plot.ymin + 0.25 * (plot.ymax - plot.ymin));
        labely[4] = Glib::ustring::format(std::showpoint,  plot.ymin);
    }
    else // raw
    {
        labely[0] = Glib::ustring::format(plot.ymax);
        labely[1] = Glib::ustring::format(plot.ymin + 0.75 * (plot.ymax - plot.ymin));
        labely[2] = Glib::ustring::format(plot.ymin + 0.50 * (plot.ymax - plot.ymin));
        labely[3] = Glib::ustring::format(plot.ymin + 0.25 * (plot.ymax - plot.ymin));
        labely[4] = Glib::ustring::format(plot.ymin);
    }

    for (int i = 0; i < numlabels; ++i) // format scientific notation labels
    {
        auto found = labely[i].lowercase().find('e');
        if (found != Glib::ustring::npos)
        {
            exponenty[i] = labely[i].substr(found + 1, labely[i].length());
            long exp = std::stol(exponenty[i]);
            labely[i] = Glib::ustring::format(std::showpoint, std::stod(labely[i]) / pow(10, exp));
        }
    }

    double y_step = 0.0;
    size_t j = 0;
    for (auto &i : layouty)
    {
        cr->move_to(OFFSET_X - 1.5 * TICKS_LENGTH, OFFSET_Y + y_step);
        cr->scale(1.0 / w,  1.0 / h);
        i = create_pango_layout(labely[j]);
        i->set_font_description(font);
        i->get_pixel_size(label_width, label_height);
        tmp_width = label_width; tmp_height = label_height;
        cr->get_current_point(xpos, ypos);
        cr->move_to(xpos - static_cast<double>(label_width), ypos - static_cast<double>(label_height) / 2.0 );
        i->show_in_cairo_context(cr);
        cr->scale(w ,h);
        y_step += 0.25 * GRAPH_HEIGHT;
        ++j;
    }


    if (exponenty[0].length() && exponenty[0] == exponenty[4])
    {
        cr->move_to(OFFSET_X - TICKS_LENGTH, OFFSET_Y);
        cr->scale(1.0 / w,  1.0 / h);
        layoutyexp[0] = create_pango_layout("x10");
        layoutyexp[0]->set_markup("x10<sup>" + exponenty[0] + "</sup>");
        layoutyexp[0]->set_font_description(font);
        layoutyexp[0]->get_pixel_size(label_width, label_height);
        cr->get_current_point(xpos, ypos);
        cr->move_to(xpos, ypos - label_height);
        layoutyexp[0]->show_in_cairo_context(cr);
        cr->scale(w, h);
    }
    else
    {
        size_t j = 0;
        double offset = 0.0;
        for (auto &i : exponenty)
        {

            if (i.length())
            {
                cr->move_to(OFFSET_X - 1.5 * TICKS_LENGTH, offset + OFFSET_Y);
                cr->scale(1.0 / w,  1.0 / h);
                layoutyexp[j] = create_pango_layout("x10");
                layoutyexp[j]->set_markup("x10<sup>" + i + "</sup>");
                layoutyexp[j]->set_font_description(font);
                cr->get_current_point(xpos, ypos);
                cr->move_to(xpos - tmp_width, ypos -1.5 * tmp_height);
                layoutyexp[j]->show_in_cairo_context(cr);
                cr->scale(w, h);
                offset += 0.25 * GRAPH_HEIGHT;
            }
            ++j;
        }
    }

    if (true == forcescientificx)
    {
        labelx[0] = Glib::ustring::format(std::scientific, plot.xmax);
        labelx[1] = Glib::ustring::format(std::scientific, plot.xmin + 0.75 * (plot.xmax - plot.xmin));
        labelx[2] = Glib::ustring::format(std::scientific, plot.xmin + 0.50 * (plot.xmax - plot.xmin));
        labelx[3] = Glib::ustring::format(std::scientific, plot.xmin + 0.25 * (plot.xmax - plot.xmin));
        labelx[4] = Glib::ustring::format(std::scientific, plot.xmin);
    }
    else if (precisionx > 0 && false == showpointx) // formatted to width
    {
        labelx[0] = Glib::ustring::format(std::fixed, std::setprecision(precisionx),  plot.xmax);
        labelx[1] = Glib::ustring::format(std::fixed, std::setprecision(precisionx),  plot.xmin + 0.75 * (plot.xmax - plot.xmin));
        labelx[2] = Glib::ustring::format(std::fixed, std::setprecision(precisionx),  plot.xmin + 0.50 * (plot.xmax - plot.xmin));
        labelx[3] = Glib::ustring::format(std::fixed, std::setprecision(precisionx),  plot.xmin + 0.25 * (plot.xmax - plot.xmin));
        labelx[4] = Glib::ustring::format(std::fixed, std::setprecision(precisionx),  plot.xmin);
    }
    else if (true == showpointx) // nicely formated
    {
        labelx[0] = Glib::ustring::format(std::showpoint, plot.xmax);
        labelx[1] = Glib::ustring::format(std::showpoint, plot.xmin + 0.75 * (plot.xmax - plot.xmin));
        labelx[2] = Glib::ustring::format(std::showpoint, plot.xmin + 0.50 * (plot.xmax - plot.xmin));
        labelx[3] = Glib::ustring::format(std::showpoint, plot.xmin + 0.25 * (plot.xmax - plot.xmin));
        labelx[4] = Glib::ustring::format(std::showpoint, plot.xmin);
    }
    else // raw
    {
        labelx[0] = Glib::ustring::format(plot.xmax);
        labelx[1] = Glib::ustring::format(plot.xmin + 0.75 * (plot.xmax - plot.xmin));
        labelx[2] = Glib::ustring::format(plot.xmin + 0.50 * (plot.xmax - plot.xmin));
        labelx[3] = Glib::ustring::format(plot.xmin + 0.25 * (plot.xmax - plot.xmin));
        labelx[4] = Glib::ustring::format(plot.xmin);
    }

    for (int i = 0; i < numlabels; ++i) // format scientific notation labels
    {
        auto found = labelx[i].lowercase().find('e');
        if (found != Glib::ustring::npos)
        {
            exponentx[i] = labelx[i].substr(found + 1, labelx[i].length());
            long exp = std::stol(exponentx[i]);
            labelx[i] = Glib::ustring::format(std::showpoint, std::stod(labelx[i]) / pow(10, exp));
        }
    }

    double x_step = GRAPH_WIDTH;
    j = 0;
    for (auto &i : layoutx)
    {
        cr->move_to(OFFSET_X + x_step, OFFSET_Y + GRAPH_HEIGHT + 2.0 * TICKS_LENGTH);
        cr->scale(1.0 / w,  1.0 / h);
        i = create_pango_layout(labelx[j]); 
        i->set_font_description(font);
        i->get_pixel_size(label_width, label_height);
        tmp_height = label_height;
        tmp_width = label_width;
        cr->get_current_point(xpos, ypos);
        cr->move_to(xpos - static_cast<double>(label_width / 2.0), ypos);
        i->show_in_cairo_context(cr);
        cr->scale(w,  h);
        x_step -= 0.25 * GRAPH_WIDTH;
        ++j;
    }

    if (exponentx[0].length() && exponentx[0] == exponentx[4])
    {
        cr->move_to(OFFSET_X + GRAPH_WIDTH + 0.5 * TICKS_LENGTH, OFFSET_Y + GRAPH_HEIGHT);
        cr->scale(1.0 / w,  1.0 / h);
        layoutxexp[0] = create_pango_layout("x10");
        layoutxexp[0]->set_markup("x10<sup>" + exponentx[0] + "</sup>");
        layoutxexp[0]->set_font_description(font);
        layoutxexp[0]->get_pixel_size(label_width, label_height);
        cr->get_current_point(xpos, ypos);
        cr->move_to(xpos, ypos - 0.5 * label_height);
        layoutxexp[0]->show_in_cairo_context(cr);
        cr->scale(w, h);
    }
    else
    {
        size_t j = 0;
        x_step = GRAPH_WIDTH;
        for (auto &i : exponentx)
        {

            if (i.length())
            {
                cr->move_to(OFFSET_X + x_step,  OFFSET_Y + GRAPH_HEIGHT + 2.0 * TICKS_LENGTH);
                cr->scale(1.0 / w,  1.0 / h);
                layoutxexp[j] = create_pango_layout("x10");
                layoutxexp[j]->set_markup("x10<sup>" + exponentx[j] + "</sup>");
                layoutxexp[j]->set_font_description(font);
                cr->get_current_point(xpos, ypos);
                cr->move_to(xpos - static_cast<double>(tmp_width) / 2, ypos + tmp_height);
                layoutxexp[j]->show_in_cairo_context(cr);
                cr->scale(w, h);
                x_step -= 0.25 * GRAPH_WIDTH;
            }
            ++j;
        }
    }
}

void CGraph::CairoGraph::create_labels(const Cairo::RefPtr<Cairo::Context> &cr)
{
    double xpos;
    double ypos;
    int label_width;
    int label_height;
    Pango::FontDescription font;

    Gtk::Allocation allocation = get_allocation();
    const int w = allocation.get_width();
    const int h = allocation.get_height();

    cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 1.0);


    if (xmarkuplabel.length())
    {
        font.set_family(labels_fontfamily);
        cr->move_to(OFFSET_X + 0.5 * GRAPH_WIDTH, 1.0);
        cr->scale(1.0 / w, 1.0 / h);
        set_font_scale(1.0, &font, h, w);
        auto labelxlayout = create_pango_layout(xmarkuplabel);
        labelxlayout->set_markup(xmarkuplabel);
        labelxlayout->set_font_description(font);
        cr->get_current_point(xpos, ypos);
        labelxlayout->get_pixel_size(label_width, label_height);
        cr->move_to(xpos - 0.5 * label_width, ypos - 1.20 * label_height);
        labelxlayout->show_in_cairo_context(cr);
        cr->scale(w, h);
    }
    

    if (ymarkuplabel.length())
    {
        cr->move_to(0, OFFSET_Y + 0.5 * GRAPH_HEIGHT);
        cr->scale(1.0 / w, 1.0 / h);
        auto labelylayout = create_pango_layout(ymarkuplabel);
        labelylayout->set_font_description(font);
        labelylayout->set_markup(ymarkuplabel);
        cr->get_current_point(xpos, ypos);
        labelylayout->get_pixel_size(label_width, label_height);
        cr->move_to(xpos + 5.0, ypos + 0.5 * label_width);
        cr->rotate_degrees(-90.0);
        labelylayout->show_in_cairo_context(cr);
        cr->rotate_degrees(90.0);
        cr->scale(w, h);
    }

    if ( graph_legend.size() && true == showlegend)
    {
        font.set_size(legend_scale * font.get_size());
        cr->scale(1.0 / w, 1.0 / h);
        cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 1.0);
        auto legendlayout =  create_pango_layout(graph_legend);
        legendlayout->set_font_description(font);
        legendlayout->set_markup(graph_legend);
        legendlayout->get_pixel_size(label_width, label_height);

        if (legend_pos == CairoGraphPos::LEGEND_BOTTOM) 
            cr->move_to(w * (OFFSET_X + 4.0 * TICKS_LENGTH), h * (OFFSET_Y + GRAPH_HEIGHT - TICKS_LENGTH) - label_height);
        else
            cr->move_to(w * (OFFSET_X + 4.0 * TICKS_LENGTH), h * (OFFSET_Y + TICKS_LENGTH));
        
        legendlayout->show_in_cairo_context(cr);

        if (true == legend_show_colour) 
            cr->set_source_rgba(seriescolour[0].get_red(), seriescolour[0].get_green(), 
                                seriescolour[0].get_blue(), seriescolour[0].get_alpha());
        else
            cr->set_source_rgba(0.0,0.0, 0.0, 0.0);
        
        cr->set_line_width(2.0);

        if (legend_pos == CairoGraphPos::LEGEND_BOTTOM)
        {
            cr->move_to(w * (OFFSET_X + TICKS_LENGTH), h * (OFFSET_Y + GRAPH_HEIGHT - TICKS_LENGTH) - 0.5 * label_height - 2.0);
            cr->line_to(w * (OFFSET_X + 3.0 * TICKS_LENGTH), h * (OFFSET_Y + GRAPH_HEIGHT - TICKS_LENGTH) - 0.5 * label_height - 2.0);
        }
        else
        {
            cr->move_to(w * (OFFSET_X + TICKS_LENGTH), h * (OFFSET_Y + TICKS_LENGTH) + 0.5 * label_height - 2.0);
            cr->line_to(w * (OFFSET_X + 3.0 * TICKS_LENGTH), h * (OFFSET_Y + TICKS_LENGTH) + 0.5 * label_height - 2.0);
        }

        cr->stroke();
        cr->scale(w, h);
    }

    if (graph_legends.size() && true == showlegend)
    {
        font.set_size(legend_scale * font.get_size());
        double offset = 1.0;
        for ( size_t i = 0; i < graph_legends.size(); ++i)
        {
            if (graph_legends[i].length())
            {
                cr->scale(1.0 / w, 1.0 / h);
                auto legendlayout =  create_pango_layout(graph_legends[i]);
                legendlayout->set_font_description(font);
                legendlayout->set_markup(graph_legends[i]);
                legendlayout->get_pixel_size(label_width,label_height);

                cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), 1.0);
                cr->move_to(w * (legend_offsetx + OFFSET_X + 4.0 * TICKS_LENGTH), 
                            h * (legend_offsety + OFFSET_Y + 0.50 * offset * label_height / h));
                
                legendlayout->show_in_cairo_context(cr);

                cr->set_line_width(2.0);
                cr->set_source_rgba(seriescolour[i].get_red(), seriescolour[i].get_green(), 
                                    seriescolour[i].get_blue(), seriescolour[i].get_alpha());

                cr->move_to(w * (legend_offsetx + OFFSET_X + TICKS_LENGTH), 
                            h * (legend_offsety + OFFSET_Y + TICKS_LENGTH  + 0.50 * offset * label_height / h) + 6.0);
                
                cr->line_to(w * (legend_offsetx + OFFSET_X + 3.0 * TICKS_LENGTH), 
                            h * (legend_offsety + OFFSET_Y + TICKS_LENGTH + 0.50 * offset * label_height / h) + 6.0);
                cr->stroke();

                cr->scale(w, h);
                offset += 2.0;
            }
        }
    }

   
   for (auto &i : text_objects)
   {
        Pango::FontDescription text_font;
        Glib::ustring text = std::get<0>(i);
        double x = std::get<1>(i) * GRAPH_WIDTH + OFFSET_X;
        double y = std::get<2>(i) * GRAPH_HEIGHT + OFFSET_Y;
        double scale = std::get<3>(i);
        cr->move_to(x, y);
        cr->scale(1.0 / w, 1.0 / h);
        cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), axes_colour.get_alpha());
        text_font.set_size(scale *  font.get_size());
        auto textlayout = create_pango_layout(text);
        textlayout->set_font_description(text_font);
        textlayout->set_markup(text);
        textlayout->get_pixel_size(label_width, label_height);
        cr->get_current_point(xpos, ypos);
        cr->move_to(xpos, ypos - label_height);
        if (std::get<4>(i) == draw_zoom && true == showlegend) textlayout->show_in_cairo_context(cr);
        cr->scale(w, h);
   }

    font.set_size(font.get_size() / legend_scale);
    if (graph_title.length())
    {
        cr->set_source_rgba(axes_colour.get_red(), axes_colour.get_green(), axes_colour.get_blue(), axes_colour.get_alpha());
        font.set_family("Cantarell Regular Bold");
        cr->move_to(OFFSET_X + 0.5 * GRAPH_WIDTH, 0.0);
        cr->scale(1.0 / w, 1.0 / h);
        set_font_scale(1.0, &font, h, w);
        auto titlelayout = create_pango_layout(graph_title);
        titlelayout->set_font_description(font);
        titlelayout->get_pixel_size(label_width, label_height);
        cr->get_current_point(xpos, ypos);
        cr->move_to(xpos - 0.5 * label_width, +5.0 * h / start_height);
        titlelayout->show_in_cairo_context(cr);
        cr->scale(w, h);
    }
}

void CGraph::CairoGraph::set_font_scale(const double scale, Pango::FontDescription *font, const int h, const int w) const
{
    if (w > 768 && h > 768)
        font->set_size(static_cast<int>(scale * 21 * PANGO_SCALE));
    else if (w > 640 && h > 640)
        font->set_size(static_cast<int>(scale * 17 * PANGO_SCALE));
    else if (w > 256 && h > 256)
        font->set_size(static_cast<int>(scale * 14 * PANGO_SCALE));
    else
        font->set_size(static_cast<int>(scale * 8 * PANGO_SCALE));
}