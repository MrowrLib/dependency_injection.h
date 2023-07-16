#include <dependency_injection.h>

int main() {
    // For the low-level implementation, store with a std::string key
    // and you can provide a raw pointer to an object along with a key.

    // Then, the next layer:
    // templated registration method which assumes that the IInterface
    // has a static function called GetInterfaceName() which returns a const char*
    //
    // It has a unique name and you'd only use it if this is your design pattern.

    // Finally, there's the local version which should be used with care.
    // But you create a unique int for each interface and use that as the key.
    // We store them separately so they don't conflict with the std::string keys.
    //
    // Use this is you're not worried about index collisions which would
    // happen if you used this across multiple .dlls
    //
    // However: to allow that: provide a way to set a unique key
    // for the current runtime program.

    // DI.Register<ISomeService, SomeServiceImpl>();

    //
    return 0;
}
