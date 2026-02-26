file(GLOB objects_src ../common/*.c)

target_sources(${PROJECT_NAME} PRIVATE ${objects_src})

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)
