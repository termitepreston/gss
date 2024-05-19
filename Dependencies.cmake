include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(gss_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#10.2.1")
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.14.1
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.6.0")
  endif()

  cpmaddpackage(
    NAME
    Boost
    VERSION
    1.84.0
    URL
    https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
    URL_HASH
    SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
    OPTIONS
    "BOOST_ENABLE_CMAKE ON" "BOOST_URL_BUILD_TESTS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio\\\;endian\\\;logic\\\;static_string\\\;url" # Note the escapes!
  )

  if (NOT TARGET Boost::beast)
    cpmaddpackage("gh:boostorg/beast#boost-1.84.0")
  endif()

  if (NOT TARGET nlohmann_json::nlohmann_json)
    cpmaddpackage("gh:nlohmann/json@3.11.2")
  endif()

  if(NOT TARGET CLI11::CLI11)
    cpmaddpackage("gh:CLIUtils/CLI11@2.4.2")
  endif()
endfunction()
