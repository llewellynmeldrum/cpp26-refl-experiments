# Config logic
Config files are generated from structs in code.


## Good ideas:
- Defaults are defined in code. The UI can include a 'reset to defaults', and the reader can fall back to them as well.
i.e: For each field in the file, if it matches a field in the struct -> it overrides. If no field is there, 
- Structs must register as 'config structs' via a specialization of some struct - this should also probably be where the config filename is stored. It should probably be constexpr to avoid issues. This can also be a point to add some validation.


```cpp
template<typename T>
struct ConfigStructRegistration{
    static constexpr inline std::string_view toml_filename; // (relative path)
};

// in config.hpp
struct Config{
    RANGE(5.0f, 120.0f) f32 fov;
    RANGE(0.1f, 10.0f)  f32 move_speed;
};

template<>
struct ConfigStructRegistration<Config>{
    static constexpr inline std::string_view toml_filename = "config.toml"sv; 
};
```



## Writing/saving files (high level logic)
- if the file doesnt exist (missing file for registered config struct)
=> Notify the user, and create the file with value defaults (dont skip any keys)
- Atomic writes: write to a temporary config.toml.tmp file and `fs::rename()` to config.toml -> saves the old config file if a crash happens during a file write.


## Reading/loading files (high level logic)
- If a key in the config does not match any key in struct  (unknown key in config file)
    => log a warning with the key and value, so that the value isnt lost if we just renamed something.
- If a key has a value of type not compatible with its (incorrect value type in config file)
    => warn with the value and its constrained/corrected version 
- If a key has a value which is outside of the bounds defined (out of bounds value in config file)
    => warn with the value and its constrained/corrected version 
- If a struct in the config was not filled in by any key (missing key in the config file), assign it to its default, 

Optionally, a mode could cause hard erroring on all of these. Thats probably safer tbh, but it might get annoying.

- If a file contains a certain amount of errors, consider reccomending the user to delete the file/offer to recreate it from the defaults.
