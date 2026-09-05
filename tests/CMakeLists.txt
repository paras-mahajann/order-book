include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(tests test_order_book.cpp)
target_link_libraries(tests PRIVATE order_book Catch2::Catch2WithMain)
add_test(NAME OrderBookTests COMMAND tests)