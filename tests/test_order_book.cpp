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

TEST_CASE("Partial cancel: shares reduced but order stays in the book") {
    OrderBook book;
    book.addOrder(1, 'B', 50, 100);
    book.cancelOrder(1, 30);

    REQUIRE(book.bestBid() == 50);
    REQUIRE(book.getOrderShares(1) == 70);
}

TEST_CASE("Partial execute: shares reduced but order stays in the book") {
    OrderBook book;
    book.addOrder(1, 'B', 50, 100);
    book.executeOrder(1, 30);

    REQUIRE(book.bestBid() == 50);
    REQUIRE(book.getOrderShares(1) == 70);
}

TEST_CASE("Orders at same price maintain FIFO order") {
    OrderBook book;
    book.addOrder(1, 'B', 50, 100);
    book.addOrder(2, 'B', 50, 100);
    book.addOrder(3, 'B', 50, 25);

    auto orderIds = book.getOrdersAtPrice('B', 50);
    std::vector<uint64_t> expected = {1, 2, 3};
    REQUIRE(std::equal(orderIds.begin(), orderIds.end(), expected.begin()));
}