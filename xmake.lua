add_rules("mode.debug", "mode.release")

set_languages("c++20")

includes("dependency_injection/xmake.lua")
includes("Tests/xmake.lua")
