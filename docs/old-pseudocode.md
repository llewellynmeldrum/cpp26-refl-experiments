
```python

def get_suffix_comment(v, parent):
    if parent.is_array():
        return  f"{:v.type_string} {:parent.identifier_string}[{:array_index}]"
    else:
        return  f"{:T.type_string} range={:T.range.min,T.range.max}"


def get_field_key(v):
    # place to add back renames if i want
    return identifier_of(v)

def serialize_scalar(v, parent): 
    suffix_comment = get_suffix_comment(v,parent);
    write("{} = {}", get_field_key(v), serialize_scalar_value(v))

    if not parent.is_root_node() or parent.is_array():
        write(",") # fields inside inline tables or inside arrays get commas
    write("{}\n", suffix_comment)
    
def serialize_struct(v, parent): 
    suffix_comment = get_suffix_comment(v,parent);
    write("{{ {}\n",suffix_comment);
    increase_indent()
        serialize_fields_of(v, top_level_field, array_element)
    decrease_indent()
    write("}")
    if not parent.is_root_node() or parent.is_array():
        write(",") # fields inside inline tables or inside arrays get commas
    write(suffix_comment)


def serialize_array(arr, parent, suffix_comment): 
    write("[ {}\n",suffix_comment);
    for each v in arr:
        if if_struct_like(m):
            serialize_struct(m,arr, get_suffix_comment(v,parent))
        else if if_array_like(m):
            serialize_array(m,arr)
        else:
            serialize_scalar(m,arr)
    write("]")
    write("{}\n",suffix_comment);
    if (not parent.is_root_node()) or parent.is_array():
        write(",") # arrays inside inline tables or inside arrays get commas



# called on structs. The direct members of a struct CANNOT be array members, but it could itself be an array member.
# this however is handled in 
def serialize_fields_of(T, parent, is_inline : bool):
    # none inline fields are:
    # 1. scalar members of root (parent = root)
    # 2. scalar members whose parents are struct members of root (parent.parent = root)
    # in other words, if your parent is inline, you are.
    # if your parent is root, you are NOT inlined.
    # otherwise, if your parent is inlined, 
    # if you are children of children of root, you are inlined.
    # if you are array elements , you are inlined. (parent = array, parent.parent = root)
    if parent is null:  # root field, members are not inlined
        is_inline = false;
    if pa

    for member in T.members:
        if is_toml_scalar(m):
            serialize_scalar(m, , array_member)
    for m in T:
        if if_struct_like(m):
            serialize_struct(m,top_level_field, array_member)
        else if if_array_like(m):
            serialize_array(m,top_level_field,array_member)
    
```
        


