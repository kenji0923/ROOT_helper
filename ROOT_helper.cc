#include "ROOT_helper.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <regex>

#include <RtypesCore.h>
#include <TF1.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TSpline.h>
#include <TStyle.h>

namespace ROOT_helper {

DataSaver::DataSaver(const std::filesystem::path& base_directory)
: base_directory_(base_directory)
{
  std::filesystem::create_directories(base_directory_);
  f_write_ = std::make_unique<TFile>((base_directory / "data.root").c_str(), "RECREATE");
}

DataSaver::~DataSaver()
{
  f_write_->Save();
}

void DataSaver::write_canvas(TCanvas* c, const std::filesystem::path& relative_save_directory) const
{
  const std::filesystem::path write_directory = base_directory_ / relative_save_directory;

  save_object(c, relative_save_directory);

  c->Print(Form("%s.pdf", (write_directory / c->GetName()).c_str()));

  std::filesystem::path png_save_directory = write_directory / "png";
  std::filesystem::create_directories(png_save_directory);
  c->Print(Form("%s.png", (png_save_directory / c->GetName()).c_str()));
}

std::filesystem::path DataSaver::create_directories(const std::filesystem::path& relative_path) const
{
  const std::filesystem::path created_path = base_directory_ / relative_path;
  std::filesystem::create_directories(created_path);
  return created_path;
}

void DataSaver::create_and_change_directory(const std::filesystem::path& relative_save_directory) const
{
  std::filesystem::create_directories(base_directory_ / relative_save_directory);

  const std::string relative_ROOT_directory = relative_save_directory.string();
  TDirectory* root_directory = f_write_->GetDirectory(relative_ROOT_directory.c_str());

  if (!root_directory) {
    auto it_path = relative_save_directory.end();
    --it_path;
    root_directory = f_write_->mkdir(relative_save_directory.c_str(), (*it_path).c_str(), kFALSE);
  }

  root_directory->cd();
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

TLegend* put_legend(unsigned int position, Option_t* option, const double width, const double height)
{
  const Double_t top_edge = 1.0 - gPad->GetTopMargin() - 0.02;
  const Double_t right_edge = 1.0 - gPad->GetRightMargin() - 0.02;
  const Double_t bottom_edge = gPad->GetBottomMargin() + 0.02;
  const Double_t left_edge = gPad->GetLeftMargin() + 0.02;

  TLegend* leg;
  if (position == 0) { // Top-left
    leg = gPad->BuildLegend(left_edge, top_edge, left_edge + width, top_edge - height,  "", option);
  } else if (position == 1) { // Top-right
    leg = gPad->BuildLegend(right_edge - width, top_edge, right_edge, top_edge - height, "", option);
  } else if (position == 2) { // Bottom-right
    leg = gPad->BuildLegend(right_edge - width, bottom_edge + height, right_edge, bottom_edge, "", option);
  } else if (position == 3) {
    leg = gPad->BuildLegend(width, height, width, height,  "", option);
  } else {
    leg = gPad->BuildLegend(width, height, width, height,  "", option);
  }

  leg->SetBorderSize(0);
  leg->SetTextSize(0.06);

  return leg;
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

}; // namespace ROOT_helper
