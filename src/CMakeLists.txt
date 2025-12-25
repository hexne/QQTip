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
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(QQTip PRIVATE
        module
)
target_link_libraries(module PRIVATE
        JsonCpp::JsonCpp
        httplib::httplib
)
