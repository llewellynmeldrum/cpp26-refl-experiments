# ui gen

## Customisation points: 
Im not sure how to decide on a customization point for these types of things. std::formatter sets a pretty high bar, but I do find that adding new types to std::formatter is pretty annoying. being FORCED to define two functions kindof sucks. Having something like std format, but giving the option of a default via some sugar would be nicer.
Std formatter also somewhat requires specializers to understand the sink output context, and has strict adherence to the 'structure' of the inital template definition (which is quite convoluted)
### templated function specialization:
(std::swap() style)
|Pros |Cons |
|-|-|
|simple |no intrinsic locality to related functions|
simplest, 
### templated struct specialization:
(std::hash/std::formatter style)
|Pros |Cons |
|-|-|
|simple |no intrinsic locality to related functions|


### templated function with `if constexpr` branches:


For the purposes of UI gen:
- you definitely need at least function specialization for primitives. I.e:

```cpp
template<typename T>
auto show_element(T& v) -> bool;
// returns true if the value changed
// optionally, it could have an overload that contained a callback which could be triggered if the value changes

// The simplest would look like:
template<>
auto show_element<bool>(auto& v){
    return ImGui::CheckBox(&v);
}
// And the most complicated:
[[=ui_format(UI_Format::eRgba01)]] glm::vec4 rgba;
// ALSO 
template<>
auto show_element<bool>(auto& v){
    return ImGui::CheckBox(&v);
}

```
Its probably a good idea to have two types of functions:
show_element -> limited to primitives or simple aggregate structs, generally produces a single ui element.

