# `#include <dependency_injection.h>`

```cpp
#include <dependency_injection.h>

void Example() {
    // Register a type and get a unique instance of the derived type
    DependencyInjection::Register<IThing, ThingImpl>();
    auto thing = DependencyInjection::New<IThing>();

    // Register a long-lived service and get a reference to the instance
    DependencyInjection::RegisterService<IService, IServiceImpl>();
    auto& service = DependencyInjection::Get<IService>();
}
```

## What?

Simplest dependency injection / inversion of control container that I could think of.

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

## How?

Either use the global interface (_which uses a global singleton container_):

```cpp
#include <dependency_injection.h>

void Example() {
    // Register a type and get a unique instance of the derived type
    DependencyInjection::Register<IThing, ThingImpl>();
    auto thing = DependencyInjection::New<IThing>();

    // Register a long-lived service and get a reference to the instance
    DependencyInjection::RegisterService<IService, IServiceImpl>();
    auto& service = DependencyInjection::Get<IService>();
}
```

Or create your own container:

```cpp
#include <dependency_injection.h>

void Example() {
    DependencyInjection::Container container;

    // Register a type and get a unique instance of the derived type
    container.Register<IThing, ThingImpl>();
    auto thing = container.New<IThing>();

    // Register a long-lived service and get a reference to the instance
    container.RegisterService<IService, IServiceImpl>();
    auto& service = container.Get<IService>();
}
```

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
