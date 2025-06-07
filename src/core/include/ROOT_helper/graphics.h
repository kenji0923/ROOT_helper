#ifndef ROOT_HELPER_GRAPHICS_H
#define ROOT_HELPER_GRAPHICS_H


#include <string>
#include <utility>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLine.h>


namespace ROOT_helper
{


struct GraphicsSize
{
    static GraphicsSize current;

    unsigned int pad_pixel_w; unsigned int pad_pixel_h;
    double text_size;
    double title_offset_x; double title_offset_y;
    double title_offset_step_horizontal;
    double top_margin; double right_margin; double bottom_margin; double left_margin;
    double top_margin_with_exponent; double right_margin_with_exponent;
    double margin_step_horizontal; double margin_step_vertcical;
};


static const GraphicsSize g_size_8pt {
    700, 500,
    0.05195,
    1.1, 1.15,
    0.225,
    0.01, 0.005, 0.14, 0.14,
    0.06, 0.07,
    0.019, 0.01
};


static const GraphicsSize g_size_10pt {
    700, 500,
    0.06494,
    1.15, 1.20,
    0.2,
    0.01, 0.005, 0.155, 0.15,
    0.06, 0.07,
    0.025, 0.01
};


void prepare();


static std::pair<unsigned int, unsigned int> get_default_n_pad(const unsigned int n_plot);


TCanvas* create_canvas(const std::string& name, const std::string& title, const unsigned int n_pad_x=1, const unsigned int n_pad_y=1, const unsigned int each_size_x=GraphicsSize::current.pad_pixel_w, const unsigned int each_size_y=GraphicsSize::current.pad_pixel_h);


TCanvas* create_canvas_with_default_pad_matrix(const std::string& name, const std::string& title, const unsigned int n_pad=1, const unsigned int each_size_x=GraphicsSize::current.pad_pixel_h, const unsigned int each_size_y=GraphicsSize::current.pad_pixel_h);


Color_t get_color_in_ring(const unsigned int index);


/**
 * Calibrated to allow a label on the top border.
 */
double increase_top_margin(const double scale=1);


double increase_right_margin(const double scale=1);


template<class GraphType>
double increase_left_margin(GraphType* graph_object, const double scale=1)
{
    const double current = gPad->GetLeftMargin();
    const double next = std::max(0., current + scale * GraphicsSize::current.margin_step_horizontal);

    gPad->SetLeftMargin(next);
    graph_object->GetYaxis()->SetTitleOffset(graph_object->GetYaxis()->GetTitleOffset() + scale * GraphicsSize::current.title_offset_step_horizontal);

    return next;
}


enum class LegendPosition
{
    TopLeft, TopRight, BottomRight, BottomLeft
};


TLegend* put_legend(LegendPosition leg_pos, Option_t* option="", const double width=0.3, const double height=0.2);


/**
 * Minimum right margin.
 */
template<class GraphType>
void set_x_axis(GraphType* graph_object)
{
    gPad->SetRightMargin(GraphicsSize::current.right_margin);
    gPad->SetBottomMargin(GraphicsSize::current.bottom_margin);

    TAxis* axis;

    axis = graph_object->GetXaxis();
    axis->SetTitleSize(GraphicsSize::current.text_size);
    axis->SetLabelSize(GraphicsSize::current.text_size);
    axis->SetTitleOffset(GraphicsSize::current.title_offset_x);
    axis->SetNdivisions(510);
    axis->SetDecimals(true);
    axis->CenterTitle();
}


template<class GraphType>
void set_y_axis(GraphType* graph_object)
{
    TAxis* axis;
    axis = graph_object->GetYaxis();

    /**
     * For 2-digits y-axis value
     */

    gPad->SetTopMargin(GraphicsSize::current.top_margin);
    gPad->SetLeftMargin(GraphicsSize::current.left_margin);

    axis->SetTitleSize(GraphicsSize::current.text_size);
    axis->SetLabelSize(GraphicsSize::current.text_size);
    axis->SetTitleOffset(GraphicsSize::current.title_offset_y);
    axis->SetNdivisions(505);
    axis->SetDecimals(true);
    axis->CenterTitle();

    const double min = graph_object->GetMinimum();
    const double max = graph_object->GetMaximum();

    const bool has_negative = ! (min > 0);

    if (has_negative) {
	increase_left_margin(graph_object);
    }

    const double diff_per_div = (max - min) / 5;

    const int y_digits = std::floor( std::log10( std::max( { std::abs(max) / diff_per_div, std::abs(min) / diff_per_div } ) ) ) + 2;
    
    increase_left_margin(graph_object, y_digits - 2);
}


template<class GraphType>
void set_axes(GraphType* graph_object)
{
    set_x_axis(graph_object);
    set_y_axis(graph_object);
}


TLine* draw_horizontal_line(const double y);


TLine* draw_vertical_line(const double x);


namespace publish 
{




template<class GraphType>
void set_max_digit_x(GraphType* graph_object)
{
    gPad->SetRightMargin(0.11);
    TAxis* axis;
    axis = graph_object->GetXaxis();
    axis->SetMaxDigits(3);
}


template<class GraphType>
void set_max_digit_y(GraphType* graph_object)
{
    gPad->SetTopMargin(0.075);
    TAxis* axis;
    axis = graph_object->GetYaxis();
    axis->SetMaxDigits(3);
}


template<class GraphType>
void set_time_x_axis(GraphType* graph_object)
{
    gPad->SetRightMargin(0.05);
    gPad->SetBottomMargin(0.19);

    TAxis* axis;

    axis = graph_object->GetXaxis();

    axis->SetTitle("Datetime");
    axis->SetTimeFormat("#splitline{%H:%M}{%m/%d}");
    axis->SetTimeDisplay(1);
    axis->SetLabelSize(0.06);
    axis->SetLabelOffset(0.03);
    axis->SetTitleSize(0.06);
    axis->SetTitleOffset(1.6);
    axis->SetNdivisions(503);
    axis->CenterTitle(true);
}


// template<class GraphType>
// void set_x_axis_medium(GraphType* graph_object)
// {
//     set_x_axis(graph_object);
//     gPad->SetRightMargin(right_margin_medium);
// }


// template<class GraphType>
// void set_y_axis_medium(GraphType* graph_object)
// {
//     gPad->SetTopMargin(top_margin);
//     gPad->SetLeftMargin(left_margin_medium);
// 
//     TAxis* axis;
// 
//     axis = graph_object->GetYaxis();
//     axis->SetTitleSize(title_size);
//     axis->SetLabelSize(label_size);
//     axis->SetTitleOffset(title_offset_y_medium);
//     axis->SetNdivisions(505);
//     axis->SetDecimals(true);
//     axis->CenterTitle();
// }


// template<class GraphType>
// void set_square_frame(GraphType* graph_object)
// {
//     set_axes(graph_object);
// 
//     const double top_right_margin = right_margin;
//     gPad->SetTopMargin(top_right_margin);
//     gPad->SetRightMargin(top_right_margin);
// 
//     const double bottom_left_margin = bottom_margin;
//     gPad->SetBottomMargin(bottom_left_margin);
//     gPad->SetLeftMargin(bottom_left_margin);
// 
//     gPad->Update();
// }


template<class GraphType>
void set_y_axis_full_width(GraphType* graph_object)
{
    gPad->SetLeftMargin(0.185);
    TAxis* axis;
    axis = graph_object->GetYaxis();
    axis->SetTitleOffset(1.4);
}


// template<class GraphType>
// void make_y_margin_smaller(GraphType* graph_object, const double ratio=0.7, const double margin_ratio=1.1)
// {
//     gPad->SetLeftMargin(left_margin * ratio * margin_ratio);
// 
//     TAxis* axis;
// 
//     axis  = graph_object->GetXaxis();
// 
//     axis = graph_object->GetYaxis();
//     axis->SetTitleOffset(axis->GetTitleOffset() * ratio);
// }


TMultiGraph* set_graph_colors_by_ring(TMultiGraph* mg);


TMultiGraph* set_graph_marker_styles_by_ring(TMultiGraph* mg);


} // namespace publish


TMultiGraph* set_multigraph_axis_from_member(TMultiGraph* mg);


double find_x(const TGraph* g, const double y, double x_start=0, double x_end=0);


template<class ObjectType>
std::vector<TCanvas*> draw_with_auto_recreator_of_canvas(const char* canvas_name_title, const size_t n_pad_x, const size_t n_pad_y, const std::vector<ObjectType*>& object_list, const char* draw_option)
{
    std::vector<TCanvas*> c_list;

    const size_t n_pad = n_pad_x * n_pad_y;
    size_t current_pad = n_pad + 1;
    for (size_t i = 0; i < object_list.size(); ++i) {
	TCanvas* c;
	if (current_pad > n_pad) {
	    c_list.push_back(new TCanvas(Form("%s_%zu", canvas_name_title, c_list.size()), Form("%s_%zu", canvas_name_title, c_list.size()), 700 * n_pad_x, 500 * n_pad_y));
	    c = c_list.back();
	    c->Divide(n_pad_x, n_pad_y);
	    current_pad = 1;
	} else {
	    c = c_list.back();
	}
	c->cd(current_pad);

	object_list[i]->Draw(draw_option);

	++current_pad;
    }

    return c_list;
}


template<class T, class ObjectType>
std::vector<TCanvas*> draw_with_auto_recreator_of_canvas(const char* canvas_name_title, const size_t n_pad_x, const size_t n_pad_y, const std::map<T, ObjectType*>& object_map, const char* draw_option)
{
    std::vector<ObjectType*> object_list;
    for (const auto& pair : object_map) {
	ObjectType* obj = pair.second;
	object_list.push_back(obj);
    }

    return draw_with_auto_recreator_of_canvas(canvas_name_title, n_pad_x, n_pad_y, object_list, draw_option);
}


template<class ConverterType>
TGraph* convert_graph_yaxis(TGraph** g, ConverterType conversion_expr, const std::string& y_title)
{
    for (size_t i = 0; i < (*g)->GetN(); ++i) {
	(*g)->GetY()[i] = conversion_expr((*g)->GetY()[i]);
    }
    (*g)->GetYaxis()->SetTitle(y_title.c_str());
    (*g)->SetBit(TGraph::kResetHisto);
    return *g;
}


} // namespace ROOT_helper


#endif
