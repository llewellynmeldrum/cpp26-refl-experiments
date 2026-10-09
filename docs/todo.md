
# Ideas:

## refl::inspect_type()
Produces some textual output about characteristics of the class:
- Special member functions, whether they were compiler generated or not,
- is class: 
    - Copy constructible
    - Default ctorable
    - Move ctorable
    - Move assignable
    - Copy assignable
    - Aggregate
    - 



1. fix clangd
2. After fixing clangd, I would like to figure out which file format I should use, and whether I should use a custom serializer


## Serialization
```cpp
template<typename T>
auto serialize(T const& v /*, SerializeConfig const& cfg*/) -> std::span<std::byte>{
}

```

## Decisions: 
- 1 file per serialized struct?

i.e RenderDebug.toml :
```toml
wireframe = false
tonemap = "aces"
```

OR: 
- N files with N serialized structs inside

/debug.toml
```toml
[debug_render]
key_a = action_move_left
key_d = action_move_right

[debug_audio]

```
/keybinds.toml
```toml
[keybinds]

[keybinds.movement]
key_a = action_move_left
key_d = action_move_right
```
/player_settings.toml


So the advantage of one file per struct is:

### 1 config struct per file
Pros: 
- simpler
- Probably makes the meta hash idea easier

Cons: 
- somewhat limiting? Like what if I wantc
### N config struct(s) per file


ALSO: 
I think there should be some type of 'meta-hash' included as a comment at the top of each file,
which acts like a version stamp. It could be generated as a hash of all the field names, 
and checked on read. If the version doesnt match, either generate a warning or fail outright.


## MORE SPECIFIC TOML STUFF

TOML is mostly composed of key = value pairs: 
```toml
key     = "value" # \n must occur after each key = value pair
key_2   = 42
```
- Whitespace is mostly ignored in toml.
- Newlines separate key-value pairs.
- Comments are written with single `#` chars.


### Tables
```toml
[foo]
# Declares a 'table' foo.
```

Tables represent collections of key-value pairs.
The names of tables have the same as keys. For my purposes, that just means to respect the java syntax for nested types.
```c++
struct Foo{
    struct Bar{
        char baz = 'A';
    }bar;
};
```

```toml
[Foo]
[Foo.bar]
baz = 'B'

```


### Arrays




```toml
[name]
first = "Tom"
last = "Preston-Werner"

[point]
x = 1
y = 2

[contact.personal]
name = "Donald Duck"
email = "donald@duckburg.com"

[contact.work]
name = "Coin cleaner"
email = "donald@ScroogeCorp.com"

```

With inline tables:
```toml
name = { first = "Tom", last = "Preston-Werner" }
point = {x=1, y=2}
animal = { type.name = "pug" }
contact = {
    personal = {
        name = "Donald Duck",
        email = "donald@duckburg.com",
    },
    work = {
        name = "Coin cleaner",
        email = "donald@ScroogeCorp.com",
    },
}
```



```toml
person = [
    arms = [

    ]
]
# represents a std::array<Finger>
fingers = [
    { overall_length = 1.0, fingernail_length = 0.1} 
    { overall_length = 2.0, fingernail_length = 0.2} 
    { overall_length = 3.0, fingernail_length = 0.3} 
]
# Same as:
[[fingers]]
overall_length = 1.0
fingernail_length = 0.1

[[fingers]]
overall_length = 2.0
fingernail_length = 0.2

[[fingers]]
overall_length = 3.0
fingernail_length = 0.3
```



Regular table:

representing: std::array<glm::ivec2, 2> { 
    {1, 2},
    {3, 4},
};

```toml
# Regular table:
[[positions]]
x = 1
y = 2

[[positions]]
x = 3
y = 4

# inline table
positions = [ 
    {x = 1, y = 2},
    {x = 3, y = 4},
]
```


for every non static field within a struct, we must find all of those which are not going
to generate their own headers, and put them first.
I.e 
key1 = 1
key2 = 2
key3 = 3

[bar]
key4 = 4 # <-- key4, and anything following (until the next header) is part of the [bar] node now.


Examples of those which need to come first:
<ScalarValue> = any valid scalar value - a string, integer, float, 
<Value> = anything that might appear on the RHS of a key-value expression in TOML.
         This includes 'inline tables':

```toml
# case 0: <Key> = <ScalarValue>
[TableName]
Key = ScalarValue                                 #(key_scalar_val_pair)

# case 1: <Key> = [<ScalarValue_0>, ... ,<ScalarValue_N-1>]
[TableName]
Key = [ScalarValue_0, ScalarValue_1]             #(key_scalar_val_array_pair)


# case 2: Key = array of non-scalars.
Key = [{ key = []}, ScalarValue ... ]             #(key_scalar_val_array_pair)


rows = [
    { 
        columns = [
            {x = 1, y = 2},
            {x = 3, y = 4},
        ]
    },
    { 
        columns = [
            {x = 1, y = 2},
            {x = 3, y = 4},
        ]
    },

]

    ```
When writing out arrays of structs, I think using inline tables is reasonable.

Individual structs themselves (which are members of the root node) should use regular tables.

i.e :

```c++
struct Foo{
    struct Nested{

    };
};

```
So truly, the grammar is simply: 

KEY_VALUE_PAIR          <- KEY "=" VALUE "\n"
KEY                     <- (any string, java style nested structs syntax)
VALUE                   <- INLINE_TABLE || SCALAR_VALUE
INLINE_ARRAY_OF_TABLES  <- "[" (INLINE_TABLE_ELEMENT)+ "]"
SCALAR_VALUE            <- (any scalar literal)
INLINE_TABLE            <- (INLINE_TABLE_ELEMENT || )
INLINE_TABLE_ELEMENT    <- { INLINE_TABLE_ELEMENT ", "}"




# RULES FOR TOML SERIALIZING
1. Root struct for the config file occupies the root node.


```toml
# Scalar fields of the root table (scalar fields of the struct)

# Subtables of the root node (nonscalar fields of the struct)
```
