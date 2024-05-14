# Consumer API

```cpp
// Available on IContainer*

// Getting singletons!
DI::Get<IType>();        // -> IType*
DI::Get<IType>("NAME"); // -> IType*

// Making new instances!
DI::Make<IType>();        // -> IType
DI::Make<IType>("NAME"); // -> IType

// Alternatives for getting various types of pointers!
DI::MakeNew<IType>();    // -> IType*
DI::MakeUnique<IType>(); // -> std::unique_ptr<IType>
DI::MakeShared<IType>(); // -> std::shared_ptr<IType>
```

# Provider API

```cpp
// Available only on Container, not on IContainer*

// Registering singletons!
DI::RegisterSingleton<IType>(IType*);
DI::RegisterSingleton<IType>(IType&);
DI::RegisterSingleton<IType>(unique_ptr<IType>);
DI::RegisterSingleton<IType>(shared_ptr<IType>);
DI::RegisterSingleton<IType, Type>(Args...);

// Register a singleton with a name!
DI::RegisterNamedSingleton<IType>("Name", IType*);
DI::RegisterNamedSingleton<IType>("Name", IType&);
DI::RegisterNamedSingleton<IType>("Name", unique_ptr<IType>);
DI::RegisterNamedSingleton<IType>("Name", shared_ptr<IType>);
DI::RegisterNamedSingleton<IType, Type>("Name", Args...);

// Register an instance factory!
DI::RegisterFactory<IType>(std::function<IType*()>);
DI::RegisterFactory<IType, Type>();

// Register an instance factory with a name!
DI::RegisterNamedFactory<IType>("Name", std::function<IType*()>);
DI::RegisterNamedFactory<IType, Type>("Name");

// Register an instance factory with a name with args!
DI::RegisterNamedFactoryWithArgs<IType, Args...>("Name", std::function<IType*(Args...)>);
DI::RegisterNamedFactoryWithArgs<IType, Type, Args...>("Name");

// Register an instance factory with args!
DI::RegisterFactoryWithArgs<IType, Args...>(std::function<IType*(Args...)>);
DI::RegisterFactoryWithArgs<IType, Type, Args...>();
```
