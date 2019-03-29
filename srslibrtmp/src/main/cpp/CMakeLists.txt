cmake_minimum_required(VERSION 3.4.1)

#project(srs_Librtmp LANGUAGES C CXX)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_subdirectory(srs)
add_subdirectory(oboe ./oboe-bin)
#add_subdirectory(mp4v2)

set(push
        push/push_rtmp.c
        push/push_queue.c
        push/push_flvenc.c)


add_library(srsPush STATIC ${push})
target_include_directories(srsPush PRIVATE push srs/libs)
target_link_libraries(srsPush srsrtmp)


set(media
        media/MediaEncoder.cpp
        media/AudioRecordEngine.cpp
        media/Mp4Mux.cpp
        jni_bridge.cpp)

add_library(ndkmedia SHARED ${media})

add_library(mp4v2 SHARED IMPORTED)

set_target_properties(mp4v2 PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/mp4v2/${ANDROID_ABI}/lib/libmp4v2.so.2.0.0)

target_link_libraries(ndkmedia oboe android mediandk atomic srsPush mp4v2)
target_include_directories(ndkmedia PRIVATE oboe/include push mp4v2/${ANDROID_ABI}/include)
target_compile_options(ndkmedia
        PRIVATE
        -std=c++11)
