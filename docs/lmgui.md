# Example layout of the spec + scope combo
```cpp
struct WidgetSpec;
struct WidgetScope;

struct WidgetSpec{
    // Constructs WidgetScope
    auto begin() -> WidgetScope;

    // Builder methods
    auto set_a(int)         -> auto&&;
    auto set_b(float)       -> auto&&;
    auto set_s(std::string) -> auto&&;

    // fields
    int         a{};
    float       b{};
    std::string s{};
};

struct [[nodiscard]] WidgetScope{
    WidgetScope()=delete;
    WidgetScope(WidgetScope const&)=delete;
    WidgetScope& operator=(WidgetScope const &)=delete;
    WidgetScope& operator=(WidgetScope &&)=delete;
    // No move, no copy. No default ctor

    operator bool() {
        return is_visible;
    }

    bool is_visible;
    // fields
    int         a{};
    float       b{};
    std::string s{};


    WidgetScope(WidgetSpec const&); // Should begin the widget, usually calling ImGui::Begin...
    ~WidgetScope(); // Should end the widget, usually calling ImGui::End...
};

```

```cpp
struct WidgetSpec{
    std::string name{""};
    WidgetFlags flags;
    bool isVisible = false;
    std::vector<WidgetColumnHeader> headers{};
    
    auto&& set_name(this auto&& m, std::string_view name){
        m.name = name;
        return std::forward<decltype(m)>(m);
    }
    auto&& set_flags(this auto&& m, WidgetFlags flags){
        m.flags = flags;
        return std::forward<decltype(m)>(m);
    }
    auto&& set_column_headers(this auto&& m, std::vector<WidgetColumnHeader> headers) {
        m.headers.assign_range(headers);
        return std::forward<decltype(m)>(m);
    }
    auto&& add_column_header(this auto&& m, std::string header, WidgetColumnFlags col_flags = WidgetColumnFlagBits::eNone, std::string hint=""){
        m.headers.emplace_back(header,col_flags, hint);
        return std::forward<decltype(m)>(m);
    }
    auto begin();

};
struct WidgetScope{
    explicit WidgetScope(WidgetSpec info);
    ~WidgetScope();
    NO_MOVE_CTOR(WidgetScope)
    NO_MOVE_ASSN(WidgetScope)
    NO_COPY_CTOR(WidgetScope)
    NO_COPY_ASSN(WidgetScope)

    operator bool(){ return isVisible; }

    auto begin(this auto&& m) -> auto&&;

    auto end() -> void{
        isVisible = false; 
        ImGui::EndWidget();
    }

};
```
