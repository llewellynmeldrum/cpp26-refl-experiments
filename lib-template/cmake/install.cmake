# =============================================================================
# Install rules. `cmake --install <build> --prefix <dir>` produces:
#
#   <dir>/include/mylib/*.hpp                          headers (from the FILE_SET)
#   <dir>/lib/libmylib.a                               only if src/ has .cpp files
#   <dir>/<share|lib>/cmake/mylib/mylibConfig.cmake    entry point for find_package(mylib)
#   <dir>/<share|lib>/cmake/mylib/mylibConfigVersion.cmake
#   <dir>/<share|lib>/cmake/mylib/mylibTargets.cmake   defines the imported target mylib::mylib
#
# A consumer then writes find_package(mylib) with CMAKE_PREFIX_PATH=<dir>.
# =============================================================================
include(GNUInstallDirs)              # CMAKE_INSTALL_INCLUDEDIR, LIBDIR, DATADIR...
include(CMakePackageConfigHelpers)   # configure_package_config_file, write_basic_package_version_file

# Header-only packages do not depend on the CPU architecture, so they go under
# share/. Compiled ones go under lib/ (which may be lib64 or lib/<triple>).
if(LIB_SOURCES)
    set(_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
    set(_arch_independent "")
else()
    set(_cmake_dir "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")
    set(_arch_independent ARCH_INDEPENDENT)
endif()

# 1. Copy the built artifacts and headers, and record them in an "export set".
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
)

# 2. Turn the export set into mylibTargets.cmake, which recreates the target
#    as an IMPORTED target named mylib::mylib on the consumer's side.
install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE   ${PROJECT_NAME}::
    DESTINATION ${_cmake_dir}
)

# 3. mylibConfig.cmake: what find_package(mylib) actually loads.
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${_cmake_dir}
)

# 4. mylibConfigVersion.cmake: answers "is this version acceptable?"
#    SameMinorVersion: asking for 0.1 accepts 0.1.x only. Right for 0.x semver,
#    where every minor bump may break the API. Switch to SameMajorVersion at 1.0.
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
    ${_arch_independent}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${_cmake_dir}
)
