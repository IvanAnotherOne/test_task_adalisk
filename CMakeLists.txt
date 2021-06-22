cmake_minimum_required(VERSION 3.8)
set(PROJECT_NAME test_task_adalisk)

project(${PROJECT_NAME})

# Boost needed
set(BOOST_JSON_STANDALONE ON)
find_package(Boost COMPONENTS graph json REQUIRED)

# Define executable
add_executable(${PROJECT_NAME} main.cpp)
# Use c++17
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
