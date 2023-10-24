cmake_minimum_required(VERSION 3.25)
project(Weather_forecast)

set(CMAKE_CXX_STANDARD 20)

add_executable(Weather_forecast main.cpp lib/Weather_forecast.h lib/model.cpp lib/controller.cpp lib/view.cpp)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(Weather_forecast PRIVATE nlohmann_json::nlohmann_json)


FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.2) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

target_link_libraries(Weather_forecast PRIVATE cpr::cpr)


FetchContent_Declare(ftxui GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui GIT_TAG v3.0.0)

FetchContent_MakeAvailable(ftxui)

target_link_libraries(${PROJECT_NAME}
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
        )

