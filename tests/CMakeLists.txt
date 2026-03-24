include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(test_parallel_consistency
    test_parallel_consistency.cpp
)

target_link_libraries(test_parallel_consistency
    PRIVATE
    replay_core
    GTest::gtest_main
)

gtest_discover_tests(test_parallel_consistency
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_executable(test_market_data_loader
  test_market_data_loader.cpp
)

target_link_libraries(test_market_data_loader
  PRIVATE
  replay_core
  GTest::gtest_main
)
gtest_discover_tests(test_market_data_loader
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)