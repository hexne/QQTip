file(GLOB module_files "*.cppm")

add_library(module STATIC)
target_sources(module
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${module_files}
)

add_executable(${PROJECT_NAME}
        main.cpp
)
find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED CONFIG)
find_package(ixwebsocket CONFIG REQUIRED)
set(NLOHMANN_JSON_BUILD_MODULES ON)
target_link_libraries(QQTip PRIVATE
        module
)
target_link_options(QQTip PRIVATE -static-libstdc++ -static-libgcc)
target_link_libraries(module PRIVATE
        nlohmann_json::nlohmann_json
        httplib::httplib
        ixwebsocket::ixwebsocket
)
