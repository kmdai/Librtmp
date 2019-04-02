cmake_minimum_required(VERSION 3.4.1)
project(SRS)

aux_source_directory(core source_files)
aux_source_directory(kernel source_files)
aux_source_directory(libs source_files)
aux_source_directory(protocol source_files)

#根据不同的平台定义不同的宏
if (${ANDROID_ABI} STREQUAL x86_64)
    add_definitions(-D__x86_64__)
elseif (${ANDROID_ABI} STREQUAL x86)
    add_definitions(-D__i386__)
else ()
    add_definitions(-D__arm__)
endif ()

add_definitions(-DSRS_DISABLE_LOG)

add_library(srsrtmp STATIC ${source_files})

target_include_directories(srsrtmp
        PRIVATE core kernel protocol
        PUBLIC libs)


find_library(lib-log log)

target_link_libraries(srsrtmp ${lib-log})

target_compile_options(srsrtmp
        PRIVATE
        -std=c++03)