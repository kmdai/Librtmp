cmake_minimum_required(VERSION 3.4.1)

#设置生成的.so的目录
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
add_subdirectory(polarssl-1.2.19)
add_subdirectory(librtmp)

set(src
        push/push_rtmp.c
        push/push_native.c
        push/push_queue.c
        push/push_flvenc.c)

add_library(push SHARED ${src})

target_include_directories(push PRIVATE librtmp)

find_library(lib-log log)
target_link_libraries(push rtmp ${lib-log})
