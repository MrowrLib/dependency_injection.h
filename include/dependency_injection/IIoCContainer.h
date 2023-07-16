#pragma once

#include <function_pointer.h>
#include <void_pointer.h>

namespace DependencyInjection {

    struct IIoCContainer {
        virtual ~IIoCContainer() = default;

        // virtual void RegisterFactoryFunction(
        //     const char* typeId, IFunctionPointer<void(IVoidPointer*)>* factoryFunctionPointer
        // ) = 0;

        // virtual void RegisterServicePointer(
        //     const char* typeId, IVoidPointer* servicePointer, bool deleteOnRemove = true
        // ) = 0;

        // virtual void UnregisterService(const char* typeId) = 0;
    };
}
