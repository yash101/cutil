add_subdirectory("${PROJECT_SOURCE_DIR}/vendor/googletest" "vendor/googletest")
include(GoogleTest)
enable_testing()

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER vendor)
set_target_properties(gtest_main PROPERTIES FOLDER vendor)
set_target_properties(gmock PROPERTIES FOLDER vendor)
set_target_properties(gmock_main PROPERTIES FOLDER vendor)

macro(add_test TESTNAME)
  add_executable(${TESTNAME} ${ARGN})
  target_link_libraries(${TESTNAME} gtest gmock gtest_main)
  target_link_libraries(${TESTNAME} cutil)
  gtest_discover_tests(
    ${TESTNAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_PROJECT_DIR}"
  )
  set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

add_test(cutil_test_list test.list.cpp)
add_test(cutil_test_hmap test.hmap.cpp)
add_test(cutil_test_hash test.hash.cpp)
