#pragma once

#include "TypeCounter.h"

namespace DependencyInjection {

    template <typename T>
    struct TypeIdentifier {
        static const int typeId;
    };

    template <typename T>
    const int TypeIdentifier<T>::typeId = TypeCounter<0>::typeId++;
}
