add_library(containers list.cpp)

target_include_directories(containers PUBLIC .)

add_library(gss::containers ALIAS containers)

