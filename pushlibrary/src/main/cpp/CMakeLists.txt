cmake_minimum_required(VERSION 3.4.1)

#设置生成的.so的目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
add_subdirectory(polarssl-1.2.19)
add_subdirectory(librtmp)

set(src
    rtmp_push.h
    rtmp_push.c
    rtmp_native.c
    rtmp_util.h
    rtmp_util.c)

add_library(push SHARED ${src})

find_library(lib-log log)
target_link_libraries(push rtmp ${lib-log})