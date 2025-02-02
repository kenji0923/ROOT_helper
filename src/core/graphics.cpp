#ifndef ROOT_HELPER_USED_IN_INTERPRETER
#include <ROOT_helper/graphics.h>
#endif


#include <regex>
#include <utility>

#include <TCanvas.h>
#include <TF1.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TLine.h>
#include <TSpline.h>
#include <TStyle.h>


namespace ROOT_helper
{


GraphicsSize GraphicsSize::current = g_size_8pt;


void prepare()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetOptTitle(0);
}


std::pair<unsigned int, unsigned int> get_default_n_pad(const unsigned int n_plot)
{
    if (n_plot <= 1) {
	return { 1, 1 };
    } else if (n_plot <= 2) {
	return { 2, 1 };
    } else if (n_plot <= 4) {
	return { 2, 2 };
    } else if (n_plot <= 6) {
	return { 3, 2 };
    } else if (n_plot <= 9) {
	return { 3, 3 };
    } else {
	return { 4, 3 };
    }
}


TCanvas* create_canvas(const std::string& name, const std::string& title, const unsigned int n_pad_x, const unsigned int n_pad_y, const unsigned int each_size_x, const unsigned int each_size_y)
{
    const double w = n_pad_x * each_size_x;
    const double h = n_pad_y * each_size_y;

    TCanvas* c = new TCanvas(name.c_str(), title.c_str(), w, h);

    if (n_pad_x > 1 || n_pad_y > 1) {
	c->Divide(n_pad_x, n_pad_y);
    }

    return c;
}


TCanvas* create_canvas_with_default_pad_matrix(const std::string& name, const std::string& title, const unsigned int n_pad, const unsigned int each_size_x, const unsigned int each_size_y)
{
    auto [ n_pad_x, n_pad_y ] = get_default_n_pad(n_pad);
    return create_canvas(name, title, n_pad_x, n_pad_y, each_size_x, each_size_y);
}


Color_t get_color_in_ring(const unsigned int index)
{
    switch (index)
    {
	case 0:  return kAzure + 2;
	case 1:  return kOrange + 10;
	case 2:  return kTeal + 4;
	case 3:  return kMagenta + 0;
	case 4:  return kCyan + 1;
	default: return kBlack;
    }
}


double increase_right_margin(const double scale)
{
    std::cout << "- " << GraphicsSize::current.left_margin << std::endl;
    std::cout << "-- " << GraphicsSize::current.margin_step_horizontal << std::endl;

    const double current = gPad->GetRightMargin();
    const double next = std::max(0., current + scale * GraphicsSize::current.margin_step_horizontal);

    gPad->SetRightMargin(next);

    return next;
}


TLegend* put_legend(LegendPosition leg_pos, Option_t* option, const double width, const double height)
{
    gPad->Update();

    const Double_t top_edge = 1.0 - gPad->GetTopMargin() - 0.02;
    const Double_t right_edge = 1.0 - gPad->GetRightMargin() - 0.02;
    const Double_t bottom_edge = gPad->GetBottomMargin() + 0.02;
    const Double_t left_edge = gPad->GetLeftMargin() + 0.02;

    TLegend* leg;

    switch (leg_pos) {
	case LegendPosition::TopLeft:
	    leg = gPad->BuildLegend(left_edge, top_edge, left_edge + width, top_edge - height,  "", option);
	    break;
	case LegendPosition::TopRight:
	    leg = gPad->BuildLegend(right_edge - width, top_edge, right_edge, top_edge - height, "", option);
	    break;
	case LegendPosition::BottomRight:
	    leg = gPad->BuildLegend(right_edge - width, bottom_edge + height, right_edge, bottom_edge, "", option);
	    break;
	case LegendPosition::BottomLeft:
	    leg = gPad->BuildLegend(width, height, width, height,  "", option);
	    break;
	default:
	    exit(1);
    }

    leg->SetBorderSize(0);
    leg->SetTextSize(GraphicsSize::current.text_size);

    return leg;
}


namespace publish
{


void prepare()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetOptTitle(0);
}


std::pair<unsigned int, unsigned int> get_default_n_pad(const unsigned int n_plot)
{
    if (n_plot <= 1) {
	return { 1, 1 };
    } else if (n_plot <= 2) {
	return { 2, 1 };
    } else if (n_plot <= 4) {
	return { 2, 2 };
    } else if (n_plot <= 6) {
	return { 3, 2 };
    } else if (n_plot <= 9) {
	return { 3, 3 };
    } else {
	return { 4, 3 };
    }
}


