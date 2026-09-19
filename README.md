# Limit Order Book Engine

A price-time priority limit order book supporting add/cancel/execute/delete
operations, built on a custom pool allocator and designed to ingest parsed
ITCH-style messages from the market data parser project.

## Design
- Bids and asks stored as sorted price levels (`std::map`, highest-bid-first /
  lowest-ask-first), each holding a FIFO queue (`std::list`) of resting orders
- O(1) order lookup and removal via an `orderId -> std::list iterator` index,
  relying on std::list's iterator-stability guarantee (insertion/removal
  elsewhere in the list never invalidates other iterators)
- Orders allocated via the custom pool allocator (see pool-allocator repo) —
  no per-order heap allocation
- bestBid()/bestAsk() are O(1): sorted map ordering means the best price is
  always the first entry, no scanning required

## Benchmark Results
Measured on 4-core CPU (2712 MHz), Release build.

| Benchmark                          | Result                    |
|--------------------------------------|-----------------------------|
| Best bid/ask query                  | 9.21 ns                    |
| Mixed workload (add/cancel/execute) | ~642 ns/op (~1.56M ops/sec) |

## Testing
10 assertions across 7 test cases: add/cancel/execute/delete correctness,
partial cancel and partial execute (shares reduced but order remains),
and FIFO ordering within a price level.

## Known limitations
- Not thread-safe (single-threaded design)
- Headers for the pool allocator and ITCH message types are copied from
  their respective repos rather than shared as a library
- No matching engine — this tracks resting order state only, doesn't
  simulate crossing the spread / generating fills between two orders