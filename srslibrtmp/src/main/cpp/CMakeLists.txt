cmake_minimum_required(VERSION 3.4.1)

#project(srs_Librtmp LANGUAGES C CXX)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_subdirectory(srs)
add_subdirectory(oboe ./oboe-bin)
set(push
        push/push_rtmp.c
        push/push_queue.c
        push/push_flvenc.c)




add_library(srsPush STATIC ${push})
target_include_directories(srsPush PRIVATE push srs/libs)
target_link_libraries(srsPush srsrtmp)


set(media
        media/MediaEncoder.cpp
        jni_bridge.cpp
        media/AudioRecordEngine.cpp)

add_library(ndkmedia SHARED ${media})
target_link_libraries(ndkmedia oboe android mediandk atomic srsPush)
target_include_directories(ndkmedia PRIVATE oboe/include push)
target_compile_options(ndkmedia
        PRIVATE
        -std=c++11)
