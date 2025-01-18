#ifndef ROOT_HELPER_USED_IN_INTERPRETER
#include <ROOT_helper/data_saver.h>
#endif


#include <filesystem>

#include <TCanvas.h>


namespace ROOT_helper
{


DataSaver::DataSaver(const std::filesystem::path& base_directory, const bool is_recreate)
: base_directory_(base_directory)
{
    std::filesystem::create_directories(base_directory_);

    std::string open_mode = "UPDATE";
    if (is_recreate) open_mode = "RECREATE";

    f_write_ = std::make_unique<TFile>((base_directory / "data.root").c_str(), open_mode.c_str());
}


DataSaver::~DataSaver()
{
    f_write_->Save();
}


void DataSaver::write_canvas(TCanvas* c, const std::filesystem::path& relative_save_directory) const
{
    write_canvas_without_data_saving(c, relative_save_directory);

    save_object(c, relative_save_directory);
}


void DataSaver::write_canvas_without_data_saving(TCanvas* c, const std::filesystem::path& relative_save_directory) const
{
    gPad->Update();

    const std::filesystem::path write_directory = base_directory_ / relative_save_directory;

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


} // namespace ROOT_helper
