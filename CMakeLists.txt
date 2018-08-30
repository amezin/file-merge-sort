cmake_minimum_required(VERSION 3.1)
project(double_sort)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sort sort.cpp)
add_executable(gen gen.cpp)
add_executable(verify verify.cpp)

add_custom_command(
    OUTPUT test.big
    COMMAND gen test.big 1000000000
    DEPENDS gen
)
add_custom_command(
    OUTPUT test.small
    COMMAND gen test.small 100000
    DEPENDS gen
)
add_custom_target(inputs DEPENDS test.big test.small)

add_custom_command(
    OUTPUT test.big.sorted
    COMMAND sort test.big test.big.sorted
    DEPENDS sort test.big
)
add_custom_command(
    OUTPUT test.small.sorted
    COMMAND sort test.small test.small.sorted
    DEPENDS sort test.small
)

add_custom_target(test-small
    COMMAND verify test.small.sorted
    DEPENDS test.small.sorted verify
)
add_custom_target(test-big
    COMMAND verify test.big.sorted
    DEPENDS test.big.sorted verify
)
