#include "plotter.hpp"

#include <cmath>
#include <iostream>
#include <limits>

using namespace CarioGraphConstants;

void CGraph::CairoGraph::add_multi_series(  const std::vector<std::vector<double>> &xvalues, 
                                            const std::vector<std::vector<double>> &yvalues, 
                                            bool make_copy  )
{
    //! Add multiple plots. This can only be called once per graph. Can be used for single or multiple plots 
    //! xvalues:   A vector of vectors of X values for 1 .. n plots.
    //! yvalues:   A vector of vectors of Y values for 1 .. n plots.
    //! make_copy: Whether to copy the X and Y values (true) or access values by reference (false), true is the default.
    //! The latter is useful for large data sets. Data will not be modified.
    //! ***************************************************
    //! line styles and colours are assigned automatically. 
    //! Use add_line_styles and add_line_colours to override, these must be called before this method.
    //! Use set_line_style set_line_colour to update existing plots.

    clear_series();

    for (size_t j = 0; j < xvalues.size(); ++j)
    {   
        numpoints.emplace_back(xvalues[j].size());

        if(numpoints[0] < 2)
        {
            std::clog << "Must have at least 2 points in every plot." << '\n';
            return;
        }

        if (true == make_copy)
        {
            seriesx.emplace_back(xvalues[j]);
            seriesy.emplace_back(yvalues[j]);
            m_px.emplace_back(seriesx[j].data());
            m_py.emplace_back(seriesy[j].data());
        }
        else
        {
            m_px.emplace_back(const_cast<double *>(xvalues[j].data()));
            m_py.emplace_back(const_cast<double *>(yvalues[j].data()));
        }

        for (size_t i = 0; i < xvalues[j].size(); ++i)
        {
            if (xvalues[j][i] < xmin) xmin = xvalues[j][i];
            if (xvalues[j][i] > xmax) xmax = xvalues[j][i];
            if (yvalues[j][i] < ymin) ymin = yvalues[j][i];
            if (yvalues[j][i] > ymax) ymax = yvalues[j][i];
        }
    }

    plot.xmin = xmin;
    plot.xmax = xmax;
    plot.ymin = ymin;
    plot.ymax = ymax;

    if(seriescolour.size() < numpoints.size()) // The user didn't set colours for one or more plots so auto assign
        set_series_colours();

    if (serieslinestyle.size() < numpoints.size()) // The user didn't set linestyles for one or more plots so auto assign
        for (size_t i = serieslinestyle.size(); i < numpoints.size(); ++i) 
            serieslinestyle.emplace_back(CairoGraphLineStyle::SOLID_LINE);
}

void CGraph::CairoGraph::add_series(const std::vector<double> &xvalues, 
                                    const std::vector<double> &yvalues,
                                    const Gdk::RGBA linecolour, 
                                    const CairoGraphLineStyle style, 
                                    bool make_copy)
{
    //! Add a single plot. This can only be called once per graph. for multiple plots use add_multi_series 
    //! xvalues: a  vector of X values
    //! yvalues: a  vector of Y values
    //! linecolour: RGBA value for line colour
    //! style:     The line style, CairoGraphLineStyle::SOLID_LINE, DASHED_LINE CIRCLES or DOTS
    //! make_copy: Whether to copy the X and Y values (true) or access values by reference (false), true is the default.
    //! The latter is useful for large data sets. Data will not be modified.


    clear_series();

    if (xvalues.size() < 2 || yvalues.size() < 2) 
    {
        std::clog << "Must have at least 2 points." << '\n';
        return;
    }

    if (xvalues.size() != yvalues.size()) 
    {
        std::clog << "X and Y values must be the same length." << '\n';
        return;
    }

    numpoints.emplace_back(xvalues.size());

    if (true == make_copy)
    {
        seriesx.emplace_back(xvalues);
        seriesy.emplace_back(yvalues);
        m_px.emplace_back(seriesx[0].data());
        m_py.emplace_back(seriesy[0].data());
    }
    else
    {
        m_px.emplace_back(const_cast<double *>(xvalues.data()));
        m_py.emplace_back(const_cast<double *>(yvalues.data()));
    }

    for (size_t i = 0; i < xvalues.size(); ++i)
    {
        if (xvalues[i] < xmin)
            xmin = xvalues[i];
        if (xvalues[i] > xmax)
            xmax = xvalues[i];
        if (yvalues[i] < ymin)
            ymin = yvalues[i];
        if (yvalues[i] > ymax)
            ymax = yvalues[i];
    }
    
    plot.xmin = xmin;
    plot.xmax = xmax;
    plot.ymin = ymin;
    plot.ymax = ymax;

    seriescolour.emplace_back(linecolour);
    serieslinestyle.emplace_back(style);
}

void CGraph::CairoGraph::init_plots(size_t numplots)
{
    //! initialise the number of plots to be added to the graph.
    //! This does not need to be called when using add_series or add add_multi_series
    //! Only use when building plots point by point using the method add_point.
    //! numplots: The number of plots

    clear_series();
    
    numpoints.resize(numplots);
    seriesx.resize(numplots);
    seriesy.resize(numplots);
    m_px.resize(numplots);
    m_py.resize(numplots);

    set_series_colours();
    // If line styles have not been set auto assign
    for ( size_t i = serieslinestyle.size(); i < numplots; ++i) 
        serieslinestyle.emplace_back(CairoGraphLineStyle::SOLID_LINE);

}

void CGraph::CairoGraph::add_point(size_t seriesnum, const double x, const double y, bool update_minmax)
{
    //! Build a single plot point by point. Do not mix with add_series or add_multi_series.
    //! the plot number, stars at zero up to the number of plots - 1.
    //! x: The x value
    //! y: The y value
    //! y: Whether to update the minimum and maximum plot range, true by default.
    
    seriesx[seriesnum].emplace_back(x);
    seriesy[seriesnum].emplace_back(y);
    m_px[seriesnum] = seriesx[seriesnum].data();
    m_py[seriesnum] = seriesy[seriesnum].data();
    numpoints[seriesnum] = seriesx[seriesnum].size();

    if ( true == update_minmax)
    {
        if ( x < xmin) xmin = x;
        if ( x > xmax) xmax = x;
        if ( y < ymin) ymin = y;
        if ( y > ymax) ymax = y;
    }
}

void CGraph::CairoGraph::clear_series()
{
    // add and subtract epsilon to avoid out of range exception from std::stod
    xmax = ymax = std::numeric_limits<double>::lowest() + std::numeric_limits<double>::epsilon();
    xmin = ymin = std::numeric_limits<double>::max() - std::numeric_limits<double>::epsilon();
    text_object_font_family = "";
    seriescolour.clear();
    serieslinestyle.clear();
    graph_legend.clear();
    graph_legends.clear();
    seriesx.clear();
    seriesy.clear();
    m_px.clear();
    m_py.clear();
    numpoints.clear();
    seriescolour.clear();
    text_objects.clear();
    plot.zoom_factor_x = 1.0;
    plot.zoom_factor_y = 1.0;
    legend_offsetx = 0.0;
    legend_offsety = 0.0;
    legend_scale = 1.0;
    draw_zoom = false;
    selection_mode = false;
}