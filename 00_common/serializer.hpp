#pragma once

#include "protocol.hpp"
#include <light_header>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <type_traits>
#include <sstream>

using Ptree = boost::property_tree::ptree;

// --------------------------------------------------------

// template<class T, class Enable=void>
// struct is_container_of_enum
// {
//     using type = T ;
//     constexpr static bool value = false;
// };

// template <class T>
// struct is_container_of_enum<std::vector<T>, std::enable_if_t<std::is_enum_v<T>>>
// {
//     using type = std::vector<T> ;
//     constexpr static bool value = true;
// };

template <typename T>
struct is_container_of_enum : std::false_type { };

template <typename T>
struct is_container_of_enum<std::vector<T>> : std::is_enum<T> { };

template <typename T>
struct is_container_of_enum<std::set<T>> : std::is_enum<T> { };


template<typename T>
struct is_vector : public std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

// template <typename T>
// struct is_string : std::false_type { };

// template <typename T>
// struct is_string<string> : std::true_type<T> { };

// --------------------------------------------------------

struct PtreeOArchive
{
    Ptree ptree;

    PtreeOArchive() {}

    struct is_saving
    {
        typedef boost::mpl::bool_<true> type;
        BOOST_STATIC_CONSTANT(bool, value=true);
    };

    struct is_loading
    {
        typedef boost::mpl::bool_<false> type;
        BOOST_STATIC_CONSTANT(bool, value=false);
    };

    template <typename T>
    PtreeOArchive& operator&(boost::serialization::nvp<T> const& wrap)
    {
        save_resolver(wrap.name(), wrap.value());
        return *this;
    }

    template<typename T, typename std::enable_if<!std::is_enum<T>::value && !is_container_of_enum<T>::value>::type* = nullptr>
    void save_resolver(char const* name, T none_enum_item)
    {
        save(name, none_enum_item);
    }

    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void save_resolver(char const* name, T enum_item)
    {
        save(name, (int) enum_item);
    }

    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void save_resolver(char const* name, vector<T> & enum_vector)
    {
        save_vec_enum(name, enum_vector);
    }

    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void save_resolver(char const* name, set<T> & enum_vector)
    {
        save_set_enum(name, enum_vector);
    }

    void save(char const* name, bool b)
    {
        ptree.put(name, b);
    }

    void save(char const* name, char c)
    {
        ptree.put(name, c);
    }

    void save(char const* name, int i)
    {
        ptree.put(name, i);
    }

    void save(char const* name, const string& s)
    {
        ptree.put(name, s);
    }

    void save(char const* name, const char * s)
    {
        ptree.put(name, s);
    }

    template <class T>
    void save(char const* name, const T& obj)
    {
        PtreeOArchive child_arch;
        T obj2 = obj; // bad: deep copy
        obj2.template serialize<PtreeOArchive>(child_arch, 0);
        ptree.add_child(name, child_arch.ptree);
    }

    template <typename T>
    void save_vec_basic(char const* name, const vector<T>& v)
    {
        PtreeOArchive child;
        for(const auto &x : v)
            child.add_basic_element_to_vector(x);
        ptree.add_child(name, child.ptree);
    }

    template <typename T>
    void save_set_basic(char const* name, const set<T>& v)
    {
        PtreeOArchive child;
        for(const auto &x : v)
            child.add_basic_element_to_set(x);
        ptree.add_child(name, child.ptree);
    }

    template <typename T>
    void save_vec_enum(char const* name, const vector<T> & enum_vector)
    {
        PtreeOArchive child;
        for(const auto &x : enum_vector)
            child.add_basic_element_to_vector((int) x);
        ptree.add_child(name, child.ptree);
    }

    template <typename T>
    void save_set_enum(char const* name, const set<T> & enum_set)
    {
        PtreeOArchive child;
        for(const auto &x : enum_set)
            child.add_basic_element_to_set((int) x);
        ptree.add_child(name, child.ptree);
    }

    // vector of vector of int
    template <typename T>
    void save_vec_vec_basic(char const* name, const vector<vector<T>>& v)
    {
        Ptree pt_child;
        for(const auto &v_inner : v)
        {
            PtreeOArchive arch_child_inner;
            for(const auto &x : v_inner)
                arch_child_inner.add_basic_element_to_vector(x);
            pt_child.push_back(std::make_pair("", arch_child_inner.ptree));
        }
        ptree.add_child(name, pt_child);
    }

    // set of objects
    template <class T, typename std::enable_if<!std::is_enum<T>::value && std::is_class<T>::value && !is_vector<T>::value && !std::is_same<string, std::decay_t<T>>::value>::type* = nullptr>
    void save(char const* name, const set<T>& v)
    {
        PtreeOArchive child;
        for(const auto &x : v)
            child.add_obj_element_to_set(x);
        ptree.add_child(name, child.ptree);
    }

