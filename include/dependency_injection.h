#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

namespace DependencyInjection {

    class DIContainer {
        using DeleterFunc = std::function<void(void*)>;
        using CreatorFunc = std::function<std::pair<void*, DeleterFunc>()>;

        std::unordered_map<std::type_index, CreatorFunc> _factories;

    public:
        DIContainer() = default;

        template <typename Base, typename Derived>
        void Register() {
            _factories[typeid(Base)] = [] {
                Derived* raw_ptr = new Derived();
                return std::make_pair(static_cast<void*>(raw_ptr), [](void* ptr) {
                    delete static_cast<Derived*>(ptr);
                });
            };
        }

        template <typename Base>
        std::unique_ptr<Base, DeleterFunc> New() {
            auto it = _factories.find(typeid(T));
            if (it != _factories.end()) {
                auto [raw_ptr, deleter] = it->second();
                return std::unique_ptr<Base, DeleterFunc>(static_cast<Base*>(raw_ptr), deleter);
            }
            throw std::logic_error("Type not registered.");
        }
    };
}
