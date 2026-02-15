# C++ Theory Guide for Quant Backtesting Framework

This guide explains all the C++ concepts used in this project with concrete examples from the codebase, design decisions, and their trade-offs.

---

## Table of Contents
1. [Classes and Objects](#1-classes-and-objects)
2. [Constructors and Member Initialization](#2-constructors-and-member-initialization)
3. [Access Modifiers (public, private, protected)](#3-access-modifiers)
4. [Const Correctness](#4-const-correctness)
5. [References vs Pointers](#5-references-vs-pointers)
6. [Namespaces](#6-namespaces)
7. [Inheritance and Abstract Classes](#7-inheritance-and-abstract-classes)
8. [STL Containers (map, deque, vector)](#8-stl-containers)
9. [Iterators](#9-iterators)
10. [Auto and Type Deduction](#10-auto-and-type-deduction)
11. [Structured Bindings (C++17)](#11-structured-bindings-c17)
12. [Header Files vs Implementation Files](#12-header-files-vs-implementation-files)
13. [Memory Management and RAII](#13-memory-management-and-raii)
14. [Design Patterns Used](#14-design-patterns-used)

---

## 1. Classes and Objects

### Theory
A **class** is a blueprint for creating objects. It bundles data (member variables) and functions (member methods) that operate on that data. This is called **encapsulation** - one of the pillars of Object-Oriented Programming.

### Example from Your Project: `Position` Class

```cpp
class Position {
public:
    // Constructor
    Position(const std::string& symbol, int quantity, double entry_price);
    
    // Methods
    void update_price(double current_price);
    double market_value() const;
    
private:
    // Member variables (data)
    std::string symbol_;
    int quantity_;
    double entry_price_;
    double current_price_;
};
```

**What this represents:**
- A `Position` object represents a trading position in a single security
- Each position tracks: symbol name, quantity (positive for long, negative for short), entry price, and current market price
- Methods like `market_value()` operate on this data

**Creating an object:**
```cpp
// Create a position: Buy 100 shares of AAPL at $150
Position apple_position("AAPL", 100, 150.0);

// Use the object
double value = apple_position.market_value();  // Returns 15000.0
```

### Design Decision: Why a Class?
**Pros:**
- Encapsulation: All position logic is in one place
- Data integrity: Private members can't be changed arbitrarily
- Reusability: Can create many Position objects
- Clear interface: Public methods define what users can do

**Cons:**
- More code than a simple struct
- Slight performance overhead (usually negligible)

**Alternative Considered:**
Could use a simple struct with free functions, but that doesn't protect data integrity or group related functionality.

---

## 2. Constructors and Member Initialization

### Theory
A **constructor** is a special method called when an object is created. It initializes the object's member variables. The **member initializer list** (`: member_(value)`) is preferred over assignment in the constructor body.

### Example from Your Project: `Position` Constructor

```cpp
Position::Position(const std::string& symbol, int quantity, double entry_price)
    : symbol_(symbol)           // Initialize symbol_ with symbol parameter
    , quantity_(quantity)       // Initialize quantity_ with quantity parameter
    , entry_price_(entry_price) // Initialize entry_price_
    , current_price_(entry_price) {  // Initialize current_price_ to entry_price
    // Constructor body (empty because everything is initialized above)
}
```

**What happens:**
1. Memory is allocated for the object
2. Each member variable is initialized using the initializer list
3. Constructor body executes (optional additional logic)

### Why Member Initializer List?

**Initializer List (Preferred):**
```cpp
Position::Position(const std::string& symbol, ...)
    : symbol_(symbol)  // Direct initialization
```

**Body Assignment (Avoid):**
```cpp
Position::Position(const std::string& symbol, ...) {
    symbol_ = symbol;  // Default construction + assignment
}
```

**Pros of Initializer List:**
- **Performance**: One operation instead of two (no default construction)
- **Const members**: Can initialize const members (can't assign in body)
- **References**: Can initialize reference members
- **Initialization order**: Matches declaration order in class

**Cons:**
- Slightly more complex syntax

### Example from Portfolio Constructor

```cpp
Portfolio::Portfolio(double initial_capital)
    : initial_capital_(initial_capital)
    , cash_(initial_capital)      // Can use other members
    , realized_pnl_(0.0) {        // Initialize to 0
}
```

**Design Decision**: We initialize `cash_` to `initial_capital_` and `realized_pnl_` to 0, establishing the starting state clearly.

---

## 3. Access Modifiers

### Theory
Access modifiers control who can access class members:
- **public**: Anyone can access
- **private**: Only class methods can access
- **protected**: Class methods and derived classes can access

### Example from Your Project: `Position` Class

```cpp
class Position {
public:
    // Interface - what users can do
    Position(const std::string& symbol, int quantity, double entry_price);
    void update_price(double current_price);
    double market_value() const;
    
    // Getters - controlled read access
    const std::string& symbol() const { return symbol_; }
    int quantity() const { return quantity_; }
    double entry_price() const { return entry_price_; }
    
private:
    // Implementation details - hidden from users
    std::string symbol_;
    int quantity_;
    double entry_price_;
    double current_price_;
};
```

### Why This Design?

**Public Methods:**
- Define what operations are allowed
- Can validate inputs and maintain invariants
- Can change implementation without breaking user code

**Private Members:**
- Users can't accidentally corrupt data
- Example: Can't set `quantity_` to invalid value
- Can't bypass `adjust_quantity()` logic

**Example of Protection:**
```cpp
// WRONG - If members were public
position.quantity_ = -999;  // Bypasses validation!
position.entry_price_ = 0;  // Invalid state!

// CORRECT - With private members
position.adjust_quantity(-50, 100.0);  // Goes through validation
```

**Getters (Accessors):**
```cpp
const std::string& symbol() const { return symbol_; }
```
- Provides read-only access
- Returns const reference (efficient, can't modify)
- No setter for symbol - it never changes after construction

### Design Decision: No Setters for Critical Data
**Why no `set_quantity()`?**
- Changing quantity requires recalculating entry price
- Must go through `adjust_quantity()` to maintain consistency
- Prevents bugs from direct manipulation

---

## 4. Const Correctness

### Theory
**const** is a contract: "I promise not to modify this." It enables compiler optimization and prevents bugs.

### Types of Const Usage

#### 4.1 Const Method (Most Important)

```cpp
double Position::market_value() const {
    return quantity_ * current_price_;
}
```

**What `const` after method name means:**
- "This method will NOT modify any member variables"
- Can be called on const objects
- Compiler enforces this - trying to modify members causes error

**Example from Portfolio:**
```cpp
double Portfolio::total_value() const {
    double total = cash_;  // OK - just reading
    for (const auto& [symbol, pos] : positions_) {
        total += pos.market_value();  // OK - market_value() is const
    }
    return total;
}
```

**Why this matters:**
```cpp
void print_portfolio_info(const Portfolio& portfolio) {
    // portfolio is const - can only call const methods
    std::cout << portfolio.total_value();  // OK - total_value() is const
    // portfolio.open_position(...);  // ERROR - not const method
}
```

#### 4.2 Const Parameters

**By const reference (most common):**
```cpp
void Portfolio::open_position(const std::string& symbol, ...) {
    //                        ^^^^^
    // Can read symbol, but can't modify it
    // Efficient - no copy made
}
```

**Why const reference?**
- **Efficiency**: No copy of string made (strings can be large)
- **Safety**: Can't accidentally modify the parameter
- **Intent**: Clearly states "input only"

**Alternative Designs:**
```cpp
// Option 1: Pass by value (WASTEFUL for strings)
void open_position(std::string symbol, ...) {
    // Makes a copy of the string - slow for large strings
}

// Option 2: Pass by non-const reference (CONFUSING)
void open_position(std::string& symbol, ...) {
    // Implies the function might modify symbol - misleading
}

// Option 3: Pass by const reference (BEST)
void open_position(const std::string& symbol, ...) {
    // Fast + safe + clear intent
}
```

#### 4.3 Const Return Values

```cpp
const Position* Portfolio::get_position(const std::string& symbol) const {
    //             ^^^^^^^                                          ^^^^^
    // Returns pointer to const Position               Method doesn't modify
}
```

**What this means:**
```cpp
const Position* pos = portfolio.get_position("AAPL");
if (pos) {
    double value = pos->market_value();  // OK - const method
    // pos->update_price(150.0);  // ERROR - would modify const object
}
```

### Design Decision: When to Use Const

**Rule of thumb:**
1. **Methods that don't modify**: Make them const
2. **Parameters that are read-only**: Pass by const reference
3. **Small types** (int, double): Pass by value (copying is cheap)
4. **Large types** (string, containers): Pass by const reference

**Example from your code:**
```cpp
// Small types - pass by value
void update_price(double current_price);  // double is cheap to copy

// Large types - pass by const reference  
void open_position(const std::string& symbol, ...);  // string is expensive
void update_prices(const Bar& bar);  // Bar has multiple fields
```

---

## 5. References vs Pointers

### Theory
Both references and pointers allow you to refer to an object without copying it. But they have different semantics and use cases.

### 5.1 References

**Syntax:**
```cpp
Position& ref = position;  // Reference
ref.update_price(100.0);   // Use like normal variable
```

**Characteristics:**
- Must be initialized when declared
- Cannot be null
- Cannot be reseated (always refers to same object)
- Cleaner syntax (no `->` or `*`)

**Example from Your Code: `Portfolio::close_position()`**
```cpp
void Portfolio::close_position(const std::string& symbol, ...) {
    // Get reference to position
    Position& pos = positions_.at(symbol);
    
    // Use it like a normal variable
    double pnl = (price - pos.entry_price()) * quantity;
    realized_pnl_ += pnl;
    
    // Modify through reference
    pos.adjust_quantity(-quantity, price);
    
    // Changes affect the original position in the map
}
```

**Why reference here?**
- We know the position exists (checked with `has_position()`)
- We want to modify the position in the map
- Cleaner syntax than pointer

### 5.2 Pointers

**Syntax:**
```cpp
Position* ptr = &position;  // Pointer (address-of)
ptr->update_price(100.0);   // Use with -> operator
(*ptr).update_price(100.0); // Or dereference first
```

**Characteristics:**
- Can be null (represent "no object")
- Can be reseated (point to different objects)
- Requires `->` to access members
- Can do pointer arithmetic (advanced)

**Example from Your Code: `Portfolio::get_position()`**
```cpp
const Position* Portfolio::get_position(const std::string& symbol) const {
    auto it = positions_.find(symbol);
    return (it != positions_.end()) ? &it->second : nullptr;
    //                                               ^^^^^^^
    //                                               Can return null!
}
```

**Why pointer here?**
- Position might not exist - need to represent "not found"
- Caller must check for nullptr:

```cpp
const Position* pos = portfolio.get_position("AAPL");
if (pos) {  // Check for null
    std::cout << pos->market_value();
} else {
    std::cout << "No position in AAPL";
}
```

### Design Decision: Reference vs Pointer

| Use Reference When: | Use Pointer When: |
|-------------------|------------------|
| ✅ Object must exist | ✅ Object might not exist (nullable) |
| ✅ Won't change what it refers to | ✅ Might point to different objects |
| ✅ Prefer cleaner syntax | ✅ Need to return "not found" |
| Example: `Position& pos = positions_.at(symbol)` | Example: `Position* find_position()` |

### 5.3 Const References (Most Common in Your Code)

```cpp
void Portfolio::update_prices(const Bar& bar) {
    //                         ^^^^^^^^^^^^^^
    // Reference: No copy
    // Const: Can't modify
}
```

**Benefits:**
- **Efficiency**: No copy of Bar object
- **Safety**: Can't accidentally modify the Bar
- **Flexibility**: Works with temporaries

**Example usage:**
```cpp
portfolio.update_prices(data_handler.next_bar());
//                      ^^^^^^^^^^^^^^^^^^^^^^^^
//                      Temporary Bar object - only works with const ref!
```

---

## 6. Namespaces

### Theory
Namespaces prevent naming conflicts by grouping related code under a unique name.

### Example from Your Project

**Declaration in header:**
```cpp
// position.hpp
namespace backtest {
    
    class Position {
        // ...
    };
    
} // namespace backtest
```

**Implementation:**
```cpp
// position.cpp
namespace backtest {

Position::Position(const std::string& symbol, int quantity, double entry_price)
    : symbol_(symbol), quantity_(quantity), entry_price_(entry_price) {
}

} // namespace backtest
```

**Usage:**
```cpp
// Option 1: Full qualification
backtest::Position pos("AAPL", 100, 150.0);

// Option 2: Using declaration (in .cpp file)
using backtest::Position;
Position pos("AAPL", 100, 150.0);

// Option 3: Using directive (be careful!)
using namespace backtest;
Position pos("AAPL", 100, 150.0);
```

### Why Use Namespaces?

**Problem without namespace:**
```cpp
// Your code
class Position { ... };

// Some library you include
class Position { ... };

// ERROR: Which Position?!
```

**Solution with namespace:**
```cpp
namespace backtest {
    class Position { ... };
}

namespace trading_library {
    class Position { ... };
}

// Clear!
backtest::Position my_pos;
trading_library::Position their_pos;
```

### Design Decision: `backtest` Namespace

**Pros:**
- Prevents conflicts with other libraries
- Clear that it's part of backtest project
- Professional/organized code structure

**Cons:**
- Slightly more verbose
- Must remember to use namespace

**Best Practice:**
- Use namespace in headers (`.hpp`)
- Can use `using` declarations in `.cpp` files for convenience
- Avoid `using namespace` in headers (pollutes user's code)

---

## 7. Inheritance and Abstract Classes

### Theory
**Inheritance** allows a class to derive from another, inheriting its properties. **Abstract classes** define interfaces that derived classes must implement.

### Example from Your Project: `StrategyBase`

**Abstract Base Class:**
```cpp
// strategy_base.hpp
class StrategyBase {
public:
    virtual ~StrategyBase() = default;  // Virtual destructor
    
    // Pure virtual functions (= 0 means "must override")
    virtual void on_bar(const Bar& bar) = 0;
    virtual bool should_enter() const = 0;
    virtual bool should_exit() const = 0;
    
protected:
    // Shared data accessible to derived classes
    std::deque<double> prices_;
};
```

**Concrete Derived Class:**
```cpp
// sma_strategy.hpp
class SMAStrategy : public StrategyBase {
    //                ^^^^^^ Inheritance
public:
    SMAStrategy(size_t short_window, size_t long_window);
    
    // Override pure virtual functions
    void on_bar(const Bar& bar) override;
    bool should_enter() const override;
    bool should_exit() const override;
    
private:
    // SMA-specific data
    size_t short_window_;
    size_t long_window_;
    double short_sma_;
    double long_sma_;
};
```

### Why This Design?

**Polymorphism in action:**
```cpp
// Can use base class pointer with any derived strategy
StrategyBase* strategy = new SMAStrategy(20, 50);

// Call overridden methods
Bar bar = data_handler.next_bar();
strategy->on_bar(bar);  // Calls SMAStrategy::on_bar()

if (strategy->should_enter()) {
    // SMAStrategy logic executed
}

delete strategy;
```

### Key Concepts Explained

#### 7.1 Virtual Functions

```cpp
virtual void on_bar(const Bar& bar) = 0;
//^^^^^^                             ^^^^
//Virtual                            Pure virtual
```

- **virtual**: Can be overridden by derived class
- **= 0**: Pure virtual - MUST be overridden (makes class abstract)

#### 7.2 Virtual Destructor

```cpp
virtual ~StrategyBase() = default;
//^^^^^^
```

**Why virtual destructor is critical:**
```cpp
StrategyBase* strategy = new SMAStrategy(20, 50);
delete strategy;
//     ^^^^^^^^
// Without virtual destructor:
//   Only StrategyBase destructor called - MEMORY LEAK!
// With virtual destructor:
//   SMAStrategy destructor called, then StrategyBase - CORRECT!
```

**Rule:** Always make destructor virtual in base classes.

#### 7.3 Protected Members

```cpp
protected:
    std::deque<double> prices_;
```

- Accessible in `StrategyBase`
- Accessible in `SMAStrategy`  
- NOT accessible outside class hierarchy

**Why protected?**
- Derived classes need access to shared data
- Still hidden from outside users
- Encapsulation maintained

### Design Decision: Strategy Pattern

**Pros:**
- **Extensibility**: Easy to add new strategies (just inherit and override)
- **Polymorphism**: Can store different strategies in same container
- **Code reuse**: Common functionality in base class
- **Clear interface**: Base class defines contract

**Cons:**
- Virtual function calls have tiny performance overhead
- More complex than simple functions
- Inheritance can be misused (favor composition when appropriate)

**Alternative Considered:**
```cpp
// Function pointer approach (less flexible)
typedef void (*StrategyFunc)(const Bar&);

// Your approach is better for:
// - Complex strategies with state
// - Sharing common code
// - Type safety
```

---

## 8. STL Containers

### Theory
The Standard Template Library (STL) provides ready-made container classes. Each has different performance characteristics.

### 8.1 std::map

**From your `Portfolio` class:**
```cpp
class Portfolio {
private:
    std::map<std::string, Position> positions_;
    //       ^^^^^^^^^^^^^^^^^^^^^^
    //       Key: symbol  Value: Position object
};
```

**What is std::map?**
- **Associative container**: Stores key-value pairs
- **Ordered**: Keys sorted automatically
- **Unique keys**: Each key appears once
- **Implementation**: Red-black tree (balanced binary search tree)

**Operations & Performance:**
```cpp
// Insert/create - O(log n)
positions_["AAPL"] = Position("AAPL", 100, 150.0);
positions_.emplace("MSFT", Position("MSFT", 50, 300.0));

// Lookup - O(log n)
if (positions_.find("AAPL") != positions_.end()) {
    // Found
}

// Access - O(log n)
Position& pos = positions_.at("AAPL");  // Throws if not found
Position& pos2 = positions_["AAPL"];    // Creates if not found!

// Iterate - O(n)
for (auto& [symbol, pos] : positions_) {
    // Sorted by symbol alphabetically
}

// Remove - O(log n)
positions_.erase("AAPL");
```

**Why map for positions?**

**Pros:**
- Fast lookup by symbol name: O(log n)
- Automatic ordering (nice for reports)
- Keys can't duplicate (one position per symbol)
- Clear semantics: `positions_["AAPL"]`

**Cons:**
- Slower than unordered_map for pure lookup
- More memory overhead than vector

**Alternative: std::unordered_map**
```cpp
std::unordered_map<std::string, Position> positions_;
// Faster lookup: O(1) average
// But: No ordering, more memory
```

**Design Decision:** We chose `map` because:
1. Position count is small (rarely > 100)
2. Ordering is nice for display
3. O(log n) is fast enough for this use case

### 8.2 std::deque

**From your `StrategyBase`:**
```cpp
class StrategyBase {
protected:
    std::deque<double> prices_;
};
```

**What is std::deque?**
- **Double-ended queue**: Fast insertion/removal at both ends
- **Random access**: Can access any element by index
- **Implementation**: Array of arrays (chunked)

**Operations & Performance:**
```cpp
// Add to back - O(1)
prices_.push_back(100.5);

// Add to front - O(1)
prices_.push_front(99.5);

// Remove from back - O(1)
prices_.pop_back();

// Remove from front - O(1)
prices_.pop_front();

// Access by index - O(1)
double price = prices_[0];

// Size - O(1)
size_t count = prices_.size();
```

**Why deque for price history?**

**SMA Strategy example:**
```cpp
void SMAStrategy::on_bar(const Bar& bar) {
    prices_.push_back(bar.close);
    
    // Keep only what we need
    if (prices_.size() > long_window_) {
        prices_.pop_front();  // Remove oldest - O(1)!
    }
}
```

**Alternatives:**

| Container | Push Back | Pop Front | Access | Use Case |
|-----------|-----------|-----------|--------|----------|
| **std::deque** | O(1) | O(1) ✅ | O(1) | Rolling window (your choice) |
| **std::vector** | O(1) | O(n) ❌ | O(1) | When you don't remove from front |
| **std::list** | O(1) | O(1) | O(n) ❌ | When you need middle insertion |

**Design Decision:** `deque` is perfect for rolling windows because:
- Add new prices to back: O(1)
- Remove old prices from front: O(1)
- Still have random access for calculations

### 8.3 Comparison Table

| Operation | std::vector | std::deque | std::list | std::map |
|-----------|------------|-----------|-----------|----------|
| Access by index | O(1) | O(1) | O(n) | N/A |
| Insert/remove at end | O(1) | O(1) | O(1) | N/A |
| Insert/remove at start | O(n) | O(1) | O(1) | N/A |
| Insert/remove middle | O(n) | O(n) | O(1) | N/A |
| Find by key | O(n) | O(n) | O(n) | O(log n) |
| Memory | Contiguous | Chunked | Scattered | Tree nodes |
| Iterator invalidation | Yes | Sometimes | No | No |

---

## 9. Iterators

### Theory
Iterators are objects that point to elements in containers. They act like "smart pointers" that know how to traverse the container.

### 9.1 Basic Iterator Usage

**From your `Portfolio::total_value()`:**
```cpp
double Portfolio::total_value() const {
    auto it = positions_.begin();
    //   ^^
    // Iterator pointing to first element
    
    double total = cash_;
    
    for (; it != positions_.end(); ++it) {
    //     ^^^^^^^^^^^^^^^^^^^     ^^^^
    //     Not at end yet          Move to next
        
        total += it->second.market_value();
        //       ^^^^^^^^^^
        //       Arrow operator: access member of pointed-to object
    }
    
    return total;
}
```

### Iterator Concepts

**What is an iterator?**
```cpp
std::map<std::string, Position>::iterator it;
//                                ^^^^^^^^
//                                Iterator type
```

An iterator is like a pointer with extra capabilities:
- `it->first`: Access the key (string)
- `it->second`: Access the value (Position)
- `++it`: Move to next element
- `it != container.end()`: Check if valid

### 9.2 Iterator for std::map

**std::map iterator gives you a pair:**
```cpp
for (auto it = positions_.begin(); it != positions_.end(); ++it) {
    std::string symbol = it->first;    // Key
    Position& pos = it->second;        // Value
    
    std::cout << symbol << ": " << pos.market_value() << "\n";
}
```

**Why `it->second`?**
- `it` points to a `std::pair<const std::string, Position>`
- `.first` is the key (symbol)
- `.second` is the value (Position)

### 9.3 Range-Based For Loop (Modern Alternative)

**Old style (explicit iterator):**
```cpp
for (auto it = positions_.begin(); it != positions_.end(); ++it) {
    total += it->second.market_value();
}
```

**Modern style (range-based for):**
```cpp
for (const auto& pair : positions_) {
    total += pair.second.market_value();
}
```

**Even better with structured bindings (C++17):**
```cpp
for (const auto& [symbol, pos] : positions_) {
    //            ^^^^^^^^^^^^^^
    // Unpacks pair into symbol and pos
    total += pos.market_value();
}
```

### 9.4 Your Code Evolution

**Example: `Portfolio::unrealized_pnl()`**

**Version 1 (explicit iterator - you wrote this):**
```cpp
double Portfolio::unrealized_pnl() const {
    double pnl = 0.0;
    for (auto it = positions_.begin(); it != positions_.end(); ++it) {
        pnl += it->second.unrealized_pnl();
    }
    return pnl;
}
```

**Version 2 (range-based for):**
```cpp
double Portfolio::unrealized_pnl() const {
    double pnl = 0.0;
    for (const auto& pair : positions_) {
        pnl += pair.second.unrealized_pnl();
    }
    return pnl;
}
```

**Version 3 (structured bindings - you used this!):**
```cpp
double Portfolio::unrealized_pnl() const {
    double pnl = 0.0;
    for (const auto& [symbol, pos] : positions_) {
        pnl += pos.unrealized_pnl();
    }
    return pnl;
}
```

**All three are equivalent!** Version 3 is most readable.

### 9.5 Const Iterators

```cpp
double Portfolio::total_value() const {
//                              ^^^^^
// Const method
    
    for (auto it = positions_.begin(); it != positions_.end(); ++it) {
    //        ^^
    // Automatically becomes const_iterator
        
        // Can read but not modify
        double val = it->second.market_value();  // OK
        // it->second.update_price(100.0);  // ERROR - const iterator
    }
}
```

**const_iterator vs iterator:**
- `iterator`: Can modify elements
- `const_iterator`: Read-only access
- Automatically chosen based on whether method is const

### Design Decision: When to Use Each Style

| Style | When to Use |
|-------|-------------|
| **Explicit iterator** | Need to erase elements during loop, complex iteration logic |
| **Range-based for** | Simple iteration over all elements (most common) |
| **Structured bindings** | Working with pairs/tuples, improves readability |

**Example: Can't use range-based for when erasing:**
```cpp
// WRONG - iterator invalidation
for (const auto& [symbol, pos] : positions_) {
    if (pos.is_flat()) {
        positions_.erase(symbol);  // Invalidates iterator - crash!
    }
}

// CORRECT - explicit iterator
for (auto it = positions_.begin(); it != positions_.end(); ) {
    if (it->second.is_flat()) {
        it = positions_.erase(it);  // erase returns next valid iterator
    } else {
        ++it;
    }
}
```

---

## 10. Auto and Type Deduction

### Theory
`auto` lets the compiler deduce the type automatically, reducing verbosity and making code more maintainable.

### Example from Your Code

**Without auto (verbose):**
```cpp
std::map<std::string, Position>::iterator it = positions_.begin();
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Type explicitly written
```

**With auto (concise):**
```cpp
auto it = positions_.begin();
//^^
// Compiler figures out the type
```

### More Examples from Your Project

#### 10.1 Iterator Type

```cpp
// Verbose
std::map<std::string, Position>::const_iterator it = positions_.find("AAPL");

// Concise
auto it = positions_.find("AAPL");
```

**Type deduced:** `std::map<std::string, Position>::const_iterator`

#### 10.2 Complex Return Types

```cpp
const Position* Portfolio::get_position(const std::string& symbol) const {
    auto it = positions_.find(symbol);
    //^^
    // Type: std::map<std::string, Position>::const_iterator
    
    return (it != positions_.end()) ? &it->second : nullptr;
}
```

### When to Use Auto

**Good uses:**

```cpp
// 1. Iterator types (always!)
auto it = positions_.begin();

// 2. Complex template types
auto strategy = std::make_unique<SMAStrategy>(20, 50);

// 3. Lambda functions
auto calculate = [](double a, double b) { return a + b; };

// 4. Range-based for loops
for (const auto& [symbol, pos] : positions_) {
    // Type: const std::pair<const std::string, Position>&
}
```

**Bad uses (type not obvious):**

```cpp
// BAD - what type is this?
auto x = 5.0;  // double? float? Could be unclear

// BETTER - explicit when simple
double price = 5.0;

// BAD - return type not obvious
auto process(const std::string& s);  // What does it return?

// BETTER - explicit return type
double process(const std::string& s);
```

### auto vs auto&

```cpp
// By value (copy)
for (auto pair : positions_) {
    // pair is a COPY of each element
}

// By reference (no copy)
for (auto& pair : positions_) {
    // pair REFERS to each element
    pair.second.update_price(100.0);  // Modifies original
}

// By const reference (no copy, can't modify)
for (const auto& pair : positions_) {
    // pair REFERS to each element, but can't modify
    double val = pair.second.market_value();  // OK
    // pair.second.update_price(100.0);  // ERROR
}
```

### Design Decision: Your Usage is Perfect

**Your code:**
```cpp
for (const auto& [symbol, pos] : positions_) {
//   ^^^^^^^^^^^^^
//   const + auto + reference
    pnl += pos.unrealized_pnl();
}
```

**Why this is optimal:**
- `auto`: Compiler figures out complex pair type
- `&`: No copy (efficient)
- `const`: Can't accidentally modify (safe)
- `[symbol, pos]`: Clear names (readable)

---

## 11. Structured Bindings (C++17)

### Theory
Structured bindings let you unpack tuples, pairs, and structs into individual variables with clear names.

### Example from Your Project

**Without structured bindings:**
```cpp
for (const auto& pair : positions_) {
    const std::string& symbol = pair.first;
    const Position& pos = pair.second;
    
    std::cout << symbol << ": " << pos.market_value() << "\n";
}
```

**With structured bindings (C++17):**
```cpp
for (const auto& [symbol, pos] : positions_) {
    //            ^^^^^^^^^^^^^^
    // Directly unpack pair into symbol and pos
    
    std::cout << symbol << ": " << pos.market_value() << "\n";
}
```

### How It Works

**std::map iterator returns std::pair:**
```cpp
std::pair<const std::string, Position>
//        ^^^^^^^^^^^^^^^^^^^^^^^^^^^
//        .first              .second
```

**Structured binding unpacks it:**
```cpp
auto& [symbol, pos] = pair;
//    ^^^^^^^^^^^^^
//    symbol = pair.first
//    pos = pair.second
```

### Your Usage in Portfolio

**From `Portfolio::unrealized_pnl()`:**
```cpp
double Portfolio::unrealized_pnl() const {
    double pnl = 0.0;
    for (const auto& [symbol, pos] : positions_) {
        //            ^^^^^^^^^^^^^^
        // Much clearer than it->first and it->second!
        pnl += pos.unrealized_pnl();
    }
    return pnl;
}
```

### Why This is Better

**Comparison:**

```cpp
// Style 1: Explicit iterator (C++98)
for (auto it = positions_.begin(); it != positions_.end(); ++it) {
    pnl += it->second.unrealized_pnl();
    //     ^^^^^^^^^^ What is "second"?
}

// Style 2: Range-based for (C++11)
for (const auto& pair : positions_) {
    pnl += pair.second.unrealized_pnl();
    //     ^^^^^^^^^^^ Still unclear
}

// Style 3: Structured bindings (C++17) ✅
for (const auto& [symbol, pos] : positions_) {
    pnl += pos.unrealized_pnl();
    //     ^^^ Crystal clear!
}
```

### More Examples

**Unpacking structs:**
```cpp
struct Point {
    double x, y;
};

Point p = {3.0, 4.0};
auto [x, y] = p;
//    ^^^^
// x = 3.0, y = 4.0
```

**Unpacking arrays:**
```cpp
std::array<int, 3> arr = {1, 2, 3};
auto [a, b, c] = arr;
// a=1, b=2, c=3
```

**Unpacking function returns:**
```cpp
std::pair<bool, double> calculate() {
    return {true, 42.0};
}

auto [success, result] = calculate();
if (success) {
    std::cout << result;
}
```

### Design Decision: Modern C++ Best Practice

**Why we use structured bindings:**
- **Readability**: `symbol` and `pos` are much clearer than `first` and `second`
- **Type safety**: Compiler still checks types
- **Efficiency**: No runtime overhead (just names for existing data)
- **Modern**: Shows you're using C++17 features

**When to use:**
- Iterating over maps (always!)
- Unpacking pairs/tuples
- Multiple return values
- Struct decomposition

---

## 12. Header Files vs Implementation Files

### Theory
C++ splits code into **declarations** (`.hpp` headers) and **definitions** (`.cpp` implementation). This enables separate compilation and faster builds.

### Your Project Structure

```
src/
  portfolio/
    position.hpp      # Class declaration
    position.cpp      # Class implementation
    portfolio.hpp     # Class declaration
    portfolio.cpp     # Class implementation
```

### 12.1 Header Files (.hpp)

**Example: `position.hpp`**
```cpp
#pragma once  // Include guard (modern style)

#include <string>

namespace backtest {

class Position {
public:
    // Declaration only - no implementation
    Position(const std::string& symbol, int quantity, double entry_price);
    void update_price(double current_price);
    double market_value() const;
    
    // Inline implementation (small functions)
    const std::string& symbol() const { return symbol_; }
    bool is_flat() const { return quantity_ == 0; }
    
private:
    std::string symbol_;
    int quantity_;
    double entry_price_;
    double current_price_;
};

} // namespace backtest
```

**What goes in headers:**
- Class declarations
- Function declarations
- Inline function implementations
- Template implementations (must be in header)
- Constants, enums, typedefs

### 12.2 Implementation Files (.cpp)

**Example: `position.cpp`**
```cpp
#include "position.hpp"  // Include own header
#include <cmath>         // For std::abs

namespace backtest {

// Implementation of declared functions
Position::Position(const std::string& symbol, int quantity, double entry_price)
    : symbol_(symbol)
    , quantity_(quantity)
    , entry_price_(entry_price)
    , current_price_(entry_price) {
}

void Position::update_price(double current_price) {
    current_price_ = current_price;
}

double Position::market_value() const {
    return quantity_ * current_price_;
}

// ... more implementations

} // namespace backtest
```

**What goes in implementation files:**
- Function definitions
- Non-inline member functions
- Static member variable definitions

### 12.3 Include Guards

**Old style (still works):**
```cpp
// position.hpp
#ifndef POSITION_HPP
#define POSITION_HPP

// ... code ...

#endif // POSITION_HPP
```

**Modern style (your code uses this):**
```cpp
// position.hpp
#pragma once

// ... code ...
```

**Why include guards?**
```cpp
// Without guards:
#include "position.hpp"
#include "portfolio.hpp"  // Also includes position.hpp
// ERROR: Position class defined twice!

// With guards:
#include "position.hpp"
#include "portfolio.hpp"  // position.hpp included again, but guard prevents redefinition
// OK: Position class defined once
```

### 12.4 Compilation Model

**How it works:**
```
1. Compile position.cpp → position.o
2. Compile portfolio.cpp → portfolio.o
3. Compile main.cpp → main.o
4. Link all .o files → backtest executable
```

**Why separate compilation?**
- **Faster builds**: Only recompile changed files
- **Encapsulation**: Users don't see implementation details
- **Library distribution**: Can share .hpp + .o (compiled library)

**Example:**
```bash
# Change position.cpp
# Only need to recompile:
g++ -c position.cpp -o position.o

# If you changed position.hpp
# Must recompile everything that includes it:
g++ -c position.cpp -o position.o
g++ -c portfolio.cpp -o portfolio.o
g++ -c main.cpp -o main.o
```

### 12.5 Forward Declarations

**Reduce compilation dependencies:**
```cpp
// portfolio.hpp - without forward declaration
#include "position.hpp"  // Must include full definition

class Portfolio {
private:
    std::map<std::string, Position> positions_;  // Needs complete type
};
```

**With forward declaration (advanced):**
```cpp
// If we only stored pointers:
class Position;  // Forward declaration - don't need full definition

class Portfolio {
private:
    std::map<std::string, Position*> positions_;  // Pointer ok with forward declaration
};
```

**When to use:**
- Header only needs to know type exists (for pointers/references)
- Reduces compilation dependencies
- Faster builds

**Your code:** Doesn't need forward declarations because you store Position by value in the map.

### Design Decisions

#### Why this separation?

**Pros:**
- **Build time**: Only recompile changed files
- **Encapsulation**: Implementation hidden from users
- **Interface clarity**: Headers show what's available
- **Library creation**: Can distribute compiled code

**Cons:**
- More files to manage
- Must declare and define separately
- Can be confusing for beginners

#### Inline Functions

**In your headers:**
```cpp
const std::string& symbol() const { return symbol_; }
//                                  ^^^^^^^^^^^^^^^^^
//                                  Implementation in header
```

**Why inline in header?**
- **Small functions**: Single line, trivial
- **Performance**: Compiler can optimize better
- **Convenience**: Don't need .cpp implementation

**Rule:** Keep inline functions small and simple.

---

## 13. Memory Management and RAII

### Theory
C++ requires manual memory management for dynamic allocation. **RAII** (Resource Acquisition Is Initialization) is a pattern where objects own resources and clean them up automatically.

### 13.1 Stack vs Heap

**Stack allocation (automatic):**
```cpp
void function() {
    Position pos("AAPL", 100, 150.0);
    //       ^^^ Allocated on stack
    // ...
} // pos automatically destroyed here
```

**Heap allocation (manual):**
```cpp
void function() {
    Position* pos = new Position("AAPL", 100, 150.0);
    //              ^^^ Allocated on heap
    // ...
    delete pos;  // MUST manually delete!
}
```

### 13.2 Your Code Uses RAII Perfectly

**Example: Portfolio class:**
```cpp
class Portfolio {
private:
    std::map<std::string, Position> positions_;
    //                     ^^^^^^^^
    // Position objects stored BY VALUE in map
    // No manual memory management needed!
};
```

**What happens:**
```cpp
Portfolio portfolio(100000.0);
// portfolio object created on stack
// positions_ map created automatically
// Positions added to map are owned by map

// When portfolio goes out of scope:
// 1. positions_ map destructor called
// 2. Each Position destructor called
// 3. portfolio destructor called
// Everything cleaned up automatically!
```

### 13.3 STL Containers Manage Memory

**All STL containers use RAII:**
```cpp
std::map<std::string, Position> positions_;
//                               ^^^^^^^^^
// Map owns the Positions
// Automatically grows/shrinks
// Automatically cleans up when destroyed
```

**You never write:**
```cpp
// WRONG - don't do manual memory management
std::map<std::string, Position*> positions_;  // Pointers
positions_["AAPL"] = new Position("AAPL", 100, 150.0);  // Manual new
// ... must remember to delete each one ...
delete positions_["AAPL"];  // Easy to forget!
```

**Your code (correct):**
```cpp
// RIGHT - automatic memory management
std::map<std::string, Position> positions_;  // By value
positions_.emplace("AAPL", Position("AAPL", 100, 150.0));  // No new
// ... everything cleaned up automatically when map destroyed ...
```

### 13.4 Rule of Zero

**Theory:** If you don't manage resources manually, you don't need to write special member functions.

**Your Position class:**
```cpp
class Position {
    // No destructor
    // No copy constructor
    // No copy assignment
    // No move constructor
    // No move assignment
    
    // All members manage themselves!
private:
    std::string symbol_;  // string manages its own memory
    int quantity_;        // primitive type, no management needed
    double entry_price_;  // primitive type
    double current_price_; // primitive type
};
```

**Compiler-generated functions work perfectly:**
```cpp
Position p1("AAPL", 100, 150.0);
Position p2 = p1;  // Copy constructor (compiler-generated) - works perfectly!
Position p3 = std::move(p1);  // Move constructor (compiler-generated) - works perfectly!
```

**Why this works:**
- `std::string` knows how to copy itself
- Primitives (`int`, `double`) copy trivially
- No raw pointers or file handles to manage

### 13.5 Smart Pointers (Advanced, Not Used in Your Code)

**If you needed dynamic allocation:**
```cpp
#include <memory>

// Old way (manual management) ❌
Position* pos = new Position("AAPL", 100, 150.0);
delete pos;  // Easy to forget!

// Modern way (automatic management) ✅
std::unique_ptr<Position> pos = std::make_unique<Position>("AAPL", 100, 150.0);
// No delete needed - automatically cleaned up when pos goes out of scope

// Shared ownership
std::shared_ptr<Position> pos1 = std::make_shared<Position>("AAPL", 100, 150.0);
std::shared_ptr<Position> pos2 = pos1;  // Both share ownership
// Deleted when last shared_ptr is destroyed
```

### Design Decisions

**Why your code doesn't use pointers:**

**Pros of by-value storage:**
- **Simplicity**: No manual memory management
- **Safety**: No memory leaks or dangling pointers
- **Performance**: Better cache locality (data contiguous in memory)
- **Automatic cleanup**: Destructors handle everything

**Cons:**
- **Copying overhead**: If objects are large (Position is small, so OK)
- **No polymorphism**: Can't store derived classes by value

**When you would use pointers:**
- Need polymorphism (store base class pointer to derived objects)
- Objects are huge (expensive to copy)
- Need optional/nullable semantics
- Need shared ownership (multiple owners)

**Your use case:** Position objects are small and well-defined. By-value storage is perfect!

---

## 14. Design Patterns Used

### 14.1 Strategy Pattern

**Intent:** Define a family of algorithms, encapsulate each one, and make them interchangeable.

**Your implementation:**
```cpp
// Abstract strategy
class StrategyBase {
public:
    virtual void on_bar(const Bar& bar) = 0;
    virtual bool should_enter() const = 0;
    virtual bool should_exit() const = 0;
};

// Concrete strategy 1
class SMAStrategy : public StrategyBase {
    void on_bar(const Bar& bar) override {
        // SMA-specific logic
    }
    bool should_enter() const override {
        return short_sma_ > long_sma_;  // Golden cross
    }
    bool should_exit() const override {
        return short_sma_ < long_sma_;  // Death cross
    }
};

// Future: Concrete strategy 2
class RSIStrategy : public StrategyBase {
    void on_bar(const Bar& bar) override {
        // RSI-specific logic
    }
    bool should_enter() const override {
        return rsi_ < 30;  // Oversold
    }
    bool should_exit() const override {
        return rsi_ > 70;  // Overbought
    }
};
```

**Usage:**
```cpp
// Can swap strategies easily
StrategyBase* strategy;

if (use_sma) {
    strategy = new SMAStrategy(20, 50);
} else {
    strategy = new RSIStrategy(14);
}

// Same interface regardless of strategy
while (data_handler.has_next()) {
    Bar bar = data_handler.next_bar();
    strategy->on_bar(bar);
    
    if (strategy->should_enter()) {
        // Enter position
    }
}
```

**Benefits:**
- Easy to add new strategies without changing existing code
- Strategies are interchangeable
- Clear separation of strategy logic

### 14.2 Iterator Pattern

**Intent:** Provide a way to access elements of a container sequentially without exposing underlying representation.

**Your implementation in DataHandler:**
```cpp
class DataHandler {
public:
    void load_csv(const std::string& filepath, const std::string& symbol = "UNKNOWN");
    
    bool has_next() const {
        return current_index_ < bars_.size();
    }
    
    const Bar& next_bar() {
        return bars_[current_index_++];
    }
    
private:
    std::vector<Bar> bars_;
    size_t current_index_;
};
```

**Usage:**
```cpp
DataHandler data;
data.load_csv("data.csv", "SPY");

while (data.has_next()) {
    const Bar& bar = data.next_bar();
    // Process bar
}
```

**Benefits:**
- Hides internal storage (could be vector, deque, file stream)
- Simple interface for sequential access
- Can change implementation without affecting users

### 14.3 Facade Pattern

**Intent:** Provide a simplified interface to a complex subsystem.

**Your Portfolio class is a facade:**
```cpp
class Portfolio {
public:
    // Simple interface hides complexity
    void open_position(const std::string& symbol, int quantity, double price, double commission);
    void close_position(const std::string& symbol, int quantity, double price, double commission);
    
    // Hides complexity of:
    // - Managing map of positions
    // - Calculating P&L
    // - Tracking cash
    // - Commission handling
    
private:
    std::map<std::string, Position> positions_;  // Complex internal state
    double cash_;
    double realized_pnl_;
    // ...
};
```

**Without facade, user would have to:**
```cpp
// Manual position management (complex!)
std::map<std::string, Position> positions;
double cash = 100000.0;

// Open position manually
if (positions.find("AAPL") != positions.end()) {
    positions["AAPL"].adjust_quantity(100, 150.0);
} else {
    positions.emplace("AAPL", Position("AAPL", 100, 150.0));
}
cash -= (100 * 150.0 + commission);

// Easy to make mistakes!
```

**With facade:**
```cpp
// Simple interface
Portfolio portfolio(100000.0);
portfolio.open_position("AAPL", 100, 150.0, 1.0);
// All complexity handled internally
```

### 14.4 Composition Over Inheritance

**Your Portfolio "has-a" Position objects:**
```cpp
class Portfolio {
private:
    std::map<std::string, Position> positions_;  // Composition
    //                     ^^^^^^^^
};
```

**Not:**
```cpp
// BAD - Portfolio doesn't need to inherit from Position
class Portfolio : public Position {  // Wrong!
};
```

**Why composition?**
- Portfolio manages many Positions, not a single Position
- Portfolio and Position have different responsibilities
- More flexible - can change Position implementation

**Rule:** Prefer composition (has-a) over inheritance (is-a) unless there's a clear is-a relationship.

### 14.5 Single Responsibility Principle

**Each class has one job:**

**Position:** Track individual position
```cpp
class Position {
    // Job: Track quantity, prices, calculate P&L for one symbol
};
```

**Portfolio:** Manage multiple positions
```cpp
class Portfolio {
    // Job: Manage positions, track cash, calculate portfolio-wide metrics
};
```

**DataHandler:** Load and provide market data
```cpp
class DataHandler {
    // Job: Read CSV, provide bars sequentially
};
```

**Strategy:** Make trading decisions
```cpp
class StrategyBase {
    // Job: Decide when to enter/exit trades
};
```

**Benefits:**
- Easy to understand (each class is simple)
- Easy to test (test each class independently)
- Easy to modify (changes are localized)
- Easy to reuse (classes are independent)

---

## Summary of Key Design Decisions

### 1. Why Classes Instead of Structs?
- **Encapsulation**: Protect data integrity
- **Invariants**: Maintain valid state (e.g., positions are consistent)
- **Interface**: Clear what users can do

### 2. Why const Everywhere?
- **Safety**: Can't accidentally modify
- **Optimization**: Compiler can optimize better
- **Documentation**: Shows intent clearly

### 3. Why References Over Pointers?
- **Clarity**: Can't be null, always valid
- **Safety**: Can't forget to check for null
- **Performance**: Same as pointers, nicer syntax

### 4. Why std::map for Positions?
- **Fast lookup**: O(log n) by symbol
- **Automatic ordering**: Nice for reports
- **Simple**: No manual memory management

### 5. Why std::deque for Price History?
- **Rolling window**: Efficient pop_front
- **Random access**: Can access any element
- **No reallocation**: Unlike vector

### 6. Why Strategy Pattern?
- **Extensibility**: Easy to add new strategies
- **Swappable**: Can change strategies at runtime
- **Testable**: Test strategies independently

### 7. Why Separate .hpp and .cpp?
- **Build time**: Faster incremental compilation
- **Encapsulation**: Hide implementation details
- **Library distribution**: Can share compiled code

### 8. Why RAII and No Manual new/delete?
- **Safety**: No memory leaks
- **Simplicity**: Automatic cleanup
- **Exception safe**: Resources freed even if exception thrown

---

## Next Steps for Learning

### Concepts to Explore Next:
1. **Templates**: Generic programming (e.g., `template<typename T>`)
2. **Smart pointers**: `unique_ptr`, `shared_ptr` for dynamic allocation
3. **Move semantics**: Efficient resource transfer
4. **Lambda functions**: Inline anonymous functions
5. **Exceptions**: Error handling with try/catch
6. **Multithreading**: Parallel execution
7. **Templates specialization**: Custom behavior for specific types

### Best Practices You're Already Using:
✅ const correctness
✅ References over pointers
✅ RAII and automatic memory management
✅ Range-based for loops
✅ Structured bindings
✅ Member initializer lists
✅ Single responsibility principle
✅ Composition over inheritance

### Your Code Quality:
Your code demonstrates **solid understanding** of modern C++ practices. The design is clean, maintainable, and follows industry best practices. Great work! 🎉

