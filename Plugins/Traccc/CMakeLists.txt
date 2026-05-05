acts_add_library(
    PluginTraccc
    src/MeasurementConversion.cpp
    src/TrackConversion.cpp
    ACTS_INCLUDE_FOLDER include/ActsPlugins
)

add_dependencies(ActsPluginTraccc traccc::core covfie::core vecmem::core)

target_include_directories(
    ActsPluginTraccc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    ActsPluginTraccc
    PUBLIC
        traccc::io
        traccc::core
        vecmem::core
)

acts_compile_headers(PluginTraccc GLOB include/**/*.hpp)
