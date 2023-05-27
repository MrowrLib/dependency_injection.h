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
public:
    void doRadStuff() override { std::cout << "RadClass::doRadStuff()" << std::endl; }
};

int main() {
    DependencyInjection::DIContainer container;

    container.Register<CoolInterface, CoolClass>();
    container.Register<RadInterface, RadClass>();

    auto newCoolThing = container.New<CoolInterface>();
    newCoolThing->doCoolStuff();

    auto newRadThing = container.New<RadInterface>();
    newRadThing->doRadStuff();
}
