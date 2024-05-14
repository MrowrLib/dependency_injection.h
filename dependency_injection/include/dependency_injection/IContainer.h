#pragma once

#include <string_view>

namespace DependencyInjection {

    /** Represents an inversion of control (IoC) container for dependency injection. */
    struct IContainer {
        virtual ~IContainer() = default;

        template <typename T>
        T* Get(std::string_view name) {
            return nullptr;
        }
    };
}
