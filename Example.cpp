#include <dependency_injection.h>

#include <iostream>

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
    virtual void Increment()  = 0;
    virtual int  GetCounter() = 0;
};

class SomeServiceImpl : public SomeService {
    int counter = 0;

public:
    SomeServiceImpl() = default;
    SomeServiceImpl(int startCount) : counter(startCount) {}

    ~SomeServiceImpl() { std::cout << "DELETE SomeServiceImpl::~SomeServiceImpl()" << std::endl; }

    void Increment() override { counter++; }
    int  GetCounter() override { return counter; }
};

int main() {
    auto x = 69;

    DependencyInjection::DIContainer container;

    container.RegisterService<SomeService, SomeServiceImpl>();

    auto& someServiceReference = container.Get<SomeService>();
    someServiceReference->Increment();
    someServiceReference->Increment();
    std::cout << "someServiceReference->GetCounter() = " << someServiceReference->GetCounter()
              << std::endl;

    // But we can also reset it so it makes a new instance
    container.ResetService<SomeService>();
    std::cout << "someServiceReference->GetCounter() = " << someServiceReference->GetCounter()
              << std::endl;  // should be 0 now, new instance of SomeServiceImpl

    // Finally, I want to support passing constructor arguments in both of these scenarios:
    // 1 - on RegisterService
    // 2 - on ResetService
    // container.RegisterService<SomeService, SomeServiceImpl>(42);
    // // or on ResetService
    // container.ResetService<SomeService>(42);

    // previous examples should still work, of course
    container.Register<CoolInterface, CoolClass>();
    container.Register<RadInterface, RadClass, int>();

    auto newCoolThing = container.New<CoolInterface>();
    newCoolThing->doCoolStuff();

    auto newRadThing = container.New<RadInterface>(42);
    newRadThing->doRadStuff();

    // Do things using the singleton
    DependencyInjection::Register<CoolInterface, CoolClass>();

    auto globalCoolThing = DependencyInjection::New<CoolInterface>();
    globalCoolThing->doCoolStuff();
}
