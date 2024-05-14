#pragma once

#include <string_view>

namespace DependencyInjection {

    struct IContainerStorage {
        // NAME
        // PTR

        /** Gets the total number of registrations in the container. */
        virtual size_t size() const = 0;

        /** Removes all registrations from the container. */
        virtual void clear() = 0;

        // virtual void add_singleton(std::string_view name,
    };
}
