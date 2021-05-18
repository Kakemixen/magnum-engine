set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
    DebugTools
    GL
    MeshTools
    Primitives
    Shaders
    GlfwApplication)

add_executable(basic-engine
    main.cpp
    application-layer/application.cpp)
target_link_libraries(basic-engine PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::DebugTools
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::Shaders)
target_include_directories(basic-engine PRIVATE
    "api")
