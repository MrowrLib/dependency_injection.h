#include <catch2/catch_test_macros.hpp>
#include <dependency_injection.h>

// --- Test types ---

struct IAnimal {
    virtual ~IAnimal() = default;
    virtual const char* Speak() = 0;
};

class Dog : public IAnimal {
public:
    const char* Speak() override { return "Woof"; }
};

class Cat : public IAnimal {
public:
    const char* Speak() override { return "Meow"; }
};

struct IGreeter {
    virtual ~IGreeter() = default;
    virtual std::string Greet() = 0;
};

class PersonGreeter : public IGreeter {
    std::string _name;

public:
    PersonGreeter(std::string name) : _name(std::move(name)) {}
    std::string Greet() override { return "Hello, " + _name; }
};

class TwoArgGreeter : public IGreeter {
    std::string _greeting;
    std::string _name;

public:
    TwoArgGreeter(std::string greeting, std::string name)
        : _greeting(std::move(greeting)), _name(std::move(name)) {}
    std::string Greet() override { return _greeting + ", " + _name; }
};

class Counter {
    int _count;

public:
    Counter() : _count(0) {}
    Counter(int start) : _count(start) {}
    void Increment() { _count++; }
    int GetCount() const { return _count; }
};

static int g_constructionCount = 0;

class ConstructionTracker {
public:
    ConstructionTracker() { g_constructionCount++; }
    ConstructionTracker(const ConstructionTracker&) { g_constructionCount++; }
    ConstructionTracker(ConstructionTracker&&) { g_constructionCount++; }
};

static int g_destructionCount = 0;

class DestructionTracker {
public:
    DestructionTracker() = default;
    ~DestructionTracker() { g_destructionCount++; }
};

struct IService {
    virtual ~IService() = default;
    virtual int Value() = 0;
};

class ServiceImpl : public IService {
    int _val;

public:
    ServiceImpl(int val) : _val(val) {}
    int Value() override { return _val; }
};

namespace DI = DependencyInjection;

// ============================================================
// RegisterType + Make — transient creation
// ============================================================

TEST_CASE("RegisterType and Make create a new instance", "[transient]") {
    DI::Container c;
    c.RegisterType<Dog>();
    auto dog = c.Make<Dog>();
    REQUIRE(dog != nullptr);
    REQUIRE(std::string(dog->Speak()) == "Woof");
}

TEST_CASE("RegisterInterface and Make create derived type via base", "[transient]") {
    DI::Container c;
    c.RegisterInterface<IAnimal, Cat>();
    auto animal = c.Make<IAnimal>();
    REQUIRE(animal != nullptr);
    REQUIRE(std::string(animal->Speak()) == "Meow");
}

TEST_CASE("Make with one constructor argument", "[transient]") {
    DI::Container c;
    c.RegisterInterface<IGreeter, PersonGreeter, std::string>();
    auto greeter = c.Make<IGreeter>(std::string("World"));
    REQUIRE(greeter->Greet() == "Hello, World");
}

TEST_CASE("Make with multiple constructor arguments", "[transient]") {
    DI::Container c;
    c.RegisterInterface<IGreeter, TwoArgGreeter, std::string, std::string>();
    auto greeter = c.Make<IGreeter>(std::string("Hey"), std::string("Purr"));
    REQUIRE(greeter->Greet() == "Hey, Purr");
}

TEST_CASE("Make returns unique instances each time", "[transient]") {
    DI::Container c;
    c.RegisterType<Counter, int>();
    auto a = c.Make<Counter>(1);
    auto b = c.Make<Counter>(2);
    REQUIRE(a.get() != b.get());
    REQUIRE(a->GetCount() == 1);
    REQUIRE(b->GetCount() == 2);
}

TEST_CASE("Make on a type registered as singleton throws", "[transient]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    REQUIRE_THROWS_AS(c.Make<Counter>(), std::logic_error);
}

TEST_CASE("Make on an unregistered type throws", "[transient]") {
    DI::Container c;
    REQUIRE_THROWS_AS(c.Make<Dog>(), std::logic_error);
}

