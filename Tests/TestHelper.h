#pragma once

#include <MiniSpecs.h>
#include <dependency_injection.h>
#include <snowhouse/snowhouse.h>

#include <string>

using namespace snowhouse;

struct IExample {
    virtual ~IExample()                                  = default;
    virtual std::string GetName()                        = 0;
    virtual void        SetName(const std::string& name) = 0;
};

class Example : public IExample {
    std::string name;

public:
    Example() = default;
    Example(const std::string& name) : name(name) {}

    ~Example() override = default;

    std::string GetName() override { return name; }
    void        SetName(const std::string& name) override { this->name = name; }
};
