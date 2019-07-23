cmake_minimum_required(VERSION 3.4.1)
set(push_path ../../../../push)
set(media_path ../../../../media)
#设置生成的.so的目录
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
add_subdirectory(polarssl-1.2.19)
add_subdirectory(librtmp)
add_subdirectory(${push_path} ./push)
add_subdirectory(${media_path} ./media)

set(src push/push_rtmp.c
        jni_bridge.c)

add_library(kpush SHARED ${src})

target_include_directories(kpush PRIVATE librtmp push ${push_path} ${media_path})
target_link_libraries(kpush rtmp log push media_k)
