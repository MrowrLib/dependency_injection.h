#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <typeindex>
#include <unordered_map>

namespace DependencyInjection {
    class DIContainer {
        using DeleterFunc = std::function<void(void*)>;

        std::unordered_map<std::type_index, std::function<std::pair<void*, DeleterFunc>(std::any)>>
            _factories;

    public:
        DIContainer() = default;

        template <typename Base, typename Derived, typename... Args>
        void Register() {
            _factories[typeid(Base)] = [](std::any args) {
                auto     argsTuple = std::any_cast<std::tuple<Args...>>(args);
                Derived* raw_ptr   = new Derived(std::apply(
                    [](auto&&... args) { return Derived(std::forward<decltype(args)>(args)...); },
                    argsTuple
                ));
                return std::make_pair(static_cast<void*>(raw_ptr), [](void* ptr) {
                    delete static_cast<Derived*>(ptr);
                });
            };
        }

        template <typename Base, typename... Args>
        std::unique_ptr<Base, DeleterFunc> New(Args&&... args) {
            auto it = _factories.find(typeid(Base));
            if (it != _factories.end()) {
                auto [raw_ptr, deleter] = it->second(std::make_tuple(std::forward<Args>(args)...));
                return std::unique_ptr<Base, DeleterFunc>(static_cast<Base*>(raw_ptr), deleter);
            }
            throw std::logic_error("Type not registered.");
        }
    };
}
