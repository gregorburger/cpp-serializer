#include <iostream>
#include <vector>
#include <memory>
#include <type_traits>
#include <cxxabi.h>

using namespace std;

struct ientry {
    virtual void print(void *) const = 0;
};

template <typename KLASS, typename T>
struct entry : public ientry {
    entry(T KLASS::*member) : member(member) {}
    T KLASS::* member;

    void print(void *v) const override {
        KLASS *k = (KLASS*) v;
        int status;
        char *name = abi::__cxa_demangle(typeid(KLASS).name(), 0, 0, &status);
        std::cout << name << ": " << k->*member << std::endl;
    }
};

struct texture;

template <typename KLASS, typename T>
struct sub_entry : public ientry {
    sub_entry(T KLASS::*member) : member(member) {}
    T KLASS::* member;

    void print(void *v) const override {
        KLASS *k = (KLASS*) v;
        T *p = &(k->*member);
        p->print();//p);
    }
};

struct serializable {
    virtual ~serializable() {}

    template <typename KLASS, typename T>
    typename std::enable_if<std::is_base_of<serializable, T>::value>::type
    add_entry(T KLASS::* member) {
        //7std::cout << "base" << std::endl;
        auto e = new sub_entry<KLASS, T>(member);
        entries.push_back(std::shared_ptr<ientry>(e));
    }

    template <typename KLASS, typename T>
    typename std::enable_if<!std::is_base_of<serializable, T>::value>::type
    add_entry(T KLASS::* member) {
        //std::cout << "not base" << std::endl;
        auto e = new entry<KLASS, T>(member);
        entries.push_back(std::shared_ptr<ientry>(e));
    }

    void print() {
        for (auto entry: entries) {
            entry->print(this);
            //std::cout << "entry" << std::endl;
        }
    }

    std::vector<std::shared_ptr<const ientry>> entries;
};

struct picture : public serializable {

    picture() : text("original smart") {
        add_entry(&picture::text);
    }
    std::string text;
};

struct texture : public serializable {

    texture() : width(640), height(480) {
        add_entry(&texture::width);
        add_entry(&texture::height);
    }
    int width, height;
};

struct container : public serializable {

    container() : float_member(3.142f), int_member(42) {
        add_entry(&container::float_member);
        add_entry(&container::int_member);
        add_entry(&container::tex);
        add_entry(&container::p);
    }

    texture tex;
    float float_member;
    int int_member;
    picture p;
};

struct derived_container : public container {

    derived_container() : member(10) {
        add_entry(&derived_container::member);
    }
    int member;
};


int main() {
    derived_container c;
    c.print();
    return 0;
}