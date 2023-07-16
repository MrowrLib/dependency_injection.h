add_rules("mode.debug")

set_languages("c++20")

add_repositories("MrowrLib https://github.com/MrowrLib/Packages.git")
add_requires("function_pointer", "void_pointer")

target("dependency_injection")
    set_kind("headeronly")
    add_headerfiles("include/(**.h)")
    add_includedirs("include", {public = true})
    add_packages("function_pointer", "void_pointer", {public = true})

target("Example")
    set_kind("binary")
    add_files("Example.cpp", "Second Example File.cpp")
    add_includedirs(".")
    add_deps("dependency_injection")
