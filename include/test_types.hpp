#pragma once 
#include <chrono>
#include <format>
#include "reflect_formatter.refl.hpp"
enum struct Family{
    Hominidae
};
template<> struct std::formatter<Family> : refl::auto_formatter{};

enum struct Tribe{
    Pongini, // orangutangs
    Gorillini,
    Hominini
};
template<> struct std::formatter<Tribe> : refl::auto_formatter{};

enum struct Genus{
    Pongo, // Orangutans
    Pan, // Chimps and bonobos
    Homo, // Humans 
    Gorilla, 

};
template<> struct std::formatter<Genus> : refl::auto_formatter{};

struct E{
    int b{};
};

struct D{
    float f{};
    float f1{};
    float f2{};
    float f3{};
};

struct C{
    int a{};
    float b{};
    D d{};
};

struct B{
    C c{};
    E e{};
};

struct A{
    float bar{};
    B b{};
    int foo{};
};

struct Date{
    std::chrono::year_month_day ymd;
    
};
template<>
struct std::formatter<Date> : refl::auto_formatter{};

struct Chimpanzee{
    static constexpr auto family = Family::Hominidae;
    static constexpr auto tribe = Tribe::Hominini;
    static constexpr auto genus = Genus::Pan;
    std::string_view full_name;
    Date date_of_birth;
    int secret{42};
};
template<>
struct std::formatter<Chimpanzee> : refl::auto_formatter{};

struct WesternGorilla{
    static constexpr auto family = Family::Hominidae;
    static constexpr auto tribe = Tribe::Gorillini;
    static constexpr auto genus = Genus::Gorilla;
    Date date_of_birth;
};
template<>
struct std::formatter<WesternGorilla> : refl::auto_formatter{};

struct vec3{
    //union{
    //    struct{
    //        float x,y,z;
    //    };
    //    std::array<float,3> arr;
    //};
    float x,y,z;
};
template<>
struct std::formatter<vec3> : refl::auto_formatter{};

template<> struct std::formatter<E> : refl::auto_formatter{};
template<> struct std::formatter<D> : refl::auto_formatter{};
template<> struct std::formatter<C> : refl::auto_formatter{};
template<> struct std::formatter<B> : refl::auto_formatter{};
template<> struct std::formatter<A> : refl::auto_formatter{};

struct HomoSapiens{
    static constexpr auto family = Family::Hominidae;
    static constexpr auto tribe = Tribe::Hominini;
    static constexpr auto genus = Genus::Homo;

    std::string_view full_name;
    Date date_of_birth;
    Chimpanzee pet_chimp;
};
template<>
struct std::formatter<HomoSapiens> : refl::auto_formatter{};

