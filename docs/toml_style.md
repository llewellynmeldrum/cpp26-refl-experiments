# Toml style

This style is focused on being human readable and writable, and preserving ease of parsing and generating.
The style seeks to mimic C/C++ designated initializers/ aggregate initialization as close as possible.


# Example 1:
```toml
[name] # Name
first = 'Tom'       # std::string 'first_name' NOTE: ....
last = 'Phillips'   # std::string 'last_name' 
```
---
Corresponding C++:
```cpp
struct Name{
    NOTE("testing")
    RENAME(first) std::string first_name = "Tom";

    RENAME(last)  std::string last_name = "Phillips";
};
struct Root{
    Name name;
};
```
---
This example shows the basics: 
- 'top level' structs get their own toml header.
- By default, an inline comment begins with the type of the variable
- Followed after that, in the comment, is the true identifier of the field in source code, iff the AttributeRename{} annotation is used
- 

# Example 2:
```toml
[player] # Player
name = { # Name
    first = 'Tom',       # std::string 'first_name' (range=)
    last = 'Phillips',   # std::string 'last_name' 
}
pos = {              # glm::vec2
    x = 0,  # f32
    y = 0,  # f32
}
```
---
Corresponding C++:
```cpp
struct Name{
    RENAME(first) std::string first_name = "Tom";
    RENAME(last)  std::string last_name = "Phillips";
};
struct Player{
    Name name;
    glm::vec2 pos{0,0};
};
struct Root{
    Player player;
};
```
---
- As we can see, only non-scalar, non-array type fields get their own separate header.
- All other rules still apply

# Example 4:
```toml
players = [ # std::vector<Player>
    { # Player (players[0])
        name = { # Name
            first = 'Tom',       # std::string
            last = 'Phillips',   # std::string
        },
        pos = {              # glm::vec2
            x = 0,  # f32
            y = 0,  # f32
        },
    }, 
]
strings = [ # std::vector<std::string>
    "a", # std::string (strings[0]) range=[0,10) NOTE: ...
    "b", # std::string (strings[1])
    "c", # std::string (strings[2])
]
```
---
Corresponding C++:
```cpp
struct Name{
    RENAME(first) std::string first_name;
    RENAME(last)  std::string last_name;
};
struct Player{
    Name name;
    glm::vec2 pos;
};
struct Root{
    std::vector<Player> players;
    std::vector<std::string> strings;
};
```
--- 
-  members of arrays have '(ARRAY_NAME[INDEX])' inside their comment after the type name string.

So formally, the comment locations (excluding top level structs):

For members of an array of structs, the suffix comment is placed after the opening brace.
For members of an array of scalars, the suffix comment is placed after the trailing comma.

QUESTION:
When are fields not formatted using the key = value thing?
1. when they are scalar members of an array (they dont have an identifier.)
2. when they are struct like, and their parent is the root (they get a header)

## the ONLY time we produce a header "[NAME]\n" is when there is a struct whos parent is root.

i.e:

```python
def serialize_root_table(root):
    write("{}\n", root.annotations.note)
    for field in scalar_members_of(root):
        serialize_scalar_field(field ,root, get_field_comment(field,root), False)
    for field in toml_array_members_of(root):
        serialize_toml_array_field(field, root, get_field_comment(field,root), False)
    for field in struct_like_members_of(root):
        serialize_struct_like_field(field, root, get_field_comment(field,root), False)

def get_field_comment(v, parent, array_index : optional<int>):
    if parent.is_array():
        return  format("# {} {} {}[{}]",
                       type_string_of(v),
                       has_range_attr(v) ? range_attr_of(v) : "",
                       key_of(parent), array_index
                )
    else:
        return  format("# {} {}",
                       type_string_of(v),
                       has_range_attr(v) ? range_attr_of(v) : "",
                )

def serialize_scalar_value(field, parent):
    # ...

def serialize_scalar_field(field, parent, field_comment, include_comma):
    # perform range checks on the field value
    if parent is toml_array: # array elements dont have names
        write("{}, {}\n", serialize_scalar_value(field), field_comment)
        "eg: 1"
    else:
        write("{} = {}{} {}\n", key_of(field), serialize_scalar_value(field), include_comma ? "," : "", field_comment)
        "eg: name = 1"

    
def serialize_struct_like_field(field, parent, field_comment, include_comma):
    assert(field is not root)
    if parent is root:
        write("[{}] {} \n",key_of(field), field_comment);
        "eg: [NAME] # field_comment"
    else if parent is toml_array:
        write("{{ {} \n", field_comment);
        "eg: { # field_comment"
    else 
        write("{} = {{ {} \n",key_of(field), field_comment);
        "eg: name = { # field_comment"

    if include_comma: indent++;
    for member_field in members_of(field):
        children_include_comma = parent is not root
        serialize_field(member_field, field, get_field_comment(member_field, field), children_include_comma)
    if include_comma: indent--;

    if include_comma:
        write("},\n");
    else: 
        write("\n"); 

def serialize_toml_array_field(field, parent, field_comment, include_comma):
    if parent is toml_array:
        write("[ {} \n", field_comment);
        "eg: [ # field_comment"
    else 
        write("{} = [ {} \n",key_of(field), field_comment);
        "eg: name = [ # field_comment"

    if include_comma: indent++;
    for (idx, arr_element) in field:
        serialize_field(arr_element, field, get_field_comment(arr_element, arr, idx), True)
    if include_comma: indent--;

    if include_comma:
        write("],\n");
    else:
        write("]\n");



# generic version for recursive calls
def serialize_field(field, parent, field_comment, include_comma):
    if field is toml_scalar: 
        serialize_scalar_field(field, parent,field_comment,include_comma)
    else if field is struct_like:
        serialize_struct_like_field(field, parent,field_comment, include_comma)
    else if field is toml_array:
        serialize_toml_array_field(field, parent,field_comment, include_comma)


```
inline members are serialized as follows:
### Arrays (arrays dont care if they are inline or not):
<key> [ <suffix-comment>
    <key[0]> ,
    <key[1]> ,
    <key[2]> ,
    <...
]

// struct-like types 
// inline:
<key> = { <suffix-comment>
    <key.field0>, 
    <key.field1>,
}



# Annotation specification

Types of annotations:

|Macro|Type|Purpose|Applicable to?|
|-----|---------|-------|---|
|NOTE(txt)|Annotations::Note| Triggers an inline comment at the end f"NOTE: {:txt}".| *| 
|RANGE(min, max)|Annotations::Range| Triggers an inline comment with the range f"Range=[min,max)".| Anything non array like.| 
|SKIP()|Annotations::Skip| Prevents serializing or reading of the field.| Anything? i guess.| 









