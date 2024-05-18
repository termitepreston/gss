# GSS

## About Generic Service Scheduler (gss)

- gss allows user to schedule tasks with a limited time budget from a web-front end 


## Building

This project requires the following tools 
to be installed to work:

1. C++20 and up
2. ccache


```bash
# Inside project root.
cmake -B build/ -S . -G "Ninja Multi-Config" -Dgss_WARNINGS_AS_ERRORS=OFF

# Inside gss/build
cmake --build . --config Debug
```
