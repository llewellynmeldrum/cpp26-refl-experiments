
Choice: the customisation point
- std::formatter style  - specializations of a struct 


 
std::swap style
```
```
- Less boilerplate 

std::formatter style
- better localization if multiple related functions are required


Ideally, for ui generation, annotations should serve as a secondary point of customisation. That is to say:
1. if the type has no annotation:
cfg::show_window<T>(T const& cfg) -> selects cfg::GuiHandler<T>::show_window(T const& cfg);
2. if the type has an annotation like:
```cpp
template<typename T>
struct GuiHandler{
    auto show_view_window(T const& v) -> void = delete;
    auto show_edit_window(T& v) -> void = delete;
};

using T = SomeConfigType;
template<>
struct GuiHandler<T>{
    auto show_view_window(T const& v) -> void{
        // performs custom imgui calls to display a non-editable window, for fps, frametime, counters, etc.
    }
    auto show_edit_window(T& v) -> bool{
        // performs custom imgui calls to display a editable window, for debug toggles, worldgen stuff, noise configs
    }
};

auto show_window<T>(T const& v) -> void{
    GuiHandler<T>{}.show_window(v);
}

// For defaulting:
template<typename T>
struct AutoGuiHandler{
    auto show_view_window(T const& v) -> void{
        // Performs automatic, reflection based (probably recursive) imgui calls
    }
    auto show_edit_window(T& v) -> void{
        // Performs automatic, reflection based (probably recursive) imgui calls
    }
};
using U = OtherConfigType;
// structs can use it by:
template<>
struct GuiHandler<U> : AutoGuiHandler{};

// or, 
#define REGISTER_AUTO_GUI_HANDLER(TYPE) template<> struct GuiHandler<TYPE> : AutoGuiHandler{};
```




```cpp
// Idea: separate the 



```
# On the topic of arrays of structs
In thinking about how ui generation should work, ive been wondering how arrays of structs should work.
For example, if you have: 

```cpp
struct Config{
    ui_config cfg;
};
```