    // vector of objects
    template <class T, typename std::enable_if<!std::is_enum<T>::value && std::is_class<T>::value && !is_vector<T>::value && !std::is_same<string, std::decay_t<T>>::value>::type* = nullptr>
    void save(char const* name, vector<T>& v)
    {
        PtreeOArchive child;
        for(const auto &x : v)
            child.add_obj_element_to_vector(x);
        ptree.add_child(name, child.ptree);
    }

    // set of enum types
    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void save(char const* name, set<T>& v)
    {
        PtreeOArchive child;
        for(const auto &x : v)
            child.add_basic_element_to_set((int) x);
        ptree.add_child(name, child.ptree);
    }

    // // set of other basic objects
    // template<typename T, typename std::enable_if<!std::is_enum<T>::value>::type* = nullptr>
    // void save(char const* name, const set<T>& v)
    // {
    //     PtreeOArchive child;
    //     for(const auto &x : v)
    //         child.add_obj_element_to_set(x);
    //     ptree.add_child(name, child.ptree);
    // }

    // vector of enum types
    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void save(char const* name, const vector<T>& v)
    {
        PtreeOArchive child;
        for(const auto &x : v)
            child.add_basic_element_to_vector((int) x);
        ptree.add_child(name, child.ptree);
    }

    // vector of other basic objects
    // template<typename T, typename std::enable_if<!std::is_enum<T>::value>::type* = nullptr>
    // void save(char const* name, const vector<T>& v)
    // {
    //     PtreeOArchive child;
    //     for(const auto &x : v)
    //         child.add_obj_element_to_vector(x);
    //     ptree.add_child(name, child.ptree);
    // }

    // template<typename T, typename std::enable_if<!std::is_enum<T>::value>::type* = nullptr>

    // vector of basisc elements

    void save(char const* name, const vector<int>& v)
    {
        save_vec_basic(name, v);
    }

    void save(char const* name, const vector<char *>& v)
    {
        save_vec_basic(name, v);
    }

    void save(char const* name, const vector<string>& v)
    {
        save_vec_basic(name, v);
    }

    void save(char const* name, const vector<char>& v)
    {
        save_vec_basic(name, v);
    }

    void save(char const* name, const vector<vector<int>>& v)
    {
        save_vec_vec_basic(name, v);
    }

    // set of basisc elements

    void save(char const* name, const set<int>& v)
    {
        save_set_basic(name, v);
    }

    void save(char const* name, const set<char *>& v)
    {
        save_set_basic(name, v);
    }

    void save(char const* name, const set<string>& v)
    {
        save_set_basic(name, v);
    }

    void save(char const* name, const set<char>& v)
    {
        save_set_basic(name, v);
    }

    template <typename T>
    void add_obj_element_to_vector(const T& x)
    {
        PtreeOArchive ptree_arch;
        T x2 = x; // bad: deep copy
        x2.template serialize<PtreeOArchive>(ptree_arch, 0);
        ptree.push_back(std::make_pair("", ptree_arch.ptree));
    }

    template <typename T>
    void add_obj_element_to_set(const T& x)
    {
        PtreeOArchive ptree_arch;
        T x2 = x; // bad: deep copy
        x2.template serialize<PtreeOArchive>(ptree_arch, 0);
        ptree.push_back(std::make_pair("", ptree_arch.ptree));
    }

    template <typename T>
    void add_basic_element_to_vector(const T& x)
    {
        Ptree pt_child;
        pt_child.put_value(x);
        ptree.push_back(std::make_pair("", pt_child));
    }

    template <typename T>
    void add_basic_element_to_set(const T& x)
    {
        Ptree pt_child;
        pt_child.put_value(x);
        ptree.push_back(std::make_pair("", pt_child));
    }

    string to_json_string(bool pretty=true)
    {
        stringstream buf;
        write_json(buf, ptree, pretty);
        string json = buf.str(); // {"foo":"bar"}
        return json;
    }
};


struct PtreeIArchive
{
    Ptree ptree;

    PtreeIArchive(Ptree ptree): ptree(ptree) {}
    PtreeIArchive(const string &txt) { this->from_json_string(txt); }


    struct is_saving
    {
        typedef boost::mpl::bool_<false> type;
        BOOST_STATIC_CONSTANT(bool, value=false);
    };

    struct is_loading
    {
        typedef boost::mpl::bool_<true> type;
        BOOST_STATIC_CONSTANT(bool, value=true);
    };

    template <typename T>
    PtreeIArchive& operator&(boost::serialization::nvp<T> const& wrap)
    {
        load_resolver(wrap.name(), wrap.value());
        return *this;
    }

    template<typename T, typename std::enable_if<!std::is_enum<T>::value && !is_container_of_enum<T>::value>::type* = nullptr>
    void load_resolver(char const* name, T &x)
    {
        load(name, x);
    }

    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void load_resolver(char const* name, T &x)
    {
        int num;
        load(name, num);
        x = (T) num;
    }

    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void load_resolver(char const* name, vector<T> &x)
    {
        load_vec_enum(name, x);
    }

