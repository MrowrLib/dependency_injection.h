add_requires("void_pointer", "collections", "_Log_")

target("dependency_injection")
    set_kind("headeronly")
    add_packages("void_pointer", "collections", "_Log_", { public = true })
    add_includedirs("include", { public = true })
