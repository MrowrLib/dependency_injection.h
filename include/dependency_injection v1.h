#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <typeindex>
#include <unordered_map>

// UPDATE so you can give it raw pointers. And that's the core storage.
//        and add an IDependencyInjection interface (that has a virtual destructor)

namespace DependencyInjection {
    class DIContainer {
    public:
        using DeleterFunc = std::function<void(void*)>;

    private:
        std::unordered_map<
            std::type_index, std::pair<
                                 std::function<std::pair<void*, DeleterFunc>(std::any)>,
                                 std::unique_ptr<void, DeleterFunc>>>
            _factories;

    public:
        DIContainer() = default;

        template <typename Base, typename Derived, typename... Args>
        void Register() {
            // OMG GROSS CAN WE DO THIS WITHOUT typeid() omg omg gross!!!!
            // it's OK to use a const char* for a name :)
            _factories[typeid(Base)] = {
                [](std::any args) {
                    auto     argsTuple = std::any_cast<std::tuple<Args...>>(args);
                    Derived* raw_ptr   = new Derived(std::apply(
                        [](auto&&... args) {
                            return Derived(std::forward<decltype(args)>(args)...);
                        },
                        argsTuple
                    ));
                    return std::make_pair(static_cast<void*>(raw_ptr), [](void* ptr) {
                        delete static_cast<Derived*>(ptr);
                    });
                },
                nullptr};
        }

        template <typename Base, typename... Args>
        std::unique_ptr<Base, DeleterFunc> New(Args&&... args) {
            auto it = _factories.find(typeid(Base));
            if (it != _factories.end()) {
                auto [raw_ptr, deleter] =
                    it->second.first(std::make_tuple(std::forward<Args>(args)...));
                return std::unique_ptr<Base, DeleterFunc>(static_cast<Base*>(raw_ptr), deleter);
            }
            throw std::logic_error("Type not registered.");
        }

        template <typename Service, typename Impl, typename... Args>
        void RegisterService(Args&&... args) {
            Register<Service, Impl, Args...>();
            ResetService<Service>(std::forward<Args>(args)...);
        }

        template <typename Service, typename... Args>
        void ResetService(Args&&... args) {
            auto it = _factories.find(typeid(Service));
            if (it != _factories.end()) {
                auto& [factory, service] = it->second;
                auto [raw_ptr, deleter]  = factory(std::make_tuple(std::forward<Args>(args)...));
                std::unique_ptr<void, DeleterFunc> new_service(raw_ptr, deleter);
                service.swap(new_service);
            } else {
                throw std::logic_error("Service not registered.");
            }
        }

        template <typename Service>
        std::unique_ptr<Service, DeleterFunc>& Get() {
            auto it = _factories.find(typeid(Service));
            if (it != _factories.end()) {
                auto& [factory, service] = it->second;
                if (service) {
                    return reinterpret_cast<std::unique_ptr<Service, DeleterFunc>&>(service);
                }
                throw std::logic_error("Service not created.");
            }
            throw std::logic_error("Service not registered.");
        }

        static DIContainer& GetGlobalInstance() {
            static DIContainer instance;
            return instance;
        }
    };

    template <typename Base, typename Derived, typename... Args>
    void Register(Args&&... args) {
        DIContainer::GetGlobalInstance().Register<Base, Derived, Args...>();
    }

    template <typename Base, typename... Args>
    std::unique_ptr<Base, typename DIContainer::DeleterFunc> New(Args&&... args) {
        return DIContainer::GetGlobalInstance().New<Base, Args...>(std::forward<Args>(args)...);
    }

    template <typename Service, typename Impl, typename... Args>
    void RegisterService(Args&&... args) {
        DIContainer::GetGlobalInstance().RegisterService<Service, Impl, Args...>(
            std::forward<Args>(args)...
        );
    }

    template <typename Service, typename... Args>
    void ResetService(Args&&... args) {
        DIContainer::GetGlobalInstance().ResetService<Service>(std::forward<Args>(args)...);
    }

    template <typename Service>
    std::unique_ptr<Service, typename DIContainer::DeleterFunc>& Get() {
        return DIContainer::GetGlobalInstance().Get<Service>();
    }
}
