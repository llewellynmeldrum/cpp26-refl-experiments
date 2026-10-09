function(project_detect_cxx_standard out_var)
  set(_project_standard_override "${${PROJECT_NAME}_CXX_STANDARD}")

  if(NOT _project_standard_override STREQUAL "")
    if(NOT _project_standard_override STREQUAL "23" AND
       NOT _project_standard_override STREQUAL "26")
      message(
        FATAL_ERROR
        "${PROJECT_NAME}_CXX_STANDARD must be either 23 or 26; "
        "received `${_project_standard_override}`."
      )
    endif()

    if(NOT "cxx_std_${_project_standard_override}" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
      message(
        FATAL_ERROR
        "Compiler ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} "
        "does not provide cxx_std_${_project_standard_override}."
      )
    endif()

    set(_project_cxx_standard ${_project_standard_override})
  elseif("cxx_std_26" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(_project_cxx_standard 26)
  elseif("cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(_project_cxx_standard 23)
  else()
    message(
      FATAL_ERROR
      "expected_fs requires C++23 or newer. "
      "Compiler ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} does not provide cxx_std_23."
    )
  endif()

  message(STATUS "Using C++${_project_cxx_standard}")
  set(${out_var} ${_project_cxx_standard} PARENT_SCOPE)
endfunction()

function(project_apply_cxx_standard target visibility)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Cannot apply the project C++ standard to unknown target `${target}`.")
  endif()

  set_target_properties(
    ${target}
    PROPERTIES
      CXX_STANDARD ${PROJECT_CXX_STANDARD}
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
  )

  target_compile_features(${target} ${visibility} cxx_std_23)
endfunction()
