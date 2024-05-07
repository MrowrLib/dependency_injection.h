#include <dependency_injection.h>

#include <fstream>
#include <iostream>

std::ofstream logFile("destructor_log.txt");

struct CoolInterface {
    virtual void doCoolStuff() = 0;
};

struct RadInterface {
    virtual void doRadStuff() = 0;
};

class CoolClass : public CoolInterface {
public:
    void doCoolStuff() override { std::cout << "CoolClass::doCoolStuff()" << std::endl; }
};

class RadClass : public RadInterface {
    int radNumber = 0;

public:
    RadClass(int radNumber) : radNumber(radNumber) {
        std::cout << "RadClass::RadClass(" << radNumber << ")" << std::endl;
    }

    ~RadClass() { std::cout << "DELETE RadClass::~RadClass()" << std::endl; }

    void doRadStuff() override {
        std::cout << "RadClass::doRadStuff() with number " << radNumber << std::endl;
    }
};

struct SomeService {
    virtual ~SomeService()    = default;
    virtual void Increment()  = 0;
    virtual int  GetCounter() = 0;
};

class SomeServiceImpl : public SomeService {
    int counter = 0;

public:
    SomeServiceImpl() { std::cout << "SomeServiceImpl::SomeServiceImpl()" << std::endl; }
    SomeServiceImpl(int startCount) : counter(startCount) {
        std::cout << "SomeServiceImpl::SomeServiceImpl(" << startCount << ")" << std::endl;
    }

    ~SomeServiceImpl() {
        //
        std::cout << "DELETE SomeServiceImpl::~SomeServiceImpl()" << std::endl;
        if (logFile.is_open()) {
            logFile << "DELETE SomeServiceImpl::~SomeServiceImpl() with counter " << counter
                    << std::endl;
        }
    }

    void Increment() override { counter++; }
    int  GetCounter() override { return counter; }
};

class SomeConcreteClass {
    int counter = 0;

public:
    SomeConcreteClass() { std::cout << "SomeConcreteClass::SomeConcreteClass()" << std::endl; }
    ~SomeConcreteClass() {
        std::cout << "DELETE SomeConcreteClass::~SomeConcreteClass()" << std::endl;
    }

    void Increment() { counter++; }
    int  GetCounter() { return counter; }
};

// SomeServiceImpl someServiceInstance(123);

void doThings() {
    // auto x = 69;

    DependencyInjection::Container container;

    // container.RegisterSingleton<SomeService, SomeServiceImpl>();

    // auto& someServiceReference = container.Get<SomeService>();
    // someServiceReference->Increment();
    // someServiceReference->Increment();
    // std::cout << "someServiceReference->GetCounter() = " << someServiceReference->GetCounter()
    //           << std::endl;

    // // But we can also reset it so it makes a new instance
    // container.ResetSingleton<SomeService>();
    // std::cout << "someServiceReference->GetCounter() = " << someServiceReference->GetCounter()
    //           << std::endl;  // should be 0 now, new instance of SomeServiceImpl

    // // Finally, I want to support passing constructor arguments in both of these scenarios:
    // // 1 - on RegisterSingleton
    // // 2 - on ResetSingleton
    // // container.RegisterSingleton<SomeService, SomeServiceImpl>(42);
    // // // or on ResetSingleton
    // // container.ResetSingleton<SomeService>(42);

    // // previous examples should still work, of course
    // container.RegisterType<CoolClass>();
    // // container.Register<CoolInterface, CoolClass>();
    // container.RegisterInterface<RadInterface, RadClass, int>();

    // auto newCoolThing = container.Make<CoolClass>();
    // // auto newCoolThing = container.Make<CoolInterface>();
    // newCoolThing->doCoolStuff();

    // auto newRadThing = container.Make<RadInterface>(42);
    // newRadThing->doRadStuff();

    // // Do things using the singleton
    // DI::RegisterSingleton<CoolInterface, CoolClass>();

    // auto& globalCoolThing = DI::Get<CoolInterface>();
    // globalCoolThing->doCoolStuff();

    // Test passing an instance...
    // SomeServiceImpl someServiceInstance(123);
    // DI::RegisterSingleton<SomeServiceImpl>(someServiceInstance);

    // auto& someServiceReference = DI::Get<SomeServiceImpl>();
    // std::cout << "someServiceReference->GetCounter() = " << someServiceReference->GetCounter()
    //           << std::endl;

    // auto instance = std::unique_ptr<SomeService>(new SomeServiceImpl{123});
    // DI::RegisterSingleton<SomeService>(std::move(instance));

    // auto& someServiceReference = DI::Get<SomeService>();
    // std::cout << "someServiceReference->GetCounter() = " << someServiceReference->GetCounter()
    //           << std::endl;

    // DI::RegisterSingleton<SomeConcreteClass>(std::make_unique<SomeConcreteClass>());
    // auto& someServiceReference = DI::Get<SomeConcreteClass>();
    // someServiceReference->Increment();

    // DI::RegisterSingletonType<RadClass>(42);
    // auto& radThing = DI::Get<RadClass>();
    // radThing->doRadStuff();

    DI::RegisterSingletonInterface<RadInterface, RadClass>(42);
    auto& radThing = DI::Get<RadInterface>();
    radThing->doRadStuff();

    // DI::RegisterSingletonInterface<CoolInterface, CoolClass>();
    // auto& coolThing = DI::Get<CoolInterface>();
    // coolThing->doCoolStuff();
}

int main() {
    try {
        doThings();

        // The some service I registered should be deleted here...
        // Let's try to get it anyway
        // auto& someServiceReference = DI::Get<SomeServiceImpl>();

        // // This should throw an exception
        // someServiceReference->Increment();

        // std::cout << "someServiceReference->GetCounter() = " <<
        // someServiceReference->GetCounter()
        //           << std::endl;

        // DI::ResetSingleton<SomeService>();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
