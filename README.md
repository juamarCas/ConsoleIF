# ConsoleIF

Simple, header only and lightweight framework for creating console interfaces for cpp.

## Introduction

## Dependencies

```
> spdlog
```

## Example

```cpp
#include <cstdlib>
#include <memory>

#include "ConsoleIf.h"
#include "spdlog/spdlog.h"
#include "Utils.h"
#include "Result.h"


void get_serial_list(const std::vector<std::any>& args){
    spdlog::debug("YEY");
}

void get_test_list(const std::vector<std::any>& args){
    spdlog::debug("Getting test list");
}

int main(){
    #if DEBUG
    spdlog::set_level(spdlog::level::debug);
    #else
    spdlog::set_level(spdlog::level::err);
    #endif 

    ConsoleIf::ConsoleIf console_if;

    std::string get_test = "get_test*this is a test*<int>_list<int string>";
    std::string get_serial_command = "get_serial_list*get a list of every serial device available*<int>";
    console_if.create_command(get_test, get_test_list);

    console_if.create_command("clear", [](const std::vector<std::any>& args){
        std::system("clear");
    });

    console_if.create_command("exit", [](const std::vector<std::any>& args){
        spdlog::debug("Exiting app");
        exit(0);
    });

    console_if.set_indicator('#');

    console_if.create_command(get_serial_command, get_serial_list);

    console_if.begin_app();

    return 0;
}
```
