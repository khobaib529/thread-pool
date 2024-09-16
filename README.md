# ThreadPool

## Overview
The `ThreadPool` class provides a way to manage a pool of worker threads to execute tasks asynchronously. It is designed to handle tasks efficiently using C++17

## Features

- **Thread Management**: Manages a pool of worker threads.
- **Task Scheduling**: Enqueues tasks to be executed by worker threads.
- **Exception Handling**: Reports exceptions that occur during task execution.
- **Future Support**: Returns a `std::future` to retrieve the result of a task.

## Installation

Simply include the header file in your project:

## Example

Here's a complete example demonstrating how to use the `ThreadPool` class:

```cpp
#include <iostream>
#include <string>
#include "ThreadPool.h"

int main() {
    // Create a ThreadPool with the default number of threads
    ThreadPool pool;

    // Enqueue a task that adds two integers
    auto future1 = pool.enqueue([](int a, int b) {
        return a + b;
    }, 5, 3);

    // Enqueue a task that multiplies two integers
    auto future2 = pool.enqueue([](int a, int b) {
        return a * b;
    }, 10, 20);

    // Enqueue a task that concatenates two strings
    auto future3 = pool.enqueue([](const std::string& s1, const std::string& s2) {
        return s1 + s2;
    }, "Hello, ", "World!");

    // Get the results from the futures
    int sum = future1.get(); // sum will be 8
    int product = future2.get(); // product will be 200
    std::string concatenated = future3.get(); // concatenated will be "Hello, World!"

    // Output the results
    std::cout << "Sum: " << sum << std::endl; // Output: Sum: 8
    std::cout << "Product: " << product << std::endl; // Output: Product: 200
    std::cout << "Concatenated String: " << concatenated << std::endl; // Output: Concatenated String: Hello, World!

    return 0;
}

