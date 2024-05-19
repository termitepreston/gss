add_library(router router.hpp impl/matches.cpp detail/impl/router.cpp detail/router.hpp )


target_link_libraries(router PRIVATE Boost::url Boost::beast)

source_group("" FILES router.hpp)

target_include_directories(router PUBLIC .)

add_library(gss::router ALIAS router)