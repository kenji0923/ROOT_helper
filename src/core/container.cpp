#ifndef ROOT_HELPER_USED_IN_INTERPRETER
#include <ROOT_helper/container.h>
#include <ROOT_helper/graphics.h>
#endif


#include <string>

#include <TAxis.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TString.h>


namespace ROOT_helper
{


template<>
ContainerWrapper<TMultiGraph>::ContainerWrapper(const std::string& nametitle)
{
    container_ = new TMultiGraph(nametitle.c_str(), nametitle.c_str());
}


template<>
ContainerWrapper<TMultiGraph>::~ContainerWrapper()
{
    delete container_;
}


template<>
void ContainerWrapper<TMultiGraph>::Add(TObject* obj, std::string option)
{
    if (option == "") {
	option = "L";
    }

    TGraph* graph_obj = dynamic_cast<TGraph*>(obj);

    container_->Add(graph_obj, option.c_str());
    container_->SetTitle(Form("%s;%s;%s", container_->GetName(), graph_obj->GetXaxis()->GetTitle(), graph_obj->GetYaxis()->GetTitle()));

    graph_obj->SetLineColor(get_color_in_ring(container_->GetListOfGraphs()->GetSize() - 1));
}


template<>
void ContainerWrapper<TMultiGraph>::Draw(std::string option)
{
    if (option == "") {
	option = "A";
    }

    container_->Draw(option.c_str());
}


template<>
TAxis* ContainerWrapper<TMultiGraph>::GetXaxis()
{
    return container_->GetXaxis();
}


template<>
TAxis* ContainerWrapper<TMultiGraph>::GetYaxis()
{
    return container_->GetYaxis();
}


template<>
std::vector<TObject*> ContainerWrapper<TMultiGraph>::get_object_list() const
{
    std::vector<TObject*> object_list;

    TList* list = container_->GetListOfGraphs();

    for (auto* obj : *list) {
	object_list.emplace_back(obj);
    }

    return object_list;
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

    publish::set_axes(container_);
}


void MultiObject::initialize_container(const std::string& nametitle)
{
    if (object_type_ == MultiObjectType::Graph) {
	container_ = new ContainerWrapper<TMultiGraph>(nametitle.c_str());
    } else {
	fprintf(stderr, "unkown type was specified for object type in MultiObject\n");
	exit(1);
    }
}


} // namespace ROOT_helper
