#pragma once
#include <gdkmm/pixbuf.h>
#include <gdkmm/general.h>
#include <cairomm/context.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm.h>
#include <glibmm.h>
#include <cairomm/cairomm.h>
#include <gtkmm/window.h>
#include <glibmm/i18n.h>

namespace CarioGraphConstants
{
    constexpr double BOX_LINEWIDTH = 1.5;
    constexpr double GRID_LINEWIDTH = 0.0005; // in scaled coordinates
    constexpr double OFFSET_X = 0.1950;
    constexpr double OFFSET_Y = 0.1000;
    constexpr double GRAPH_WIDTH = 0.700;
    constexpr double GRAPH_HEIGHT = 0.720;
    constexpr double TICKS_LENGTH = 0.010;
    const std::vector<double> dashes1{5.0, 2.5};
    const std::vector<double> dashes2{2.5, 2.5};
    const std::vector<double> dashes3{0.008, 0.012};
    constexpr int start_height = 256;
    constexpr int start_width = 256;

}; // namespace CarioGraphConstants

enum class CairoGraphLineStyle
{
    SOLID_LINE,
    DASHED_LINE,
    DOTS,
    CIRCLE
};

enum class CairoGraphBoxStyle
{
    AXES_GRID,
    BOX_GRID,
    AXES_ONLY,
    BOX_ONLY
};

enum class CairoGraphPos
{
    LEGEND_TOP,
    LEGEND_BOTTOM
};

namespace CGraph
{
    class CairoGraph : public Gtk::DrawingArea
    {
    public:
        explicit CairoGraph();
        ~CairoGraph() {}

        // thou shallt not do the following
        CairoGraph(const CairoGraph &) = delete;
        CairoGraph(CairoGraph &&) = delete;
        CairoGraph &operator=(const CairoGraph &other) = delete;
        CairoGraph &operator=(CairoGraph &&) = delete;

        // public methods
        // complete graph with motion tracker
        Gtk::Grid &create_graph();
        
        // motion tracker widget that diplays the x and y values following the cursor
        Gtk::Grid &get_motion_tracker();

        // widgets one can use and have access to for custom layouts
        Gtk::Grid &get_cursor_grid() {return *cursor_grid;}
        Gtk::Grid &get_graph_grid() {return *grid;}
        Gtk::Entry &get_xvalue_entry() {return *xvalue;}
        Gtk::Entry &get_yvalue_entry() {return *yvalue;}
        Gtk::Label &get_xvalue_label() {return *xvaluelabel;}
        Gtk::Label &get_yvalue_label() {return *yvaluelabel;}

        // graph methods
        void update_graph();
        void set_background_style(const Gdk::RGBA colour1, const Gdk::RGBA colour2);
        void set_background_style(const Gdk::RGBA colour1);
        void set_axes_labels(const Glib::ustring &xlabel, const Glib::ustring &ylabel, Glib::ustring fontfamily = _("Nimbus Roman"));
        void set_tick_label_format_x(const bool showpointx, const int precision);
        void set_tick_label_format_y(const bool showpointy, const int precision);
        void add_multi_series(const std::vector<std::vector<double>> &xvalues, const std::vector<std::vector<double>> &yvalues);

        void add_series(const std::vector<double> &xvalues, const std::vector<double> &yvalues,
                        const Gdk::RGBA linecolour, const CairoGraphLineStyle style);

        void add_multi_legends(const std::vector<Glib::ustring> &legends, const double offsetx = 0.0, const double offsety = 0.0);
        void add_single_legend(const Glib::ustring &legend, CairoGraphPos pos, const bool showlinecolour);
        void show_legend(const bool show);
        void set_legend_scale(const double scale);
        void add_text(/*const Glib::ustring& text, const double x, const double y */); // todo
        const Glib::ustring &get_theme_name() const;
        void force_scientific_notation(const bool scientificx, const bool scientificy);
        void set_line_colour(const size_t seriesnum, Gdk::RGBA colour);
        void add_line_colours(const std::vector<Gdk::RGBA> &colours);
        void set_axes_colour(const Gdk::RGBA colour);
        void set_line_style(const size_t seriesnum, const CairoGraphLineStyle style);
        void add_line_styles(const std::vector<CairoGraphLineStyle> &style);
        void set_graph_box_style(const CairoGraphBoxStyle style);
        void set_title(const Glib::ustring &title);
        void set_theme(const Glib::ustring &theme, bool automatic = true);
        void add_text_objects(const std::vector<std::tuple<Glib::ustring, double, double, double, bool> >& text);

