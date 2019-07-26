cmake_minimum_required(VERSION 3.4.1)

set(media_path ${CMAKE_SOURCE_DIR}/../../../../media)
set(push_path ${CMAKE_SOURCE_DIR}/../../../../push)

include_directories(push ${push_path} librtmp ${media_path}/src)

add_subdirectory(${media_path} ./media)
add_subdirectory(push ./push )

add_library(kpush SHARED jni_bridge.cpp)
target_link_libraries(kpush log spush media_k)
