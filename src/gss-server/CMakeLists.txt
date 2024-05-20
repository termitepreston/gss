add_executable(gss-server main.cpp net.hpp beast.hpp shared_state.hpp shared_state.cpp websocket_session.hpp websocket_session.cpp listener.hpp listener.cpp http_session.hpp http_session.cpp Ticker.hpp Ticker.cpp Task.hpp Task.cpp)

target_link_libraries(
  gss-server
  PRIVATE gss::gss_options
          gss::gss_warnings)

target_link_system_libraries(
  gss-server
  PRIVATE
          CLI11::CLI11
          fmt::fmt
          spdlog::spdlog
          Boost::beast
          Boost::url
          nlohmann_json::nlohmann_json
          gss::router
          gss::containers
          )

target_include_directories(gss-server PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
