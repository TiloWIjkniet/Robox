file(GLOB objects_src *.c)

target_sources(${PROJECT_NAME} PRIVATE ${objects_src})

configure_file(./include/config.h.in ./include/config.h)

target_include_directories(${PROJECT_NAME} PRIVATE . ${CMAKE_CURRENT_BINARY_DIR})