TCanvas* create_canvas(const std::string& name, const std::string& title, const unsigned int n_pad_x, const unsigned int n_pad_y, const unsigned int each_size_x, const unsigned int each_size_y)
{
    TCanvas* c = new TCanvas(name.c_str(), title.c_str(), n_pad_x * each_size_x, n_pad_y * each_size_y);
    if (n_pad_x > 1 || n_pad_y > 1) {
	c->Divide(n_pad_x, n_pad_y);
    }
    return c;
}


TCanvas* create_canvas_with_default_pad_matrix(const std::string& name, const std::string& title, const unsigned int n_pad, const unsigned int each_size_x, const unsigned int each_size_y)
{
    auto [ n_pad_x, n_pad_y ] = get_default_n_pad(n_pad);
    return create_canvas(name, title, n_pad_x, n_pad_y, each_size_x, each_size_y);
}


template<class ObjType, class IteratableType>
void set_colors_by_ring(IteratableType* iteratable, Color_t base_color=1, Int_t inc_color=+1)
{
    Color_t color = base_color;

    for (auto* obj : *iteratable) {
	ObjType* obj_casted = dynamic_cast<ObjType*>(obj);
	if (!obj_casted) continue;

	obj_casted->SetLineColor(color);
	obj_casted->SetMarkerColor(color);

	color += inc_color;
    }
}


template<class ObjType, class IteratableType>
void set_marker_styles_by_ring(IteratableType* iteratable, Style_t base_style=20, Int_t inc_style=+1)
{
    Style_t style = base_style;

    for (auto* obj : *iteratable) {
	ObjType* obj_casted = dynamic_cast<ObjType*>(obj);
	if (!obj_casted) continue;

	obj_casted->SetMarkerStyle(style);

	style += inc_style;
    }
}


TMultiGraph* set_graph_colors_by_ring(TMultiGraph* mg)
{
    set_colors_by_ring<TGraph>(mg->GetListOfGraphs());
    return mg;
}


TMultiGraph* set_graph_marker_styles_by_ring(TMultiGraph* mg)
{
    set_marker_styles_by_ring<TGraph>(mg->GetListOfGraphs());
    return mg;
}


} // namespace publish


TMultiGraph* set_multigraph_axis_from_member(TMultiGraph* mg)
{
    TList* list = mg->GetListOfGraphs();
    for (auto* obj : *list) {
	TGraph* g = static_cast<TGraph*>(obj);

	std::string new_title;
	std::smatch m_title;
	const std::string orig_title = mg->GetTitle();
	if (std::regex_search(orig_title, m_title, std::regex("^(.+);$"))) {
	    new_title = m_title.str(1);
	} else {
	    new_title = orig_title;
	}
	const std::string new_title_x = g->GetXaxis()->GetTitle();
	const std::string new_title_y = g->GetYaxis()->GetTitle();

	mg->SetTitle(Form("%s;%s;%s", new_title.c_str(), new_title_x.c_str(), new_title_y.c_str()));

	break;
    }
    return mg;
}


TLine* draw_horizontal_line(const double y)
{
    gPad->Update();
    TLine* l = new TLine(gPad->GetUxmin(), y, gPad->GetUxmax(), y);
    l->Draw("SAME");
    return l;
}


TLine* draw_vertical_line(const double x)
{
    gPad->Update();
    TLine* l = new TLine(x, gPad->GetUymin(), x, gPad->GetUymax());
    l->Draw("SAME");
    return l;
}


double find_x(const TGraph*g, const double y, double x_start, double x_end)
{
    if (x_start >= x_end) {
	x_start = *std::min_element(g->GetX(), g->GetX() + g->GetN());
	x_end = *std::max_element(g->GetX(), g->GetX() + g->GetN());
    }
    TSpline* spl = new TSpline3("spl_g", g);
    TF1* f = new TF1("f_spl_g", [spl](const double* x, const double* p) { return spl->Eval(x[0]); }, x_start, x_end, 0);

    double x = 0;
    if (y > f->GetMaximum(x_start, x_end)) {
	x = f->GetMaximumX(x_start, x_end);
    } else if (y < f->GetMinimum(x_start, x_end)) {
	x = f->GetMinimumX(x_start, x_end);
    } else {
	x = f->GetX(y, x_start, x_end);
    }

    delete f;
    delete spl;

    return x;
}


} // namespace ROOT_helper
