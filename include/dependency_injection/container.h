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

        // Bug 1 fix: check end() BEFORE dereferencing, then check null
        template <typename Base>
        inline auto GetFactory() {
            auto it = _factories.find(typeid(Base));
            if (it == _factories.end())
                throw std::logic_error("Type not registered: " + std::string(typeid(Base).name()));
            if (it->second == nullptr)
                throw std::logic_error(
                    "Type has no factory function: " + std::string(typeid(Base).name())
                );
            return it->second;
        }

    public:
        template <typename Base>
        bool IsRegistered() {
            return _lifetimes.find(typeid(Base)) != _lifetimes.end();
        }

        template <typename Base>
        Lifetime GetLifetime() {
            auto it = _lifetimes.find(typeid(Base));
            if (it != _lifetimes.end()) return it->second;
            throw std::logic_error("Type not registered: " + std::string(typeid(Base).name()));
        }

        template <typename Singleton, typename... Args>
        void ResetSingleton(Args&&... args) {
            if (GetLifetime<Singleton>() != Lifetime::Singleton)
                throw std::logic_error(
                    "Singleton is not a singleton: " + std::string(typeid(Singleton).name())
                );

            auto factory = GetFactory<Singleton>();
            auto [raw_ptr, deleter] = factory(std::make_tuple(std::forward<Args>(args)...));

            // Clear any raw pointer entry
            _singletonsRawPtrs.erase(typeid(Singleton));

            std::unique_ptr<void, DeleterFunc> singleton(raw_ptr, deleter);
            _singletons[typeid(Singleton)] = std::move(singleton);
        }

        // Bug 3 fix: erase from _singletons instead of reset()
        template <typename Base>
        void ResetSingletonPointer(Base* singletonPtr) {
            if (GetLifetime<Base>() != Lifetime::Singleton)
                throw std::logic_error(
                    "Singleton is not a singleton: " + std::string(typeid(Base).name())
                );

            _singletonsRawPtrs[typeid(Base)] = singletonPtr;
            _singletons.erase(typeid(Base));
        }

        // Bug 5 fix: factory constructs directly on heap, no temp + move
        template <typename Base, typename Derived, typename... Args>
        void RegisterInterface(Lifetime lifetime = Lifetime::Transient) {
            auto factory = [](std::any args) {
                auto  argsTuple = std::any_cast<std::tuple<Args...>>(args);
                auto* raw_ptr   = std::apply(
                    [](auto&&... args) {
                        return new Derived(std::forward<decltype(args)>(args)...);
                    },
                    argsTuple
                );
                return std::make_pair(static_cast<void*>(raw_ptr), DeleterFunc([](void* ptr) {
                    delete static_cast<Derived*>(ptr);
                }));
            };

            _factories[typeid(Base)] = factory;
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

        // Bug 7 fix: register factory as nullptr + lifetime so IsRegistered works
        template <typename Base>
        void RegisterSingleton(Base* singletonPtr) {
            _factories[typeid(Base)] = nullptr;
            _lifetimes[typeid(Base)] = Lifetime::Singleton;
            _singletonsRawPtrs[typeid(Base)] = singletonPtr;
        }

        // Bug 8 fix: simplified deleter, no dead captures
        template <typename Base>
        void RegisterSingleton(std::unique_ptr<Base> singletonPtr) {
            _factories[typeid(Base)] = nullptr;
            _lifetimes[typeid(Base)] = Lifetime::Singleton;

            auto* raw_ptr = singletonPtr.release();
            _singletons[typeid(Base)] = std::unique_ptr<void, DeleterFunc>(
                raw_ptr, [](void* ptr) { delete static_cast<Base*>(ptr); }
            );
        }

        // Bug 2 fix: static_cast instead of reinterpret_cast
        template <typename Singleton>
        Singleton* Get() {
            if (GetLifetime<Singleton>() != Lifetime::Singleton)
                throw std::logic_error(
                    "Type is not singleton: " + std::string(typeid(Singleton).name())
                );

            auto it = _singletons.find(typeid(Singleton));
            if (it != _singletons.end() && it->second)
                return static_cast<Singleton*>(it->second.get());

            auto it_raw_ptr = _singletonsRawPtrs.find(typeid(Singleton));
            if (it_raw_ptr != _singletonsRawPtrs.end())
                return static_cast<Singleton*>(it_raw_ptr->second);

            throw std::logic_error(
                "Singleton not created: " + std::string(typeid(Singleton).name())
            );
        }

        template <typename Transient, typename... Args>
        std::unique_ptr<Transient, DeleterFunc> Make(Args&&... args) {
            if (GetLifetime<Transient>() != Lifetime::Transient)
                throw std::logic_error(
                    "Type is not transient: " + std::string(typeid(Transient).name())
                );

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
