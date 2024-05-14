add_requires("minispecs", "vcpkg::snowhouse")

target("Tests")
    set_kind("binary")
    add_deps("dependency_injection")
    add_packages("minispecs", "vcpkg::snowhouse")
    add_files("*.cpp")
