#pragma once

#include <function_pointer.h>
#include <void_pointer.h>

#include <memory>


namespace DependencyInjection {

    struct IFactoryManager {
        virtual ~IFactoryManager() = default;

        virtual RegisterFactoryFunction(
            const char* typeId, IFunctionPointer<void(IVoidPointer*)>* factoryFunctionPointer
        )                                                                       = 0;
        virtual UnregisterFactoryFunction(const char* typeId)                   = 0;
        virtual void*                         CreatePointer(const char* typeId) = 0;
        virtual std::unique_ptr<IVoidPointer> CreateUnique(const char* typeId)  = 0;
        virtual void                          Clear()                           = 0;
    };
}
