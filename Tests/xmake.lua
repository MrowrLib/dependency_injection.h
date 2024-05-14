add_repositories("MrowrLib https://github.com/MrowrLib/Packages.git")

add_requires("minispecs", "vcpkg::snowhouse")

target("Tests")
    set_kind("binary")
    add_packages("minispecs", "vcpkg::snowhouse")
    add_files("*.cpp")
