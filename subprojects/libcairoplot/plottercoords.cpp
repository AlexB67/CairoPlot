#include "plotter.hpp"

using namespace CarioGraphConstants;

double CGraph::CairoGraph::x_to_graph_coords(const double x) const
{
     return(OFFSET_X + ((x - xmin) * GRAPH_WIDTH) / (xmax - xmin));
}

double CGraph::CairoGraph::y_to_graph_coords(const double y) const
{
    return(OFFSET_Y + GRAPH_HEIGHT - ((y - ymin) * GRAPH_HEIGHT) / (ymax - ymin));
}
