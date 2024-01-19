#include "ROOT_helper.cc"
#include "ROOT_helper.h"

void publish()
{
  root_helper::publish::prepare();

  const unsigned int n_pad_x = 2;
  const unsigned int n_pad_y = 1;
  TCanvas* c_test = root_helper::publish::create_canvas("c_test", "Test canvas", n_pad_x, n_pad_y);

  const double range_x_start = 0;
  const double range_x_end = 1000;
  TF1* f_test = new TF1("f_test", "[0] * TMath::Sin(TMath::TwoPi() * [1] * x) +[2]", range_x_start, range_x_end);
  f_test->SetParameters(0.1, 2.0 / range_x_end, 0);
  f_test->GetXaxis()->SetTitle("x-quantity");
  f_test->GetYaxis()->SetTitle("y-VALUE");

  c_test->cd(1);
  f_test->Draw("L");
  root_helper::publish::set_axes(f_test);

  TF1* f_test_2 = new TF1("f_test_2", "[0] * TMath::Gaus(x, [1], [2]) +[3]", 0, 1000);
  f_test_2->SetParameters(1000, 500, 100);
  f_test_2->GetXaxis()->SetTitle("x-quantity");
  f_test_2->GetYaxis()->SetTitle("y-VALUE");

  c_test->cd(2);
  f_test_2->Draw("L");
  root_helper::publish::set_axes(f_test_2);
}

int test_ROOT_helper()
{
  publish();

  return 0;
}
