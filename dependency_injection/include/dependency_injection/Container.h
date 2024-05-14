#pragma once

#include <string_view>

#include "IContainer.h"
#include "IContainerStorage.h"
#include "IRegistry.h"

namespace DependencyInjection {

    class Container : public IContainer,         // Consumer interface, e.g. Get<T>
                      public IContainerStorage,  // Storage interface, e.g. containers
                      public IRegistry           // Registration interface, e.g. Register<T>
    {
    public:
        /*
            IContainer implementation
        */

        bool   empty() const { return true; }
        size_t size() const { return 0; }
        void   clear() {}

        /*
            Named Singletons
        */

        template <typename T>
        void RegisterNamedSingleton(const std::string& name, T* instance) {}
    };
}
