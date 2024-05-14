#define SPEC_NAME RegisterNamedSingleton

#include "TestHelper.h"  // IWYU pragma: keep

// DI::RegisterNamedSingleton<IType>("Name", IType*);
// DI::RegisterNamedSingleton<IType>("Name", IType&);
// DI::RegisterNamedSingleton<IType>("Name", unique_ptr<IType>);
// DI::RegisterNamedSingleton<IType>("Name", shared_ptr<IType>);
// DI::RegisterNamedSingleton<IType, Type>("Name", Args...);

DependencyInjection::Container ioc;

Setup {
    ioc.clear();
    AssertThat(ioc, IsEmpty());
}

Test(R"(DI::RegisterNamedSingleton<IType>("Name", IType*);)") {
    // Create singleton instance
    Example example;

    // Register singleton instance
    ioc.RegisterNamedSingleton<IExample>("ExampleSingleton", &example);

    // Get singleton instance
    auto* exampleSingleton = ioc.Get<IExample>("ExampleSingleton");

    AssertThat(exampleSingleton, Equals(&example));
}
