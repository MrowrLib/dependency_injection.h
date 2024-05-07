# `#include <dependency_injection.h>`

```cpp
#include <dependency_injection.h>

void Example() {
    // Given a type, create a new instance
    auto dog = DI::Make<Dog>();

    // Provide constructor arguments, if available
    auto cat = DI::Make<Cat>("Fluffy");

    // Or get a pointer to a singleton instance
    auto* instance = DI::Get<MySingleton>();
}

// Elsewhere in the code, setup these types...
void Setup() {
    // Register Dog so it created a new DogImplementation
    DI::RegisterInterface<Dog, DogImplementation>();

    // Or it can directly create a concrete type
    DI::RegisterType<Dog>();

    // You can define arguments for the constructor
    DI::RegisterInterface<Cat, CatImplementation, std::string>();
    DI::RegisterType<Cat, std::string>();

    // Singletons can given types or interfaces
    // and DI will automatically construct the instance
    DI::RegisterSingletonType<MySingleton>();
    DI::RegisterSingletonInterface<MySingleton, MySingletonImplementation>();

    // Singletons also support constructor arguments
    DI::RegisterSingletonInterface<MySingleton, MySingletonImplementation, int, std::string>(42, "The Answer");
    DI::RegisterSingletonType<MySingleton, int, std::string>(42, "The Answer");

    // You can reset a singleton which will delete the instance and make a new one
    DI::ResetSingleton<MySingleton>();
    DI::ResetSingleton<MySingleton>(42, "The Answer"); // if constructor arguments

    // Finally, if you want to provide your own reference or unique_ptr to a singleton, you can
    DI::RegisterSingleton<MySingleton>(exampleSingletonInstance);
    DI::RegisterSingleton<MySingleton>(std::make_unique<MySingleton>());
    DI::RegisterSingleton<MySingleton>(std::move(existingUniquePtr));
}

MySingleton exampleSingletonInstance;
```

## What?

Dependency Injection / Inversion of Control container for C++

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
