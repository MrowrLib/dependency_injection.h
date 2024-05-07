#pragma once

#include "container.h"

namespace DependencyInjection {

    template <typename Base, typename Derived, typename... Args>
    inline void RegisterInterface(Lifetime lifetime = Lifetime::Transient) {
        Container::GetGlobalInstance().RegisterInterface<Base, Derived, Args...>();
    }

    template <typename Impl, typename... Args>
    inline void RegisterType(Lifetime lifetime = Lifetime::Transient) {
        Container::GetGlobalInstance().RegisterType<Impl, Args...>();
    }

    template <typename Base, typename Derived, typename... Args>
    inline void RegisterSingletonInterface(Args&&... args) {
        Container::GetGlobalInstance().RegisterSingletonInterface<Base, Derived, Args...>(
            std::forward<Args>(args)...
        );
    }

    template <typename Impl, typename... Args>
    inline void RegisterSingletonType(Args&&... args) {
        Container::GetGlobalInstance().RegisterSingletonType<Impl, Args...>(std::forward<Args>(args
        )...);
    }

    template <typename Base>
    inline void RegisterSingleton(Base& singleton) {
        Container::GetGlobalInstance().RegisterSingleton<Base>(singleton);
    }

    template <typename Base>
    inline void RegisterSingleton(Base* singletonPtr) {
        Container::GetGlobalInstance().RegisterSingleton<Base>(singletonPtr);
    }

    template <typename Base>
    inline void RegisterSingleton(std::unique_ptr<Base> singletonPtr) {
        Container::GetGlobalInstance().RegisterSingleton<Base>(std::move(singletonPtr));
    }

    template <typename Base, typename... Args>
    inline std::unique_ptr<Base, typename Container::DeleterFunc> Make(Args&&... args) {
        return Container::GetGlobalInstance().Make<Base, Args...>(std::forward<Args>(args)...);
    }

    template <typename Base>
    inline Base* Get() {
        return Container::GetGlobalInstance().Get<Base>();
    }

    template <typename Base, typename... Args>
    inline void ResetSingleton(Args&&... args) {
        Container::GetGlobalInstance().ResetSingleton<Base>(std::forward<Args>(args)...);
    }
}
