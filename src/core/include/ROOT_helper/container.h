#ifndef ROOT_HELPER_CONTAINER_H
#define ROOT_HELPER_CONTAINER_H


#include <string>
#include <vector>

#include <TAxis.h>
#include <TDirectory.h>
#include <TGraph.h>
#include <THStack.h>
#include <TMultiGraph.h>

#ifndef ROOT_HELPER_USED_IN_INTERPRETER
#include <ROOT_helper/graphics.h>
#endif


namespace ROOT_helper
{


std::vector<std::string> get_object_path_from_directories(const std::string& object_name, const std::vector<std::string>& directory_list);


class ObjectList
{
public:
    ObjectList(const std::string& list_name);
    ~ObjectList();

    template<class ObjectType>
    int load_data(TDirectory* directory, const std::vector<std::string>& path_list, const std::vector<std::string>& title_list = {});

    int get_list_size() const { return object_list_.size(); }
    const std::vector<TObject*>& get_object_list() const { return object_list_; }

    template<class ObjectType>
    std::vector<ObjectType*> get_converted_object_list() const;

    template<class ObjectType>
    ObjectType* get_object(const int i) const { return dynamic_cast<ObjectType*>(object_list_.at(i)); }

    std::string get_title(const int i) const { return title_list_.at(i); }

    std::string list_name_;

private:
    std::vector<TObject*> object_list_;
    std::vector<TDirectory*> directory_list_;
    std::vector<std::string> path_list_;
    std::vector<std::string> title_list_;
};


template<class ObjectType>
int ObjectList::load_data(TDirectory* directory, const std::vector<std::string>& _path_list, const std::vector<std::string>& _title_list)
{
    const int n_obj = _path_list.size();

    for (int i_obj = 0; i_obj < n_obj; ++i_obj) {
	ObjectType* obj_buffer = nullptr;
	directory->GetObject(_path_list[i_obj].c_str(), obj_buffer);

	if (!obj_buffer) {
	    fprintf(stderr, "%s was not found\n", _path_list[i_obj].c_str());
	    exit(1);
	}

	object_list_.emplace_back(obj_buffer);

	directory_list_.emplace_back(directory);

	path_list_.emplace_back(_path_list[i_obj]);

	if (i_obj < _title_list.size()) {
	    static_cast<TNamed*>(obj_buffer)->SetTitle(_title_list[i_obj].c_str());
	    title_list_.emplace_back(_title_list[i_obj]);
	} else {
	    title_list_.emplace_back(object_list_.back()->GetTitle());
	}
    }

    return n_obj;
}


template<class ObjectType>
std::vector<ObjectType*> ObjectList::get_converted_object_list() const
{
    std::vector<ObjectType*> converted_object_list;

    for (auto* obj : object_list_) {
	converted_object_list.emplace_back(static_cast<ObjectType*>(obj));
    }

    return converted_object_list;
}


struct IContainerWrapper
{
    virtual ~IContainerWrapper() = default;

    virtual void Add(TObject* obj, std::string option="") = 0;
    virtual void Draw(std::string option="") = 0;
    virtual TAxis* GetXaxis() = 0;
    virtual TAxis* GetYaxis() = 0;
    virtual double GetMinimum() = 0;
    virtual double GetMaximum() = 0;

    virtual std::vector<TObject*> get_object_list() const = 0;
};


namespace ContainerWrapperHelper
{


template<class ContainerType>
struct DefaultOptions
{ };


template<>
struct DefaultOptions<TMultiGraph>
{
    int get_list_size(TMultiGraph* container) { return container->GetListOfGraphs()->GetSize(); }
    TList* get_list(TMultiGraph* container) { return container->GetListOfGraphs(); }
    TGraph* get_type_specified_obj(TObject* obj) { return static_cast<TGraph*>(obj); }

    std::string add = "PL";
    std::string draw = "A";
};


template<>
struct DefaultOptions<THStack>
{
    int get_list_size(THStack* container) const { return container->GetHists()->GetSize(); }
    TList* get_list(THStack* container) const { return container->GetHists(); }
    TH1* get_type_specified_obj(TObject* obj) const { return static_cast<TH1*>(obj); }

