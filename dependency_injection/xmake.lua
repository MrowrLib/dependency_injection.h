add_requires("void_pointer")

target("dependency_injection")
    set_kind("headeronly")
    add_packages("void_pointer")
    add_includedirs("include", {public = true})
