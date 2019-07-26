cmake_minimum_required(VERSION 3.4.1)

#project(srs_Librtmp LANGUAGES C CXX)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(fdk_path ${CMAKE_SOURCE_DIR}/../../../../fdkaac)
set(media_k_path ../../../../media)
set(push_path ../../../../push)

add_subdirectory(srs ./srs-bin)
add_subdirectory(${media_k_path} ./media_k)

add_subdirectory(${push_path} ./push)

include_directories(${push_path})
set(push_s
        srs_rtmp.c
        jni_bridge.cpp)

add_library(srsPush STATIC ${push_s})
target_include_directories(srsPush PRIVATE push srs/libs push ${media_k_path}/src)
target_link_libraries(srsPush push srsrtmp android atomic media_k)
target_compile_options(srsPush
        PRIVATE
        -std=c++11)

