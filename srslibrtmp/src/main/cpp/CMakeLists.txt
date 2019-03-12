cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(srs)

set(push
        push/push_rtmp.c
        push/push_native.c
        push/push_queue.c
        push/push_flvenc.c)

set(media
        media/MediaEncoder.cpp)

add_library(srsPush SHARED ${push} ${media})
target_include_directories(srsPush PRIVATE push media)
target_link_libraries(srsPush srsrtmp mediandk)