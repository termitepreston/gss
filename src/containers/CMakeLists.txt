include(GenerateExportHeader)


add_library(containers sample_library.cpp)



add_library(gss::containers ALIAS containers)

target_link_libraries(containers PRIVATE gss_options gss_warnings)

target_include_directories(containers ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(containers PUBLIC cxx_std_20)

set_target_properties(
  containers
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(containers EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/gss/containers_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(containers PUBLIC CONTAINERS_STATIC_DEFINE)
endif()
