target("version-test")
do
    -- set_kind("shared")
    add_files("src/*.cc")
    -- add_files("src/*.rc")
    -- add_packages("spdlog")
    -- add_deps("mods")
    set_exceptions("cxx")
    -- add_cxflags("/bigobj")
    -- add_links("Shell32.lib")
    -- set_policy("build.optimization.lto", true)
    -- add_links("User32.lib", "Ole32.lib", "OleAut32.lib")
end