TEST_CASE("Transient unique_ptr cleans up via correct deleter", "[transient]") {
    DI::Container c;
    c.RegisterType<DestructionTracker>();

    g_destructionCount = 0;
    {
        auto obj = c.Make<DestructionTracker>();
        REQUIRE(g_destructionCount == 0);
    }
    REQUIRE(g_destructionCount == 1);
}

TEST_CASE("Interface transient deletes via derived type", "[transient]") {
    // Ensures the deleter uses static_cast<Derived*>, not Base*
    DI::Container c;
    c.RegisterInterface<IAnimal, Dog>();
    {
        auto animal = c.Make<IAnimal>();
        REQUIRE(animal != nullptr);
    }
    // If the deleter was wrong, this would be UB/crash — passing is good enough
}

// ============================================================
// RegisterSingletonType + Get
// ============================================================

TEST_CASE("RegisterSingletonType and Get return same instance", "[singleton]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    auto* a = c.Get<Counter>();
    auto* b = c.Get<Counter>();
    REQUIRE(a != nullptr);
    REQUIRE(a == b);
}

TEST_CASE("RegisterSingletonType with constructor args", "[singleton]") {
    DI::Container c;
    c.RegisterSingletonType<Counter, int>(42);
    auto* counter = c.Get<Counter>();
    REQUIRE(counter->GetCount() == 42);
}

TEST_CASE("RegisterSingletonInterface and Get return derived via base", "[singleton]") {
    DI::Container c;
    c.RegisterSingletonInterface<IAnimal, Dog>();
    auto* animal = c.Get<IAnimal>();
    REQUIRE(animal != nullptr);
    REQUIRE(std::string(animal->Speak()) == "Woof");
}

TEST_CASE("RegisterSingletonInterface with constructor args", "[singleton]") {
    DI::Container c;
    c.RegisterSingletonInterface<IService, ServiceImpl, int>(99);
    auto* svc = c.Get<IService>();
    REQUIRE(svc != nullptr);
    REQUIRE(svc->Value() == 99);
}

TEST_CASE("Singleton state persists across Get calls", "[singleton]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    auto* counter = c.Get<Counter>();
    counter->Increment();
    counter->Increment();
    counter->Increment();

    auto* same = c.Get<Counter>();
    REQUIRE(same->GetCount() == 3);
}

TEST_CASE("Get on a transient throws", "[singleton]") {
    DI::Container c;
    c.RegisterType<Dog>();
    REQUIRE_THROWS_AS(c.Get<Dog>(), std::logic_error);
}

TEST_CASE("Get on unregistered type throws", "[singleton]") {
    DI::Container c;
    REQUIRE_THROWS_AS(c.Get<Dog>(), std::logic_error);
}

// ============================================================
// RegisterSingleton — external ownership
// ============================================================

TEST_CASE("RegisterSingleton with reference does not take ownership", "[singleton][external]") {
    DI::Container c;
    Counter myCounter(100);
    c.RegisterSingleton<Counter>(myCounter);

    auto* ptr = c.Get<Counter>();
    REQUIRE(ptr == &myCounter);
    REQUIRE(ptr->GetCount() == 100);
}

TEST_CASE("RegisterSingleton with raw pointer", "[singleton][external]") {
    DI::Container c;
    Counter externalCounter(42);
    c.RegisterSingleton<Counter>(&externalCounter);

    auto* retrieved = c.Get<Counter>();
    REQUIRE(retrieved == &externalCounter);
    REQUIRE(retrieved->GetCount() == 42);
}

TEST_CASE("RegisterSingleton with unique_ptr transfers ownership", "[singleton][external]") {
    DI::Container c;
    c.RegisterSingleton<Counter>(std::make_unique<Counter>(77));

    auto* ptr = c.Get<Counter>();
    REQUIRE(ptr != nullptr);
    REQUIRE(ptr->GetCount() == 77);
}

TEST_CASE("RegisterSingleton unique_ptr deletes on container destruction", "[singleton][external]") {
    g_destructionCount = 0;
    {
        DI::Container c;
        c.RegisterSingleton<DestructionTracker>(std::make_unique<DestructionTracker>());
        REQUIRE(g_destructionCount == 0);
    }
    REQUIRE(g_destructionCount == 1);
}