    std::string add= "HIST";
    std::string draw = "NOSTACK";
};


template<class ContainerType>
void set_default_add_option_if_null(std::string& option)
{
    if (option == "") {
	DefaultOptions<ContainerType> defaults;
	option = defaults.add;
    }
}


template<class ContainerType>
void set_default_draw_option_if_null(std::string& option)
{
    if (option == "") {
	DefaultOptions<ContainerType> defaults;
	option = defaults.draw;
    }
}


} // namespace ContainerWrapperHelper


template<class ContainerType>
struct ContainerWrapper : IContainerWrapper
{
public:
    ContainerWrapper(const std::string& nametitle);
    ~ContainerWrapper();

    void Add(TObject* obj, std::string option) override;
    void Draw(std::string option) override;
    TAxis* GetXaxis() override;
    TAxis* GetYaxis() override;
    double GetMinimum() override;
    double GetMaximum() override;

    std::vector<TObject*> get_object_list() const override;

    ContainerType* container_;
private:
    ContainerWrapperHelper::DefaultOptions<ContainerType> defaults_;
};


template<class ContainerType>
ContainerWrapper<ContainerType>::ContainerWrapper(const std::string& nametitle)
{
    container_ = new ContainerType(nametitle.c_str(), nametitle.c_str());
}


template<class ContainerType>
ContainerWrapper<ContainerType>::~ContainerWrapper()
{
    delete container_;
}


template<class ContainerType>
void ContainerWrapper<ContainerType>::Add(TObject* obj, std::string option)
{
    ContainerWrapperHelper::set_default_add_option_if_null<ContainerType>(option);

    auto* specified_obj = defaults_.get_type_specified_obj(obj);

    container_->Add(specified_obj, option.c_str());
    container_->SetTitle(Form("%s;%s;%s", container_->GetName(), specified_obj->GetXaxis()->GetTitle(), specified_obj->GetYaxis()->GetTitle()));

    specified_obj->SetMarkerColor(get_color_in_ring(defaults_.get_list_size(container_) - 1));
    specified_obj->SetLineColor(get_color_in_ring(defaults_.get_list_size(container_) - 1));
}


template<class ContainerType>
void ContainerWrapper<ContainerType>::Draw(std::string option)
{
    ContainerWrapperHelper::set_default_draw_option_if_null<ContainerType>(option);

    container_->Draw(option.c_str());
}


template<class ContainerType>
TAxis* ContainerWrapper<ContainerType>::GetXaxis()
{
    return container_->GetXaxis();
}


template<class ContainerType>
TAxis* ContainerWrapper<ContainerType>::GetYaxis()
{
    return container_->GetYaxis();
}


template<class ContainerType>
double ContainerWrapper<ContainerType>::GetMinimum()
{
    return container_->GetHistogram()->GetMinimum();
}


template<class ContainerType>
double ContainerWrapper<ContainerType>::GetMaximum()
{
    return container_->GetHistogram()->GetMaximum();
}


template<class ContainerType>
std::vector<TObject*> ContainerWrapper<ContainerType>::get_object_list() const
{
    ContainerWrapperHelper::DefaultOptions<ContainerType> defaults;

    std::vector<TObject*> object_list;

    TList* list = defaults.get_list(container_);

    for (auto* obj : *list) {
	object_list.emplace_back(obj);
    }

    return object_list;
}


enum class MultiObjectType
{
    Graph, Histo
};


class MultiObject
{
public:
    MultiObject(MultiObjectType object_type, const std::string& nametitle, TDirectory* directory, const std::vector<std::string>& object_name);
    MultiObject(MultiObjectType object_type, const std::string& nametitle, const std::vector<TObject*> obj_list);
    ~MultiObject();

    void Draw(std::string option="");

    template<class ContainerType>
    ContainerType* get_container() const;

    template<class ObjectType>
    std::vector<ObjectType*> get_object_list() const;

private:
    void initialize_container(const std::string& nametitle);

    MultiObjectType object_type_;
    IContainerWrapper* container_;
    std::vector<TObject*> object_;
};


template<class ContainerType>
ContainerType* MultiObject::get_container() const
{
    return dynamic_cast<ContainerWrapper<ContainerType>*>(container_)->container_;
}


template<class ObjectType>
std::vector<ObjectType*> MultiObject::get_object_list() const
{
    std::vector<ObjectType*> object_list;

    std::vector<TObject*> abs_obj_list = container_->get_object_list();

    for (auto* obj : abs_obj_list) {
	object_list.emplace_back(dynamic_cast<ObjectType*>(obj));
    }

    return object_list;
}


} // namespace ROOT_helper


#endif
