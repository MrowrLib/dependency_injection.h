#pragma once

#if !defined(DI_TypesCounter_StartIndex)
    #define DI_TypesCounter_StartIndex 0
#endif

namespace DependencyInjection {

    template <int N>
    struct TypeCounter {
        static int typeId;
    };

    template <int N>
    int TypeCounter<N>::typeId = DI_TypesCounter_StartIndex;
}
