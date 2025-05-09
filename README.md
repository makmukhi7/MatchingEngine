# MatchingEngine

## Quickstart
### Pre-requisites
While the production code doesn't have any third-party dependencies, we use [Bazel](https://bazel.build/) for building and [Googletests](https://github.com/google/googletest) for testing. 

### Build
Having installed Bazel, compilation can be achieved with the following command (run under the repository root directory):
```
$ bazel build --cxxopt=-std=c++17 //:main
```

The compiled binary can be found under `bazel-bin/main`.

### Run tests
To run tests use the following command:
```
$ bazel test --cxxopt=-std=c++17 --test_output=all //:all
```

### Running binary directly
Following is an example of running a compiled binary with test data:
```
$ mkfifo test_pipe
$ bazel-bin/main < test_pipe &
$ cat testdata/basic/input.txt > test_pip
```

## Design
A library to process trade orders sequentially and maintain an in-memory state of orders that haven't yet been fully matched with a counter party.

Follwoing are the core componenets:
* __Order Book:__ An in-memory data structure that keeps track of open orders (aka resting orders) and matches them against incoming orders.
* __Messages:__ Defines the messages and their format used to communicate with this library. Also has the parsing logic for incoming messages.
* __Matching Engine:__ Top level class that wraps reading and processing of order messages coming from an incoming stream.

### Data structures
To be able to match incoming orders quickly we want to keep the resting orders sorted, this leads us to using b-tree, `std::map`, for holding resting orders. We maintain two `std::map`s (one for buy side and one for sell side) and keep them sorted by price. Note that the sorting order of these maps is opposite of each other. Since it possible for more than one orders to have the same price, we maintain a doubly linked-list, `std::list`, on each node of the b-tree. This list simply keeps the resting orders in the same order they came in. The constant time insertion and deletion property of a doubly linked list is ideal for our use case.

> **_NOTE:_**  We could have used a `std::multimap` here and got roughly the same time complexities. For instance, insertion in a `std::multimap` at a specific node is amortized constant as opposed to the general insertion complexity of `O(log(n))`. This is similar to the constant time complexity for list insertions. We could explore this route by running microbenchmarks first. We leave that as a future exercise.

The b-tree approach enables constant time matching of incoming orders but the insertion and deletion time complexities, `O(log(n))`, can further be improved upon.

To improve deletion we keep a hash map,`std::unordered_map`, to index orders by their ids; `order_id -> pair(pointer to b-tree node, pointer to list node)`. So an incoming cancel order first looks up the order by its id in constant time then deletes this order from the list in constant time and if the list becomes empty the relevant b-tree node is deleted in amortized constant time. Note that the same approach (of deletion with pointer to the node) applies when an order is removed after being fulfilled. 

To improve insertions of unmatched, or partially filled, incoming orders we keep a hash map, `std::unordered_map`, to index order lists by their price; `price -> pointer to b-tree node`. When an order needs to be inserted, we first look up the price in this price index to see if an order list already exists, in such a case the insertion can happen in constant time (map lookup + list insertion). Otherwise, the insertion takes `log(n)` time dominated by the insertion complexity in b-tree.

Following are the complexties of these operations:
* Inserting a new order (partially matched or unmatched): If an order with same
price and same type (buy/sell) already exists in the book then O(1), otherwise
O(log(n)).

* Deletion (canceled or fulfilled): O(1).

* Matching: O(m), where m is the number of resting orders an incoming order
matches. So determining if there's at least one match is constant time
complexity.

### Parsing and contraints
The engine reads two types of messages on the input stream and expects the following formats:

```
1. AddOrderRequest: msgtype,orderid,side,quantity,price
	msgtype: 0
	orderid: unique positive integer to identify each order
	side: 0 (Buy), 1 (Sell)
	quantity: maximum quantity to buy/sell (positive integer)
	price: max price at which to buy/min price to sell (decimal number)
Example: (e.g., 0,123,0,9,1000)

2. CancelOrderRequest: msgtype,orderid
	msgtype: 1
	orderid: ID of the order to remove
Example: (e.g., 1,123)
```

## Testing
Individual components like order book and parsing logic have corrosponding unit tests. Additionally, end to end tests are added to test the complete flow using testing data sets.

##  Improvements for production
Following is a non-exhuastive list of further improvements to consider:
* Using a log library to improve debugging.
* Since the development of this project happened on a macbook, we weren't able to use `std::format` for floating point numbers (it isn't yet available in standard library, libc++, used by MacOs). We'd like to fix that by using roughly the same development environment as deployment (e.g. use Linux on dev machines).
* Run benchmarks: This is a first attempt implementation and almost certainly isn't the most optimal version that can be achieved. However, since performance tuning is involved and requires additional context we leave it as a future exercise. Following are some ideas to explore:
  *   `std::multimap<Price, Order>` vs `std::map<Price, std::list<Order>>`.
  *   Audit for extraneous memory copies.
  *   Benchmark running parsing and matching in separate threads.
