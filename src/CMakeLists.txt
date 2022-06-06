set(
  cutil_src
    map.c
    vector.c
    list.c
)

add_library(
  cutil_obj OBJECT
    ${cutil_src}
)
set_property(
  TARGET cutil_obj
  PROPERTY POSITION_INDEPENDENT_CODE 1
)
target_compile_features(cutil_obj PRIVATE c_std_11)

add_library(
  cutil SHARED
    $<TARGET_OBJECTS:cutil_obj>
)

add_library(
  cutil_static STATIC
    $<TARGET_OBJECTS:cutil_obj>
)

add_library(cutil::cutil ALIAS cutil)
add_library(cutil::cutil_shared ALIAS cutil)
