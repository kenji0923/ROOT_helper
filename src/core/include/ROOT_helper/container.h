#ifndef ROOT_HELPER_CONTAINER_H
#define ROOT_HELPER_CONTAINER_H


#include <TAxis.h>
#include <TDirectory.h>
#include <TMultiGraph.h>


namespace ROOT_helper
{


class IContainerWrapper
{
public:
    virtual ~IContainerWrapper() = default;

    virtual void Add(TObject* obj, std::string option="") = 0;
    virtual void Draw(std::string option="") = 0;
    virtual TAxis* GetXaxis() = 0;
    virtual TAxis* GetYaxis() = 0;

    virtual std::vector<TObject*> get_object_list() const = 0;
};


template<class ContainerType>
class ContainerWrapper : public IContainerWrapper
{
public:
    ContainerWrapper(const std::string& nametitle);
    ~ContainerWrapper();

    void Add(TObject* obj, std::string option) override;
    void Draw(std::string option) override;
    TAxis* GetXaxis() override;
    TAxis* GetYaxis() override;

    std::vector<TObject*> get_object_list() const override;

    ContainerType* container_;
};


enum class MultiObjectType
{
    Graph,
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
