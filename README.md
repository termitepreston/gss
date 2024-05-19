# GSS

## About Generic Service Scheduler (gss)

- gss allows user to schedule tasks with a limited time budget from a web-front end 

- Central to the working of this program is a producer consumer 
task queue that gets enqueued and dequeued concurrently.

## Building

This project requires the following tools 
to be installed to work:

1. C++20 and up
2. ccache
3. Ninja
4. mold

## Dependencies
1. Boost (ASIO, Beast) for C++ networking.
2. nlohmann/json for JSON SerDe.
3. spdlog for logging
4. Catch2 for testing.


```bash
# Inside project root.
cmake -B build/ -S . -G "Ninja Multi-Config"

# Or for faster builds, which turn off most optimizations and safeguards.
cmake -B build/ -S . --preset unixlike-clang-debug-fast

# Inside gss/build
cmake --build . -j 8

# Then run

gss-server --address 0.0.0.0 --port 8080 --threads 4
```
