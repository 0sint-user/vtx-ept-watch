cmake_minimum_required(VERSION 3.20)
project(vtx_ept_watch VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ept_audit_core STATIC
    src/ept_audit.cpp
)
target_include_directories(ept_audit_core PUBLIC include)
if(MSVC)
    target_compile_options(ept_audit_core PRIVATE /W4 /WX)
else()
    target_compile_options(ept_audit_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(vtx-ept-audit src/cli/main.cpp)
target_link_libraries(vtx-ept-audit PRIVATE ept_audit_core)

enable_testing()
add_executable(test_ept_audit tests/test_ept_audit.cpp)
target_link_libraries(test_ept_audit PRIVATE ept_audit_core)
add_test(NAME test_ept_audit COMMAND test_ept_audit)
