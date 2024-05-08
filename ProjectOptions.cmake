include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(gss_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(gss_setup_options)
  option(gss_ENABLE_HARDENING "Enable hardening" ON)
  option(gss_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    gss_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    gss_ENABLE_HARDENING
    OFF)

  gss_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR gss_PACKAGING_MAINTAINER_MODE)
    option(gss_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(gss_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(gss_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(gss_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(gss_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(gss_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(gss_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(gss_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(gss_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(gss_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(gss_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(gss_ENABLE_PCH "Enable precompiled headers" OFF)
    option(gss_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(gss_ENABLE_IPO "Enable IPO/LTO" ON)
    option(gss_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(gss_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(gss_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(gss_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(gss_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(gss_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(gss_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(gss_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(gss_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(gss_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(gss_ENABLE_PCH "Enable precompiled headers" OFF)
    option(gss_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      gss_ENABLE_IPO
      gss_WARNINGS_AS_ERRORS
      gss_ENABLE_USER_LINKER
      gss_ENABLE_SANITIZER_ADDRESS
      gss_ENABLE_SANITIZER_LEAK
      gss_ENABLE_SANITIZER_UNDEFINED
      gss_ENABLE_SANITIZER_THREAD
      gss_ENABLE_SANITIZER_MEMORY
      gss_ENABLE_UNITY_BUILD
      gss_ENABLE_CLANG_TIDY
      gss_ENABLE_CPPCHECK
      gss_ENABLE_COVERAGE
      gss_ENABLE_PCH
      gss_ENABLE_CACHE)
  endif()

  gss_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (gss_ENABLE_SANITIZER_ADDRESS OR gss_ENABLE_SANITIZER_THREAD OR gss_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(gss_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(gss_global_options)
  if(gss_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    gss_enable_ipo()
  endif()

  gss_supports_sanitizers()

  if(gss_ENABLE_HARDENING AND gss_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR gss_ENABLE_SANITIZER_UNDEFINED
       OR gss_ENABLE_SANITIZER_ADDRESS
       OR gss_ENABLE_SANITIZER_THREAD
       OR gss_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${gss_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${gss_ENABLE_SANITIZER_UNDEFINED}")
    gss_enable_hardening(gss_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(gss_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(gss_warnings INTERFACE)
  add_library(gss_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  gss_set_project_warnings(
    gss_warnings
    ${gss_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(gss_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    gss_configure_linker(gss_options)
  endif()

  include(cmake/Sanitizers.cmake)
  gss_enable_sanitizers(
    gss_options
    ${gss_ENABLE_SANITIZER_ADDRESS}
    ${gss_ENABLE_SANITIZER_LEAK}
    ${gss_ENABLE_SANITIZER_UNDEFINED}
    ${gss_ENABLE_SANITIZER_THREAD}
    ${gss_ENABLE_SANITIZER_MEMORY})

  set_target_properties(gss_options PROPERTIES UNITY_BUILD ${gss_ENABLE_UNITY_BUILD})

  if(gss_ENABLE_PCH)
    target_precompile_headers(
      gss_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(gss_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    gss_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(gss_ENABLE_CLANG_TIDY)
    gss_enable_clang_tidy(gss_options ${gss_WARNINGS_AS_ERRORS})
  endif()

  if(gss_ENABLE_CPPCHECK)
    gss_enable_cppcheck(${gss_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(gss_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    gss_enable_coverage(gss_options)
  endif()

  if(gss_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(gss_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(gss_ENABLE_HARDENING AND NOT gss_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR gss_ENABLE_SANITIZER_UNDEFINED
       OR gss_ENABLE_SANITIZER_ADDRESS
       OR gss_ENABLE_SANITIZER_THREAD
       OR gss_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    gss_enable_hardening(gss_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
