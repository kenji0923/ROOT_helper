#ifndef ROOT_HELPER_HH
#define ROOT_HELPER_HH

#include <filesystem>
#include <iostream>
#include <memory>
#include <utility>

#include <RtypesCore.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TClass.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TH1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TPad.h>

namespace ROOT_helper
{

class DataSaver
{
public:
  DataSaver(const std::filesystem::path& base_directory);
  ~DataSaver();

  template<class ObjectType>
  void save_object(ObjectType* c, const std::filesystem::path& relative_save_directory) const;

  void write_canvas(TCanvas* c, const std::filesystem::path& relative_save_directory="") const;

  std::filesystem::path create_directories(const std::filesystem::path& relative_path) const;

private:
  void create_and_change_directory(const std::filesystem::path& relative_save_directory) const;

  const std::filesystem::path base_directory_;
  std::unique_ptr<TFile> f_write_;
  std::vector<TClass*> class_to_save_list_ = std::vector<TClass*> {
    TClass::GetClass<TH1>(), TClass::GetClass<TGraph>(), TClass::GetClass<TGraph2D>(), TClass::GetClass<TMultiGraph>()
  };
};

template<class ObjectType>
void DataSaver::save_object(ObjectType* obj, const std::filesystem::path& relative_save_directory) const
{
  create_and_change_directory(relative_save_directory);

  auto save_child = [relative_save_directory, this](TList* list)
  {
      if (list) {
	  for (auto* child : *list) {
	      save_object(child, relative_save_directory);
	  }
      }
  };

  if (obj->InheritsFrom(TClass::GetClass<TPad>())) {
    if (obj->InheritsFrom(TClass::GetClass<TCanvas>())) obj->Write();
    save_child(dynamic_cast<TPad*>(obj)->GetListOfPrimitives());
  } else if (obj->InheritsFrom(TClass::GetClass<TMultiGraph>())) {
    obj->Write();
    save_child(dynamic_cast<TMultiGraph*>(obj)->GetListOfGraphs());
  } else if (obj->InheritsFrom(TClass::GetClass<THStack>())) {
    obj->Write();
    save_child(dynamic_cast<THStack*>(obj)->GetHists());
  } else {
    for (const auto* class_type : class_to_save_list_) {
      if (obj->InheritsFrom(class_type)) {
        obj->Write();
        break;
      }
    }
  }
}

namespace publish
{

static const double top_margin = 0.04;
static const double right_margin = 0.02;
static const double bottom_margin = 0.17;
static const double left_margin = 0.14;

static const double title_size = 0.07;
static const double label_size = 0.07;
static const double title_offset_x = 1.1;
static const double title_offset_y = 1.0;
static const double label_offset_x = 0.015;

static const double top_margin_medium = 0.075;
static const double right_margin_medium = 0.07;
static const double left_margin_medium = 0.16;
static const double title_offset_y_medium = 1.15;

void prepare();

std::pair<unsigned int, unsigned int> get_default_n_pad(const unsigned int n_plot);

TCanvas* create_canvas(const std::string& name, const std::string& title, const unsigned int n_pad_x=1, const unsigned int n_pad_y=1, const unsigned int each_size_x=700, const unsigned int each_size_y=500);

TCanvas* create_canvas_with_default_pad_matrix(const std::string& name, const std::string& title, const unsigned int n_pad=1, const unsigned int each_size_x=700, const unsigned int each_size_y=500);

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


template<class GraphType>
void set_x_axis(GraphType* graph_object)
{
    gPad->SetRightMargin(right_margin);
    gPad->SetBottomMargin(bottom_margin);

    TAxis* axis;

    axis  = graph_object->GetXaxis();
    axis->SetTitleSize(title_size);
    axis->SetLabelSize(label_size);
    axis->SetTitleOffset(title_offset_x);
    axis->SetNdivisions(510);
    axis->SetDecimals(true);
    axis->CenterTitle();
}


template<class GraphType>
void set_x_axis_medium(GraphType* graph_object)
{
    set_x_axis(graph_object);
    gPad->SetRightMargin(right_margin_medium);
}


/**
 * Smallest margin.
 * Suitable for the axis with the positive range
 */
template<class GraphType>
void set_y_axis(GraphType* graph_object)
{
  gPad->SetTopMargin(top_margin);
  gPad->SetLeftMargin(left_margin);

  TAxis* axis;

  axis = graph_object->GetYaxis();
  axis->SetTitleSize(title_size);
  axis->SetLabelSize(label_size);
  axis->SetTitleOffset(title_offset_y);
  axis->SetNdivisions(505);
  axis->SetDecimals(true);
  axis->CenterTitle();
}

template<class GraphType>
void set_y_axis_medium(GraphType* graph_object)
{
  gPad->SetTopMargin(top_margin);
  gPad->SetLeftMargin(left_margin_medium);

  TAxis* axis;

  axis = graph_object->GetYaxis();
  axis->SetTitleSize(title_size);
  axis->SetLabelSize(label_size);
  axis->SetTitleOffset(title_offset_y_medium);
  axis->SetNdivisions(505);
  axis->SetDecimals(true);
  axis->CenterTitle();
}

template<class GraphType>
void set_axes(GraphType* graph_object)
{
  set_x_axis(graph_object);
  set_y_axis(graph_object);
}

template<class GraphType>
void set_square_frame(GraphType* graph_object)
{
    set_axes(graph_object);

    const double top_right_margin = right_margin;
    gPad->SetTopMargin(top_right_margin);
    gPad->SetRightMargin(top_right_margin);

    const double bottom_left_margin = bottom_margin;
    gPad->SetBottomMargin(bottom_left_margin);
    gPad->SetLeftMargin(bottom_left_margin);

    gPad->Update();
}

template<class GraphType>
void set_y_axis_full_width(GraphType* graph_object)
{
  gPad->SetLeftMargin(0.185);
  TAxis* axis;
  axis = graph_object->GetYaxis();
  axis->SetTitleOffset(1.4);
}

template<class GraphType>
void make_y_margin_smaller(GraphType* graph_object, const double ratio=0.7, const double margin_ratio=1.1)
{
  gPad->SetLeftMargin(left_margin * ratio * margin_ratio);

  TAxis* axis;

  axis  = graph_object->GetXaxis();

  axis = graph_object->GetYaxis();
  axis->SetTitleOffset(axis->GetTitleOffset() * ratio);
}

TLegend* put_legend(unsigned int position, Option_t* option="", const double width=0.3, const double height=0.2);

TMultiGraph* set_graph_colors_by_ring(TMultiGraph* mg);

TMultiGraph* set_graph_marker_styles_by_ring(TMultiGraph* mg);

} // namespace publish

TMultiGraph* set_multigraph_axis_from_member(TMultiGraph* mg);

TLine* draw_horizontal_line(const double y);

TLine* draw_vertical_line(const double x);

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

}; // namespace ROOT_helper

#endif
