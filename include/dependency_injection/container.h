#pragma once

#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <typeindex>
#include <unordered_map>

namespace DependencyInjection {
    enum class Lifetime { Transient, Singleton };

    class Container {
    public:
        using DeleterFunc = std::function<void(void*)>;

    private:
        std::unordered_map<std::type_index, Lifetime> _lifetimes;
        std::unordered_map<std::type_index, std::function<std::pair<void*, DeleterFunc>(std::any)>>
                                                                                _factories;
        std::unordered_map<std::type_index, std::unique_ptr<void, DeleterFunc>> _singletons;
        std::unordered_map<std::type_index, void*>                              _singletonsRawPtrs;

        template <typename Base>
        inline auto GetFactory() {
            auto it = _factories.find(typeid(Base));
            if (it->second == nullptr)
                throw std::logic_error(
                    "Type has no factory function: " + std::string(typeid(Base).name())
                );
            if (it != _factories.end()) return it->second;
            throw std::logic_error("Type not registered: " + std::string(typeid(Base).name()));
        }

    public:
        template <typename Base>
        bool IsRegistered() {
            return _factories.find(typeid(Base)) != _factories.end();
        }

        template <typename Base>
        Lifetime GetLifetime() {
            auto it = _lifetimes.find(typeid(Base));
            if (it != _lifetimes.end()) return it->second;
            throw std::logic_error("Type not registered.");
        }

        template <typename Singleton, typename... Args>
        void ResetSingleton(Args&&... args) {
            if (GetLifetime<Singleton>() != Lifetime::Singleton)
                throw std::logic_error("Singleton is not a singleton.");

            // Get the factory function
            auto factory = GetFactory<Singleton>();

            // Create a new instance of the singleton using the arguments provided
            auto [raw_ptr, deleter] = factory(std::make_tuple(std::forward<Args>(args)...));

            // Store the new instance of the singleton
            std::unique_ptr<void, DeleterFunc> singleton(raw_ptr, deleter);
            _singletons[typeid(Singleton)] = std::move(singleton);
        }

        template <typename Base>
        void ResetSingleton(Base* singletonPtr) {
            if (GetLifetime<Base>() != Lifetime::Singleton)
                throw std::logic_error("Singleton is not a singleton.");

            _singletonsRawPtrs[typeid(Base)] = singletonPtr;

            auto existing_singleton = _singletons.find(typeid(Base));
            if (existing_singleton != _singletons.end()) existing_singleton->second.reset();
        }

        template <typename Base, typename Derived, typename... Args>
        void RegisterInterface(Lifetime lifetime = Lifetime::Transient) {
            // Create factory function
            auto factory = [](std::any args) {
                auto     argsTuple = std::any_cast<std::tuple<Args...>>(args);
                Derived* raw_ptr   = new Derived(std::apply(
                    [](auto&&... args) { return Derived(std::forward<decltype(args)>(args)...); },
                    argsTuple
                ));
                return std::make_pair(static_cast<void*>(raw_ptr), [](void* ptr) {
                    delete static_cast<Derived*>(ptr);
                });
            };

            // Store factory function
            _factories[typeid(Base)] = factory;

            // Store lifetime associated with the type
            _lifetimes[typeid(Base)] = lifetime;
        }

        template <typename Impl, typename... Args>
        void RegisterType(Lifetime lifetime = Lifetime::Transient) {
            RegisterInterface<Impl, Impl, Args...>(lifetime);
        }

        template <typename Base, typename Impl, typename... Args>
        void RegisterSingletonInterface(Args&&... args) {
            RegisterInterface<Base, Impl, Args...>(Lifetime::Singleton);
            ResetSingleton<Base>(std::forward<Args>(args)...);
        }

        template <typename Impl, typename... Args>
        void RegisterSingletonType(Args&&... args) {
            RegisterType<Impl, Args...>(Lifetime::Singleton);
            ResetSingleton<Impl>(std::forward<Args>(args)...);
        }

        template <typename Base>
        void RegisterSingleton(Base& singleton) {
            _factories[typeid(Base)] = nullptr;
            _lifetimes[typeid(Base)] = Lifetime::Singleton;
            _singletons[typeid(Base)] =
                std::unique_ptr<Base, DeleterFunc>(&singleton, [](void*) {});
        }

        template <typename Base>
        void RegisterSingleton(Base* singletonPtr) {
            _lifetimes[typeid(Base)] = Lifetime::Singleton;
            ResetSingleton<Base>(singletonPtr);
        }

        template <typename Base>
        void RegisterSingleton(std::unique_ptr<Base> singletonPtr) {
            _factories[typeid(Base)] = nullptr;
            _lifetimes[typeid(Base)] = Lifetime::Singleton;

            auto*       raw_ptr = singletonPtr.release();
            DeleterFunc deleter = [raw_ptr, captured_deleter =
                                                singletonPtr.get_deleter()](void* ptr) mutable {
                std::unique_ptr<Base, decltype(captured_deleter)> temp_ptr(
                    static_cast<Base*>(ptr), std::move(captured_deleter)
                );
                temp_ptr.reset();
            };

            _singletons[typeid(Base)] = std::unique_ptr<void, DeleterFunc>(raw_ptr, deleter);
        }

        template <typename Singleton>
        Singleton* Get() {
            if (GetLifetime<Singleton>() != Lifetime::Singleton)
                throw std::logic_error("Type is not singleton.");

            auto it = _singletons.find(typeid(Singleton));
            if (it != _singletons.end())
                return reinterpret_cast<std::unique_ptr<Singleton, DeleterFunc>&>(it->second).get();

            auto it_raw_ptr = _singletonsRawPtrs.find(typeid(Singleton));
            if (it_raw_ptr != _singletonsRawPtrs.end())
                return static_cast<Singleton*>(it_raw_ptr->second);

            throw std::logic_error("Singleton not created.");
        }

        template <typename Transient, typename... Args>
        std::unique_ptr<Transient, DeleterFunc> Make(Args&&... args) {
            if (GetLifetime<Transient>() != Lifetime::Transient)
                throw std::logic_error("Type is not transient.");

            auto factory            = GetFactory<Transient>();
            auto [raw_ptr, deleter] = factory(std::make_tuple(std::forward<Args>(args)...));
            return std::unique_ptr<Transient, DeleterFunc>(
                static_cast<Transient*>(raw_ptr), deleter
            );
        }

        static Container& GetGlobalInstance() {
            static Container instance;
            return instance;
        }
    };
}
