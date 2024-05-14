add_requires("minispecs", "vcpkg::snowhouse", "unordered_dense", "spdlog")

target("Tests")
    set_kind("binary")
    add_deps("dependency_injection")
    add_packages("minispecs", "vcpkg::snowhouse", "unordered_dense", "spdlog")
    add_files("*.cpp")
