#include "../ROOT_helper.h"

#include <string>

#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>


namespace rh = ROOT_helper;


TCanvas* create_test_canvas(std::string canvas_name, const unsigned int n_pad_x=1, const unsigned int n_pad_y=1);


rh::DataSaver data_saver("GraphicsTest");


int test_graphics()
{
    rh::prepare();

    /**
     * Single pad.
     * 8pt text size with 86mm width.
     * Increasing right margin of the pad for a label at the end of x-axis.
     */
    create_test_canvas("SinglePad_8pt_LabelAtXend");


    /**
     * Four pads.
     */
    create_test_canvas("FourPads_8pt", 2, 2);


    /**
     * Six pads.
     */
    create_test_canvas("SixPads_8pt", 3, 2);


    /**
     * Single pad.
     * 10pt text size with 86mm width.
     * Increasing right margin of the pad for a label at the end of x-axis.
     */
    rh::GraphicsSize::current = rh::g_size_10pt;
    create_test_canvas("SinglePad_10pt_LabelAtXend");

    return 0;
}


TCanvas* create_test_canvas(std::string canvas_name, const unsigned int n_pad_x, const unsigned int n_pad_y)
{
    canvas_name = std::string("c_") + canvas_name;
    TCanvas* c = rh::create_canvas(canvas_name.c_str(), canvas_name.c_str(), n_pad_x, n_pad_y);

    TF1* f_wave = new TF1("f_wave", [](const double* x, const double* p){ return p[0] * TMath::Sin(TMath::TwoPi() * p[1] * x[0]); }, 0, 1, 2);

    for (int i_pad = 0; i_pad < n_pad_x * n_pad_y; ++i_pad) {
	c->cd(1 + i_pad);

	f_wave->SetLineColor(rh::get_color_in_ring(i_pad));
	f_wave->SetParameters(1, 4);
	TF1* f_wave_copy = f_wave->DrawCopy("L");
	rh::set_axes(f_wave_copy);
	rh::increase_right_margin(1);
	f_wave_copy->GetXaxis()->SetTitle("xyzABC (arb. units)");
	f_wave_copy->GetYaxis()->SetTitle("yzxBCA (arb. units)");
    }

    data_saver.write_canvas(c);

    return c;
}
