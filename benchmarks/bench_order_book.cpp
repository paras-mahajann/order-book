#include <benchmark/benchmark.h>

#include <random>
#include <vector>

#include "order_book.hpp"

static void BM_OrderBookMixedWorkload(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        OrderBook book;
        std::vector<uint64_t> activeOrders;
        uint64_t nextOrderId = 1;
        std::mt19937 rng(42);  // fixed seed for reproducibility
        std::uniform_int_distribution<uint32_t> priceDist(9900, 10100);
        std::uniform_int_distribution<uint32_t> sizeDist(10, 500);
        state.ResumeTiming();

        constexpr int kNumOps = 10000;
        for (int i = 0; i < kNumOps; i++) {
            int op = i % 10;  // rough mix: mostly adds, some cancels/executes
            if (op < 7 || activeOrders.empty()) {
                // Add
                char side = (i % 2 == 0) ? 'B' : 'S';
                book.addOrder(nextOrderId, side, priceDist(rng), sizeDist(rng));
                activeOrders.push_back(nextOrderId);
                nextOrderId++;
            } else if (op < 9) {
                // Cancel a random active order
                uint64_t id = activeOrders[rng() % activeOrders.size()];
                book.cancelOrder(id, sizeDist(rng));
            } else {
                // Execute a random active order
                uint64_t id = activeOrders[rng() % activeOrders.size()];
                book.executeOrder(id, sizeDist(rng));
            }
        }
        benchmark::DoNotOptimize(book.bestBid());
    }
}
BENCHMARK(BM_OrderBookMixedWorkload);

// Isolated: how fast is a single bestBid()/bestAsk() query once the book is populated?
static void BM_BestBidAskQuery(benchmark::State& state) {
    OrderBook book;
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint32_t> priceDist(9900, 10100);
    for (uint64_t i = 1; i <= 1000; i++) {
        book.addOrder(i, (i % 2 == 0) ? 'B' : 'S', priceDist(rng), 100);
    }

    for (auto _ : state) {
        auto bid = book.bestBid();
        auto ask = book.bestAsk();
        benchmark::DoNotOptimize(bid);
        benchmark::DoNotOptimize(ask);
    }
}
BENCHMARK(BM_BestBidAskQuery);

BENCHMARK_MAIN();