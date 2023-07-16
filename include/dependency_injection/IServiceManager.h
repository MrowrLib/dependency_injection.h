#pragma once

#include <function_pointer.h>
#include <void_pointer.h>

#include "ServiceName.h"
#include "ServicePointer.h"

namespace DependencyInjection {

    struct IServiceManager {
        virtual ~IServiceManager() = default;

        virtual void RegisterServicePointer(ServiceName, IVoidPointer* servicePointer) = 0;
        virtual void UnregisterService(ServiceName)                                    = 0;
        virtual void DestroyService(ServiceName)                                       = 0;
        virtual void ResetService(ServiceName)                                         = 0;
        virtual ServicePointer GetServicePointer(ServiceName)                          = 0;
        virtual void           ForEachService(
                      IFunctionPointer<void(ServiceName, ServicePointer servicePointer)>*
                          callbackFunctionPointer
                  )          = 0;
        virtual void Clear() = 0;
    };
}