    template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    void load_resolver(char const* name, set<T> &x)
    {
        load_set_enum(name, x);
    }

    template <class T>
    void load(char const* name, T &x)
    {
        PtreeIArchive child_arch(this->ptree.get_child(name));
        x.template serialize<PtreeIArchive>(child_arch, 0);
    }

    void load(char const* name, bool &x)
    {
        x = this->ptree.get<bool>(name);
    }
    
    void load(char const* name, int &x)
    {
        x = this->ptree.get<int>(name);
    }

    void load(char const* name, string& s)
    {
        s = this->ptree.get<string>(name);
    }

    void load(char const* name, vector<int> &x)
    {
        load_vec_basic<int>(name, x);
    }

    void load(char const* name, vector<char*> &x)
    {
        load_vec_basic<char*>(name, x);
    }

    void load(char const* name, vector<string> &x)
    {
        load_vec_basic<string>(name, x);
    }

    void load(char const* name, vector<char> &x)
    {
        load_vec_basic<char>(name, x);
    }

    void load(char const* name, set<int> &x)
    {
        load_set_basic<int>(name, x);
    }

    void load(char const* name, set<char*> &x)
    {
        load_set_basic<char*>(name, x);
    }

    void load(char const* name, set<string> &x)
    {
        load_set_basic<string>(name, x);
    }

    void load(char const* name, set<char> &x)
    {
        load_set_basic<char>(name, x);
    }

    template <class T, typename std::enable_if<!std::is_enum<T>::value && std::is_class<T>::value && !is_vector<T>::value && !std::is_same<string, std::decay_t<T>>::value>::type* = nullptr>
    void load(char const* name, vector<T> &x)
    {
        load_vec_obj(name, x);
    }

    template <class T, typename std::enable_if<!std::is_enum<T>::value && std::is_class<T>::value && !is_vector<T>::value && !std::is_same<string, std::decay_t<T>>::value>::type* = nullptr>
    void load(char const* name, set<T> &x)
    {
        load_set_obj(name, x);
    }

    // template <typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    // void load(char const* name, vector<T> &x)
    // {
    //     load_vec_enum(name, x);
    // }

    // template <typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
    // void load(char const* name, set<T> &x)
    // {
    //     load_set_enum(name, x);
    // }

    void load(char const* name, vector<vector<int>> &x)
    {
        load_vec_vec_basic<int>(name, x);
    }

    // load vector of vector
    template <typename T>
    void load_vec_vec_basic(char const* name, vector<vector<T>> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        vector<vector<T>> vec_outer;
        for(auto& e_outer : pt)
        {
            vector<T> vec_inner;// = e_outer.second.get_value<vector<T>>();
            for(auto& e_inner : e_outer.second)
            {
                T basic_item = e_inner.second.get_value<T>();
                vec_inner.push_back(basic_item);
            }
            vec_outer.push_back(vec_inner);
        }
        x = vec_outer;
    }

    template <typename T>
    void load_vec_basic(char const* name, vector<T> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        vector<T> vec;
        for(auto& e : pt)
        {
            T basic_item = e.second.get_value<T>();
            vec.push_back(basic_item);
        }
        x = vec;
    }

    template <typename T>
    void load_set_basic(char const* name, set<T> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        set<T> set_list;
        for(auto& e : pt)
        {
            T basic_item = e.second.get_value<T>();
            set_list.insert(basic_item);
        }
        x = set_list;
    }

    template <typename T>
    void load_vec_enum(char const* name, vector<T> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        vector<T> vec;
        for(auto& e : pt)
        {
            T basic_item = (T) e.second.get_value<int>();
            vec.push_back(basic_item);
        }
        x = vec;
    }

    template <typename T>
    void load_set_enum(char const* name, set<T> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        set<T> set_list;
        for(auto& e : pt)
        {
            T basic_item = (T) e.second.get_value<int>();
            set_list.insert(basic_item);
        }
        x = set_list;
    }

    template <class T>
    void load_vec_obj(char const* name, vector<T> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        vector<T> vec;
        for(auto& e : pt)
        {
            PtreeIArchive arch_child(e.second);
            T obj;
            obj.template serialize<PtreeIArchive>(arch_child, 0);
            vec.push_back(obj);
        }
        x = vec;
    }

    template <class T>
    void load_set_obj(char const* name, set<T> &x)
    {
        Ptree pt = this->ptree.get_child(name);
        set<T> set_list;
        for(auto& e : pt)
        {
            PtreeIArchive arch_child(e.second);
            T obj;
            obj.template serialize<PtreeIArchive>(arch_child, 0);
            set_list.push_back(obj);
        }
        x = set_list;
    }

    void from_json_string(const string &json_text)
    {
        this->ptree = Ptree {};
        std::istringstream i_str(json_text);
        read_json(i_str, this->ptree);
    }
};

Context context_from_string(const string &);
string to_string(const Context &);