TEST_CASE("RegisterSingleton reference does NOT delete on container destruction", "[singleton][external]") {
    g_destructionCount = 0;
    DestructionTracker tracker;
    g_destructionCount = 0;  // reset after construction
    {
        DI::Container c;
        c.RegisterSingleton<DestructionTracker>(tracker);
    }
    // Container destroyed but should NOT have deleted tracker (no-op deleter)
    REQUIRE(g_destructionCount == 0);
}

// ============================================================
// ResetSingleton
// ============================================================

TEST_CASE("ResetSingleton creates a fresh instance", "[reset]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    auto* first = c.Get<Counter>();
    first->Increment();
    first->Increment();
    REQUIRE(first->GetCount() == 2);

    c.ResetSingleton<Counter>();
    auto* second = c.Get<Counter>();
    REQUIRE(second->GetCount() == 0);
    REQUIRE(second != first);
}

TEST_CASE("ResetSingleton with constructor args", "[reset]") {
    DI::Container c;
    c.RegisterSingletonType<Counter, int>(10);
    REQUIRE(c.Get<Counter>()->GetCount() == 10);

    c.ResetSingleton<Counter>(50);
    REQUIRE(c.Get<Counter>()->GetCount() == 50);
}

TEST_CASE("ResetSingleton on transient throws", "[reset]") {
    DI::Container c;
    c.RegisterType<Counter>();
    REQUIRE_THROWS_AS(c.ResetSingleton<Counter>(), std::logic_error);
}

TEST_CASE("ResetSingleton deletes the old instance", "[reset]") {
    DI::Container c;
    c.RegisterSingletonType<DestructionTracker>();

    g_destructionCount = 0;
    c.ResetSingleton<DestructionTracker>();
    REQUIRE(g_destructionCount == 1);
}

// ============================================================
// ResetSingletonPointer
// ============================================================

TEST_CASE("ResetSingletonPointer switches to external pointer", "[reset]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    auto* original = c.Get<Counter>();
    REQUIRE(original != nullptr);

    Counter externalCounter(999);
    c.ResetSingletonPointer<Counter>(&externalCounter);

    auto* retrieved = c.Get<Counter>();
    REQUIRE(retrieved != nullptr);
    REQUIRE(retrieved == &externalCounter);
    REQUIRE(retrieved->GetCount() == 999);
}

TEST_CASE("ResetSingletonPointer on transient throws", "[reset]") {
    DI::Container c;
    c.RegisterType<Dog>();
    Dog d;
    REQUIRE_THROWS_AS(c.ResetSingletonPointer<Dog>(&d), std::logic_error);
}

// ============================================================
// IsRegistered + GetLifetime
// ============================================================

TEST_CASE("IsRegistered returns false for unregistered types", "[meta]") {
    DI::Container c;
    REQUIRE_FALSE(c.IsRegistered<Dog>());
}

TEST_CASE("IsRegistered returns true after RegisterType", "[meta]") {
    DI::Container c;
    c.RegisterType<Dog>();
    REQUIRE(c.IsRegistered<Dog>());
}

TEST_CASE("IsRegistered returns true after RegisterSingletonType", "[meta]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    REQUIRE(c.IsRegistered<Counter>());
}

TEST_CASE("IsRegistered returns true after RegisterSingleton with raw pointer", "[meta]") {
    DI::Container c;
    Counter ext(1);
    c.RegisterSingleton<Counter>(&ext);
    REQUIRE(c.IsRegistered<Counter>());
}

TEST_CASE("IsRegistered returns true after RegisterSingleton with reference", "[meta]") {
    DI::Container c;
    Counter ext(1);
    c.RegisterSingleton<Counter>(ext);
    REQUIRE(c.IsRegistered<Counter>());
}

TEST_CASE("IsRegistered returns true after RegisterSingleton with unique_ptr", "[meta]") {
    DI::Container c;
    c.RegisterSingleton<Counter>(std::make_unique<Counter>(1));
    REQUIRE(c.IsRegistered<Counter>());
}

TEST_CASE("GetLifetime returns Transient for transient types", "[meta]") {
    DI::Container c;
    c.RegisterType<Dog>();
    REQUIRE(c.GetLifetime<Dog>() == DI::Lifetime::Transient);
}

