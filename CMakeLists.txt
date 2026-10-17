cmake_minimum_required(VERSION 3.25)

project(
    TaskManager
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(taskmanager_core STATIC
    src/core/TaskManager.cpp
)

target_include_directories(taskmanager_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_compile_features(taskmanager_core
    PUBLIC
        cxx_std_23
)

add_library(taskmanager_application STATIC
    src/application/TaskService.cpp
)

target_include_directories(taskmanager_application
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/application
)

target_link_libraries(taskmanager_application
    PUBLIC
        taskmanager_core
)

add_library(taskmanager_infrastructure STATIC
    src/infrastructure/FileTaskRepository.cpp
)

target_include_directories(taskmanager_infrastructure
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure
)

target_link_libraries(taskmanager_infrastructure
    PUBLIC
        taskmanager_application
)

add_executable(taskmanager
    src/app/main.cpp
    src/app/ConsoleApp.cpp
)

target_include_directories(taskmanager
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(taskmanager
    PRIVATE
        taskmanager_application
        taskmanager_infrastructure
)

enable_testing()

add_executable(taskmanager_tests
    tests/TaskManagerTests.cpp
)

target_link_libraries(taskmanager_tests
    PRIVATE
        taskmanager_core
)

add_test(
    NAME taskmanager_tests
    COMMAND taskmanager_tests
)

add_executable(taskservice_tests
    tests/TaskServiceTests.cpp
)

target_link_libraries(taskservice_tests
    PRIVATE
        taskmanager_application
)

add_test(
    NAME taskservice_tests
    COMMAND taskservice_tests
)

add_executable(file_repository_tests
    tests/FileTaskRepositoryTests.cpp
)

target_link_libraries(file_repository_tests
    PRIVATE
        taskmanager_infrastructure
)

add_test(
    NAME file_repository_tests
    COMMAND file_repository_tests
)