    private:
        double xmax;
        double xmin;
        double ymax;
        double ymin;
        double start_x;
        double start_y;
        double end_x;
        double end_y;
        double legend_offsetx = 0.0;
        double legend_offsety = 0.0;
        double legend_scale = 1.0;
        bool selection_mode = false;
        bool draw_zoom = false;
        bool forcescientificx = false;
        bool forcescientificy = false;
        bool legend_show_colour = true;
        bool showlegend = true;
        bool showpointx = false;
        bool showpointy = false;
        unsigned int precisionx = 0;
        unsigned int precisiony = 0;
        

        struct graph_params
        {
            double xmin;
            double xmax;
            double ymin;
            double ymax;
            double zoom_factor_x;
            double zoom_factor_y;
            double zoom_start_x;
            double zoom_end_x;
            double zoom_start_y;
            double zoom_end_y;
            bool zoomed = false;
        };

        graph_params plot;
        Glib::ustring xmarkuplabel;
        Glib::ustring ymarkuplabel;
        Glib::ustring graph_title;
        Glib::ustring graph_legend;
        Glib::ustring labels_fontfamily;
        std::vector<Glib::ustring> graph_legends;
        std::vector<Gdk::RGBA> seriescolour;
        std::vector<CairoGraphLineStyle> serieslinestyle;
        CairoGraphBoxStyle graphboxstyle;
        CairoGraphPos legend_pos;
        Glib::ustring current_theme;
        Gtk::Grid *grid;
        Gtk::Grid *cursor_grid;
        Gtk::Entry *xvalue;
        Gtk::Entry *yvalue;
        Gtk::Label *xvaluelabel;
        Gtk::Label *yvaluelabel;
        Gdk::RGBA bg_colour1;
        Gdk::RGBA bg_colour2;
        Gdk::RGBA axes_colour;
        Gdk::RGBA border_colour;
        Cairo::RefPtr<Cairo::LinearGradient> gradient;
        Glib::RefPtr<Gdk::Cursor> cross_hair_cursor;
        std::vector<double> m_xvalues;
        std::vector<double> m_yvalues;
        std::vector<std::vector<double>> seriesx;
        std::vector<std::vector<double>> seriesy;
        std::vector<std::tuple<Glib::ustring, double, double, double, bool> > text_objects;
        Cairo::RefPtr<Cairo::ImageSurface> canvas;

        void create_tickmark_labels(const Cairo::RefPtr<Cairo::Context> &cr);
        void create_labels(const Cairo::RefPtr<Cairo::Context> &cr);
        void create_legends(const Cairo::RefPtr<Cairo::Context> &cr);
        void set_font_scale(const double scale, Pango::FontDescription *font, const int h, const int w) const;
        void draw_multi_series(const Cairo::RefPtr<Cairo::Context> &cr);
        void draw_single_series(const Cairo::RefPtr<Cairo::Context> &cr);
        void clear_graph();

        //events
        bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
        bool on_button_press_event(GdkEventButton *event) override;
        bool on_button_release_event(GdkEventButton *event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
    // bool on_leave_notify_event(GdkEventCrossing *event) override;

    protected:
        virtual void get_preferred_width_vfunc(int &minimum_width, int &natural_width) const override;
        virtual void get_preferred_height_vfunc(int &minimum_height, int &natural_height) const override;
        virtual void get_preferred_width_for_height_vfunc(int, int &minimum_width, int &natural_width) const override;
        virtual void get_preferred_height_for_width_vfunc(int, int &minimum_height, int &natural_height) const override;
    };
}