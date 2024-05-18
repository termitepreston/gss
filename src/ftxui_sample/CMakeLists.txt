add_executable(intro main.cpp net.hpp beast.hpp shared_state.hpp shared_state.cpp websocket_session.hpp websocket_session.cpp listener.hpp listener.cpp http_session.hpp http_session.cpp)

target_link_libraries(
  intro
  PRIVATE gss::gss_options
          gss::gss_warnings)

target_link_system_libraries(
  intro
  PRIVATE
          CLI11::CLI11
          fmt::fmt
          spdlog::spdlog
          Boost::beast
          )

target_include_directories(intro PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
