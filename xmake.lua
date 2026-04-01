add_rules("mode.debug")

set_languages("c++20")

add_requires("catch2")

target("dependency_injection")
    set_kind("headeronly")
    add_headerfiles("include/(**.h)")
    add_includedirs("include", {public = true})

target("Example")
    set_kind("binary")
    add_files("Example.cpp")
    add_deps("dependency_injection")

target("tests")
    set_kind("binary")
    add_files("tests/*.cpp")
    add_deps("dependency_injection")
    add_packages("catch2")
