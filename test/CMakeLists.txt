include_directories(${INCLUDE_PATH}/test)
add_definitions(-DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

function(add_himalaya_test name)
  add_executable(
    ${name}
    ${name}.cpp
    )
  target_link_libraries(
    ${name}
    Himalaya_static
    DSZ_static
    )
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_himalaya_test(test_example)
add_himalaya_test(test_EFT_expressions)
add_himalaya_test(test_FO_expressions)
add_himalaya_test(test_Himalaya_interface)
add_himalaya_test(test_delta_lambda)

if(Mathematica_FOUND)
  Mathematica_WolframLibrary_ADD_TEST (
    NAME test_LibraryLink
    TARGET Himalaya_LibraryLink
    SCRIPT test_LibraryLink.m)
endif()
