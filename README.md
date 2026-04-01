# `#include <dependency_injection.h>`

```cpp
#include <dependency_injection.h>

DI::Container services;

// Register an interface to an implementation
services.RegisterInterface<ILogger, FileLogger>();

// Register a singleton
services.RegisterSingletonInterface<IDatabase, SqliteDatabase>();

// Create transient instances (caller owns it, new each time)
auto logger = services.Make<ILogger>();

// Get singletons (container owns it, same instance every time)
auto* db = services.Get<IDatabase>();
```

### Global container

A global container is available for convenience:

```cpp
DI::RegisterSingletonType<AppConfig>();
auto* config = DI::Get<AppConfig>();
```

### Constructor arguments

If the implementation type takes constructor arguments, declare the argument
types during registration and pass the values when creating instances:

```cpp
// FileLogger's constructor takes a std::string (the log file path)
//                                          vvvvvvvvvvvvv
services.RegisterInterface<ILogger, FileLogger, std::string>();

// Pass the actual value when you Make it
auto logger = services.Make<ILogger>(std::string("/var/log/app.log"));
```

For singletons, the values are passed at registration time (since the
container creates the instance immediately):

```cpp
// Database's constructor takes a std::string (the connection string)
//                                       vvvvvvvvvvvvv
services.RegisterSingletonType<Database, std::string>("connection_string");
//                                                     ^^^^^^^^^^^^^^^^^^^
//                                          value passed here, at registration
```

### Existing instances

```cpp
// Reference (you own the lifetime)
MyService myService;
services.RegisterSingleton<MyService>(myService);

// unique_ptr (container takes ownership)
services.RegisterSingleton<MyService>(std::make_unique<MyService>());
```

### Reset singletons

```cpp
services.ResetSingleton<IDatabase>();
```

## What?

Dependency Injection / Inversion of Control container for C++.

- `Make<T>()` returns a `unique_ptr` — caller owns it, new instance every time
- `Get<T>()` returns a raw pointer — container owns it, same instance every time

## Installation

### xmake

#### `xmake.lua`

```lua
add_repositories("MrowrLib https://github.com/MrowrLib/Packages.git")
add_requires("dependency_injection")

target("Example")
    add_packages("dependency_injection")
```

### vcpkg

#### `CMakeLists.txt`

```cmake
add_executable(Example main.cpp)

# Find dependency_injection and link it to your target
find_package(dependency_injection CONFIG REQUIRED)
target_link_libraries(Example PRIVATE dependency_injection::dependency_injection)
```

#### `vcpkg.json`

```json
{
    "dependencies": ["mrowr-dependency-injection"]
}
```

#### `vcpkg-configuration.json`

```json
{
    "default-registry": {
        "kind": "git",
        "repository": "https://github.com/microsoft/vcpkg.git",
        "baseline": "95252eadd63118201b0d0df0b4360fa613f0de84"
    },
    "registries": [
        {
            "kind": "git",
            "repository": "https://github.com/MrowrLib/Packages.git",
            "baseline": "0e9c7ad01ab81bb4f381360a9726bbe6c77e4865",
            "packages": ["mrowr-dependency-injection"]
        }
    ]
}
```

> _Update the default-registry baseline to the latest commit from https://github.com/microsoft/vcpkg_
> _Update the MrowrLib/Packages baseline to the latest commit from https://github.com/MrowrLib/Packages_

## Why?

I wanted a super duper simple dependency injection / inversion of control library.

I just need:
- Register a type and get a unique instance of the derived type
- Register a long-lived service and get a reference to the instance


## License

Use however, no attribution required.

```
BSD Zero Clause License (SPDX: 0BSD)

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
```
