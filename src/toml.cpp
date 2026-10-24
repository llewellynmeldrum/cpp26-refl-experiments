#include <fstream>
#include <iostream>
#include <filesystem>
#include "meta_shorthand.hpp"


//#include <toml++/toml.hpp>
#include <expected_fs/expected_fs.hpp>

#include "toml.hpp"
#include "file_io.hpp"
#include "primitive_types.hpp"
#include "logger.hpp"
//#include "config_meta.hpp"
#include "cfg_registration.hpp"
#include "test_config.hpp"
#include "cfg_helpers.hpp"

namespace fs = std::filesystem;
namespace efs = expected_fs;
#if CPP26
#ifndef STD_META_SHORTHAND
    namespace meta = std::meta;
    #define STD_META_SHORTHAND
#endif 
#endif // CPP26








