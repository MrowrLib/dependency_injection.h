#pragma once

#include <string_view>

namespace DependencyInjection {

    struct IRegistry {
        virtual ~IRegistry() = default;

        template <typename T>
        void RegisterNamedSingleton(std::string_view name, T* instance) {
            //
        }
    };
}
