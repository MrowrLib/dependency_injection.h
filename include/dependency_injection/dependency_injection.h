#pragma once

#include "container.h"

namespace DependencyInjection {

    template <typename Base, typename Derived, typename... Args>
    inline void Register(Lifetime lifetime = Lifetime::Transient) {
        Container::GetGlobalInstance().Register<Base, Derived, Args...>();
    }

    template <typename Base, typename Impl, typename... Args>
    inline void RegisterSingleton() {
        Container::GetGlobalInstance().RegisterSingleton<Base, Impl, Args...>();
    }

    template <typename Base, typename Impl, typename... Args>
    inline void RegisterSingleton(Impl& singleton) {
        Container::GetGlobalInstance().RegisterSingleton<Base, Impl, Args...>(singleton);
    }

    template <typename Base, typename... Args>
    inline std::unique_ptr<Base, typename Container::DeleterFunc> GetTransient(Args&&... args) {
        return Container::GetGlobalInstance().GetTransient<Base, Args...>(std::forward<Args>(args
        )...);
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
