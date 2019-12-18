# test folder
set(app_name Test)
add_executable(${app_name} main.cpp)

target_link_libraries(${app_name} PUBLIC ${library_name})

target_include_directories(${app_name} PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}"
                          )
