cmake_minimum_required(VERSION 3.14)   # Drogon requires >= 3.14
project(API-Gateway CXX)

# ==============================================================================
# 1. C++ Standard Setup
# ==============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "C++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "Using C++17")
else ()
    message(STATUS "Using C++20")
endif ()

# ==============================================================================
# 2. Find Drogon
# ==============================================================================
find_package(Drogon CONFIG REQUIRED)

# ==============================================================================
# 3. Build Target
# ==============================================================================
add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)


# ==============================================================================
# 4. Source File and Directory Setup
# ==============================================================================
aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

drogon_create_views(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/views
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/models
)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${CTL_SRC}
        ${FILTER_SRC}
        ${PLUGIN_SRC}
        ${MODEL_SRC}
)

# Uncomment if you want to load views dynamically
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ==============================================================================
# 5. Tests
# ==============================================================================
add_subdirectory(test)
