#include<catch2/catch_test_macros.hpp>
#include "order_book.hpp"


TEST_CASE("Add order updates best bid/ask") { 
    OrderBook book;
    book.addOrder(1, 'B', 100, 50);
    book.addOrder(2, 'S', 105, 30);


    REQUIRE(book.bestBid() == 100);
    REQUIRE(book.bestAsk() == 105);
}

TEST_CASE("Better price becomes new best bid") {
    OrderBook book;
    book.addOrder(1, 'B', 100, 50);
    book.addOrder(2, 'B', 102, 20);  

    REQUIRE(book.bestBid() == 102);
}

TEST_CASE("Cancel order removes it and updates book") {
    OrderBook book;
    book.addOrder(1, 'B', 100, 50);
    book.cancelOrder(1, 50);  

    REQUIRE(book.bestBid() == 0);
}

TEST_CASE("Delete order removes regardless of remaining shares") {
    OrderBook book;
    book.addOrder(1, 'B', 100, 50);
    book.deleteOrder(1);  

    REQUIRE(book.bestBid() == 0);
}
