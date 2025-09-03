# Try to find GTest, but don't require it
find_package(GTest QUIET)

if(GTest_FOUND)
    # Test executable with GTest
    add_executable(polygon_offset_tests
        test_point.cpp
        test_edge.cpp
        test_offset.cpp
        test_main.cpp
    )

    target_link_libraries(polygon_offset_tests
        PRIVATE
        polygon_offset
        GTest::gtest
        GTest::gtest_main
    )

    # Enable testing
    include(GoogleTest)
    gtest_discover_tests(polygon_offset_tests)
else()
    # Simple test executable without GTest
    add_executable(polygon_offset_simple_tests
        simple_tests.cpp
    )

    target_link_libraries(polygon_offset_simple_tests
        PRIVATE
        polygon_offset
    )
    
    add_test(NAME simple_tests COMMAND polygon_offset_simple_tests)
endif()