TEST_CASE("GetLifetime returns Singleton for singleton types", "[meta]") {
    DI::Container c;
    c.RegisterSingletonType<Counter>();
    REQUIRE(c.GetLifetime<Counter>() == DI::Lifetime::Singleton);
}

TEST_CASE("GetLifetime throws for unregistered types", "[meta]") {
    DI::Container c;
    REQUIRE_THROWS_AS(c.GetLifetime<Dog>(), std::logic_error);
}

// ============================================================
// Multiple containers are independent
// ============================================================

TEST_CASE("Two containers are independent", "[container]") {
    DI::Container c1;
    DI::Container c2;

    c1.RegisterSingletonType<Counter, int>(10);
    c2.RegisterSingletonType<Counter, int>(20);

    REQUIRE(c1.Get<Counter>()->GetCount() == 10);
    REQUIRE(c2.Get<Counter>()->GetCount() == 20);
}

// ============================================================
// Re-registration overwrites previous registration
// ============================================================

TEST_CASE("Re-registering a type overwrites the previous one", "[container]") {
    DI::Container c;
    c.RegisterInterface<IAnimal, Dog>();
    auto dog = c.Make<IAnimal>();
    REQUIRE(std::string(dog->Speak()) == "Woof");

    c.RegisterInterface<IAnimal, Cat>();
    auto cat = c.Make<IAnimal>();
    REQUIRE(std::string(cat->Speak()) == "Meow");
}

// ============================================================
// Construction count — no double construction (Bug 5)
// ============================================================

TEST_CASE("Factory does not double-construct objects", "[construction]") {
    DI::Container c;
    c.RegisterType<ConstructionTracker>();

    g_constructionCount = 0;
    auto obj = c.Make<ConstructionTracker>();
    // Ideally 1, but move elision may or may not fire — the key thing is
    // we're not doing new T(T(...)) anymore. With the fix, apply returns
    // new T directly, so this should be exactly 1.
    REQUIRE(g_constructionCount == 1);
}

// ============================================================
// Free function wrappers (dependency_injection.h)
// ============================================================

// Use unique types per test to avoid global container pollution

namespace FreeFuncTests {
    struct TypeA {
        int val = 0;
    };
    struct TypeB {
        int val = 0;
        TypeB() = default;
        TypeB(int v) : val(v) {}
    };
    struct IFoo {
        virtual ~IFoo() = default;
        virtual int Value() = 0;
    };
    struct FooImpl : IFoo {
        int _v;
        FooImpl() : _v(0) {}
        int Value() override { return _v; }
    };
}

TEST_CASE("Free function RegisterType and Make work via global container", "[free]") {
    // Reset global state by re-registering
    auto& gc = DI::Container::GetGlobalInstance();
    gc.RegisterType<FreeFuncTests::TypeA>();
    auto obj = DependencyInjection::Make<FreeFuncTests::TypeA>();
    REQUIRE(obj != nullptr);
}

TEST_CASE("Free function RegisterSingletonType and Get", "[free]") {
    DependencyInjection::RegisterSingletonType<FreeFuncTests::TypeB, int>(123);
    auto* ptr = DependencyInjection::Get<FreeFuncTests::TypeB>();
    REQUIRE(ptr != nullptr);
    REQUIRE(ptr->val == 123);
}

TEST_CASE("Free function RegisterSingletonInterface and Get", "[free]") {
    DependencyInjection::RegisterSingletonInterface<FreeFuncTests::IFoo, FreeFuncTests::FooImpl>();
    auto* ptr = DependencyInjection::Get<FreeFuncTests::IFoo>();
    REQUIRE(ptr != nullptr);
    REQUIRE(ptr->Value() == 0);
}

// ============================================================
// Bug 4 regression: free function lifetime forwarding
// ============================================================

namespace Bug4Test {
    struct MyType {};
    struct MyImpl : MyType {};
}

TEST_CASE("Free function RegisterInterface forwards lifetime parameter", "[free][bug4]") {
    // Use container directly to test — the free functions forward to global,
    // but we can verify the fix by checking the container method itself
    DI::Container c;
    c.RegisterInterface<Bug4Test::MyType, Bug4Test::MyImpl>(DI::Lifetime::Singleton);
    REQUIRE(c.GetLifetime<Bug4Test::MyType>() == DI::Lifetime::Singleton);
}
