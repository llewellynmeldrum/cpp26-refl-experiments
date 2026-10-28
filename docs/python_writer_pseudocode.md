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
