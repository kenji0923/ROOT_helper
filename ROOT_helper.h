#ifndef ROOT_HELPER_HH
#define ROOT_HELPER_HH

#include <filesystem>
#include <iostream>

#include "TCanvas.h"
#include "TClass.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TH1.h"
#include "TLine.h"
#include "TObject.h"
#include "TPad.h"

namespace root_helper
{

class DataSaver
{
public:
  DataSaver(const std::filesystem::path& base_dir);

  template<class... PathTypes>
  std::unique_ptr<TFile> create_TFile(const std::string& filename, PathTypes&&... directory_names);

  void save_data_object(TObject* obj) const;

  template<class... PathTypes>
  void save_canvas(TCanvas* c, PathTypes&&... directory_names) const;

private:
  template<class... PathTypes>
  std::filesystem::path get_path(std::filesystem::path& mother_path, std::filesystem::path path_0="", PathTypes&&... directory_names) const;

  template<class... PathTypes>
  std::filesystem::path create_directory(PathTypes&&... directory_names) const;

  const std::filesystem::path base_dir_;
  std::vector<TClass*> class_to_save_list_ = {
    TClass::GetClass<TH1>(), TClass::GetClass<TGraph>(), TClass::GetClass<TGraph2D>()
  };
};

template<class... PathTypes>
std::unique_ptr<TFile> DataSaver::create_TFile(const std::string& filename, PathTypes&&... directory_names)
{
  std::filesystem::path save_dir = create_directory(std::forward<PathTypes>(directory_names)...);

  std::unique_ptr<TFile> f_created(new TFile((save_dir / filename).c_str(), "RECREATE"));

  return f_created;
}

template<class... PathTypes>
void DataSaver::save_canvas(TCanvas* c, PathTypes&&... directory_names) const
{
  std::filesystem::path save_dir = create_directory(std::forward<PathTypes>(directory_names)...);

  c->Print(Form("%s.pdf", (save_dir / c->GetName()).c_str()));

  std::filesystem::path png_save_dir = save_dir / "png";
  std::filesystem::create_directories(png_save_dir);
  c->Print(Form("%s.png", (png_save_dir / c->GetName()).c_str()));

  std::filesystem::path ROOT_save_dir = save_dir / "root";
  std::filesystem::create_directories(ROOT_save_dir);
  TFile* f_write = new TFile(Form("%s.root", (ROOT_save_dir / c->GetName()).c_str()), "RECREATE");
  c->Write();
  save_data_object(c);
  f_write->Close();
  delete f_write;
}

template<class... PathTypes>
std::filesystem::path DataSaver::get_path(std::filesystem::path& mother_path, std::filesystem::path directory_name_0, PathTypes&&... directory_names) const
{
  if (directory_name_0 == "") {
    return mother_path;
  } else {
    mother_path /= directory_name_0;
    return get_path(mother_path, std::forward<PathTypes>(directory_names)...);
  }
}

template<class... PathTypes>
std::filesystem::path DataSaver::create_directory(PathTypes&&... directory_names) const
{
  std::filesystem::path save_dir = base_dir_;
  save_dir = get_path(save_dir, std::forward<PathTypes>(directory_names)...);
  std::filesystem::create_directories(save_dir);
  return save_dir;
}

TLine* draw_horizontal_line(const double y);

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

}; // namespace root_helper

#endif
