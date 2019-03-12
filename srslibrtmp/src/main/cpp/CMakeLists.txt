cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(srs)

set(push
        push/push_rtmp.c
        push/push_rtmp.h
        push/push_native.c
        push/push_queue.h
        push/push_queue.c
        push/push_flvenc.h
        push/push_flvenc.c)

set(media
        media/MediaEncoder.cpp
        media/MediaEncoder.h)

add_library(srsPush SHARED ${push} ${media})
#target_include_directories(srsPush PRIVATE srs/include)
target_link_libraries(srsPush srsrtmp mediandk)