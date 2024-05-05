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

        template <typename Base>
        inline auto GetFactory() {
            auto it = _factories.find(typeid(Base));
            if (it != _factories.end()) return it->second;
            throw std::logic_error("Type not registered.");
        }

    public:
        Container() = default;

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

        template <typename Base, typename Derived, typename... Args>
        void Register(Lifetime lifetime = Lifetime::Transient) {
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

            // Setup singleton instance if it is a singleton
            if (lifetime == Lifetime::Singleton) {
                auto [raw_ptr, deleter] = factory(std::make_tuple(Args{}...));
                std::unique_ptr<void, DeleterFunc> singleton(raw_ptr, deleter);
                _singletons[typeid(Base)] = std::move(singleton);
            }
        }

        template <typename Singleton, typename Impl, typename... Args>
        void RegisterSingleton() {
            Register<Singleton, Impl, Args...>(Lifetime::Singleton);
        }

        // Update the Get() below to support any number of arguments
        template <typename Singleton>
        std::unique_ptr<Singleton, DeleterFunc>& GetSingleton() {
            if (GetLifetime<Singleton>() != Lifetime::Singleton)
                throw std::logic_error("Type is not singleton.");

            auto it = _singletons.find(typeid(Singleton));
            if (it != _singletons.end())
                return reinterpret_cast<std::unique_ptr<Singleton, DeleterFunc>&>(it->second);
            throw std::logic_error("Singleton not created.");
        }

        template <typename Transient, typename... Args>
        std::unique_ptr<Transient, DeleterFunc> GetTransient(Args&&... args) {
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

    namespace Global {

        template <typename Base, typename Derived, typename... Args>
        inline void Register(Lifetime lifetime = Lifetime::Transient) {
            Container::GetGlobalInstance().Register<Base, Derived, Args...>();
        }

        template <typename Base, typename Impl, typename... Args>
        inline void RegisterSingleton() {
            Container::GetGlobalInstance().RegisterSingleton<Base, Impl, Args...>();
        }

        template <typename Base, typename... Args>
        inline std::unique_ptr<Base, typename Container::DeleterFunc> GetTransient(Args&&... args) {
            return Container::GetGlobalInstance().GetTransient<Base, Args...>(
                std::forward<Args>(args)...
            );
        }

        template <typename Base>
        inline std::unique_ptr<Base, typename Container::DeleterFunc>& GetSingleton() {
            return Container::GetGlobalInstance().GetSingleton<Base>();
        }

        template <typename Base, typename... Args>
        inline void ResetSingleton(Args&&... args) {
            Container::GetGlobalInstance().ResetSingleton<Base>(std::forward<Args>(args)...);
        }
    }
}
