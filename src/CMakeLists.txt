add_library(ilog ilog.cc ilog.h)
add_library(dlx dlx.cc dlx.h)
target_link_libraries(dlx ilog)
add_library(chip chip.cc chip.h)
target_link_libraries(chip ilog)

add_executable(gen_cover gen-cover.cc)
target_link_libraries(gen_cover ilog chip dlx)

add_executable(gen_sols gen-sols.cc)
target_link_libraries(gen_sols ilog chip)
