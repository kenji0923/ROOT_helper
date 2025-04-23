#ifndef ROOT_HELPER_USED_IN_INTERPRETER
#include <ROOT_helper/container.h>
#include <ROOT_helper/graphics.h>
#endif


#include <string>

#include <TAxis.h>
#include <TGraph.h>
#include <THStack.h>
#include <TMultiGraph.h>
#include <TString.h>


namespace ROOT_helper
{


std::vector<std::string> get_object_path_from_directories(const std::string& object_name, const std::vector<std::string>& directory_list)
{
    std::vector<std::string> path_list;

    for (const auto& directory : directory_list) {
	path_list.emplace_back(directory + "/" + object_name);
    }

    return path_list;
}


ObjectList::ObjectList(const std::string& list_name)
: list_name_(list_name)
{
}


ObjectList::~ObjectList()
{
}


MultiObject::MultiObject(MultiObjectType object_type, const std::string& nametitle, TDirectory* directory, const std::vector<std::string>& object_name)
: object_type_(object_type)
{
    initialize_container(nametitle);

    const int n_obj = object_name.size();

    object_.resize(n_obj);

    for (int i_obj = 0; i_obj < n_obj; ++i_obj) {
	const std::string& name = object_name[i_obj];

	TObject* obj;
	directory->GetObject(name.c_str(), obj);

	object_[i_obj] = obj;

	container_->Add(obj);
    }
}


MultiObject::MultiObject(MultiObjectType object_type, const std::string& nametitle, const std::vector<TObject*> obj_list)
: object_type_(object_type)
{
    initialize_container(nametitle);

    const int n_obj = obj_list.size();

    object_.resize(n_obj);

    for (int i_obj = 0; i_obj < n_obj; ++i_obj) {
	TObject* obj;
	obj = obj_list[i_obj];

	object_[i_obj] = obj;

	container_->Add(obj);
    }
}


MultiObject::~MultiObject()
{
    delete container_;
}


void MultiObject::Draw(std::string option)
{
    container_->Draw(option);

    set_axes(container_);
}


void MultiObject::initialize_container(const std::string& nametitle)
{
    if (object_type_ == MultiObjectType::Graph) {
	container_ = new ContainerWrapper<TMultiGraph>(nametitle.c_str());
    } else if (object_type_ == MultiObjectType::Histo) {
	container_ = new ContainerWrapper<THStack>(nametitle.c_str());
    } else {
	fprintf(stderr, "unkown type was specified for object type in MultiObject\n");
	exit(1);
    }
}


} // namespace ROOT_helper
