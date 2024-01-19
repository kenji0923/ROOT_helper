#include "ROOT_helper.h"

#include <algorithm>
#include <filesystem>

#include "TF1.h"
#include "TSpline.h"
#include "TStyle.h"

namespace root_helper {

DataSaver::DataSaver(const std::filesystem::path& base_dir)
: base_dir_(base_dir)
{
}

void DataSaver::save_data_object(TObject* obj) const
{
  if (obj->InheritsFrom(TClass::GetClass<TPad>())) {
    TList* l_prim = static_cast<TPad*>(obj)->GetListOfPrimitives();
    for (auto* child : *l_prim) {
      save_data_object(child);
    }
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

void prepare()
{
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptTitle(0);
}

TCanvas* create_canvas(const std::string& name, const std::string& title, const unsigned int n_pad_x, const unsigned int n_pad_y, const unsigned int each_size_x, const unsigned int each_size_y)
{
  TCanvas* c = new TCanvas(name.c_str(), title.c_str(), n_pad_x * each_size_x, n_pad_y * each_size_y);
  if (n_pad_x > 1 || n_pad_y > 1) {
    c->Divide(n_pad_x, n_pad_y);
  }
  return c;
}

} // namespace publish

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

TLine* draw_horizontal_line(const double y)
{
  gPad->Update();
  TLine* l = new TLine(gPad->GetUxmin(), y, gPad->GetUxmax(), y);
  l->Draw("SAME");
  return l;
}

}; // namespace root_helper
