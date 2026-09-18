#pragma once
#include <list>
#include <map>
#include <unordered_map>

#include "pool_allocator.hpp"

struct Order {
    uint64_t orderId;
    uint32_t price;
    uint32_t shares;
    char side;
    Order(uint64_t id, uint32_t p, uint32_t s, char sd)
        : orderId(id), price(p), shares(s), side(sd) {}
};

struct OrderLocation {
    uint32_t price;
    char side;
    std::list<Order*>::iterator listIt;
};

class OrderBook {
    std::map<uint32_t, std::list<Order*>, std::greater<uint32_t>> bids;
    std::map<uint32_t, std::list<Order*>> asks;
    std::unordered_map<uint64_t, OrderLocation> orderIndex;
    PoolAllocator<Order, 100000> pool;

   private:
    std::list<Order*>& getOrCreatePriceLevel(char side, uint32_t price) {
        if (side == 'B') {
            return bids[price];
        } else {
            return asks[price];
        }
    }

    void erasePriceLevelIfEmpty(char side, uint32_t price, const std::list<Order*>& priceLevel) {
        if (!priceLevel.empty()) return;
        if (side == 'B') {
            bids.erase(price);
        } else {
            asks.erase(price);
        }
    }

   public:
    uint32_t bestBid() {
        if (bids.empty()) return 0;
        return bids.begin()->first;
    }
    uint32_t bestAsk() {
        if (asks.empty()) return 0;
        return asks.begin()->first;
    }
    void addOrder(uint64_t orderId, char side, uint32_t price, uint32_t shares) {
        Order* order = pool.construct(orderId, price, shares, side);

        auto& priceLevel = getOrCreatePriceLevel(side, price);
        priceLevel.push_back(order);
        auto it = std::prev(priceLevel.end());

        orderIndex[orderId] = OrderLocation{price, side, it};
    }

    void cancelOrder(uint64_t orderId, uint32_t sharesToCancel) {
        auto idxIt = orderIndex.find(orderId);
        if (idxIt == orderIndex.end()) return;

        OrderLocation& loc = idxIt->second;
        auto& priceLevel = getOrCreatePriceLevel(loc.side, loc.price);

        Order* order = *loc.listIt;
        order->shares -= sharesToCancel;

        if (order->shares == 0) {
            priceLevel.erase(loc.listIt);
            orderIndex.erase(idxIt);
            pool.destroy(order);

            erasePriceLevelIfEmpty(loc.side, loc.price, priceLevel);
        }
    }

    void executeOrder(uint64_t orderId, uint32_t sharesExecuted) {
        auto idxIt = orderIndex.find(orderId);
        if (idxIt == orderIndex.end()) return;

        OrderLocation& loc = idxIt->second;
        auto& priceLevel = getOrCreatePriceLevel(loc.side,loc.price);

        Order* order = *loc.listIt;
        order->shares -= sharesExecuted;

        if (order->shares == 0) {
            priceLevel.erase(loc.listIt);
            orderIndex.erase(idxIt);
            pool.destroy(order);

            erasePriceLevelIfEmpty(loc.side, loc.price, priceLevel);
        }
    }

    void deleteOrder(uint64_t orderId) {
        auto idxIt = orderIndex.find(orderId);
        if (idxIt == orderIndex.end()) return;

        OrderLocation& loc = idxIt->second;
     
        auto& priceLevel = getOrCreatePriceLevel(loc.side,loc.price);

        Order* order = *loc.listIt;

        priceLevel.erase(loc.listIt);
        orderIndex.erase(idxIt);
        pool.destroy(order);

        erasePriceLevelIfEmpty(loc.side, loc.price, priceLevel);
    }
};