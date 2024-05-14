#pragma once

#include <collections.h>
#include <void_pointer.h>

#include <stdexcept>
#include <string>
#include <string_view>

namespace DependencyInjection {

    class Container {
        collections_map<std::string, void_ptr> _singletons;

    public:
        /*
            Container storage functions
        */

        bool   empty() const { return _singletons.empty(); }
        void   clear() { _singletons.clear(); }
        size_t size() const { return _singletons.size(); }

        /*
            Named Singletons
        */

        template <typename T>
        T* Get(std::string_view name) {
            auto it = _singletons.find(name.data());
            if (it == _singletons.end()) return nullptr;
            return it->second->as<T*>();
        }

        template <typename T>
        void RegisterNamedSingleton(const std::string& name, T* singletonRawPointer) {
            auto [result, success] =
                _singletons.try_emplace(name, make_void_ptr(singletonRawPointer));
            if (!success) {
                // TODO: in tests define what this behavior should be :)
                throw std::runtime_error("Named singleton already exists");
            }
            result->second->disable_delete();
        }

        template <typename T>
        void RegisterNamedSingleton(const std::string& name, T& singletonReference) {
            RegisterNamedSingleton(name, &singletonReference);
        }
    };
}
