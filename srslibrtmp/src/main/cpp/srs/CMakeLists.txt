cmake_minimum_required(VERSION 3.4.1)
project(SRS)
set(core
        core/srs_core_autofree.cpp
        core/srs_core_mem_watch.cpp
        core/srs_core_performance.cpp
        core/srs_core.cpp)

set(kernel
        kernel/srs_kernel_aac.cpp
        kernel/srs_kernel_buffer.cpp
        kernel/srs_kernel_codec.cpp
        kernel/srs_kernel_consts.cpp
        kernel/srs_kernel_error.cpp
        kernel/srs_kernel_file.cpp
        kernel/srs_kernel_flv.cpp
        kernel/srs_kernel_log.cpp
        kernel/srs_kernel_mp3.cpp
        kernel/srs_kernel_stream.cpp
        kernel/srs_kernel_ts.cpp
        kernel/srs_kernel_utility.cpp)

set(libs
        libs/srs_lib_bandwidth.cpp
        libs/srs_lib_simple_socket.cpp
        libs/srs_librtmp.cpp)

set(protocol
        protocol/srs_http_stack.cpp
        protocol/srs_protocol_buffer.cpp
        protocol/srs_protocol_json.cpp
        protocol/srs_protocol_kbps.cpp
        protocol/srs_raw_avc.cpp
        protocol/srs_rtmp_amf0.cpp
        protocol/srs_rtmp_handshake.cpp
        protocol/srs_rtmp_io.cpp
        protocol/srs_rtmp_msg_array.cpp
        protocol/srs_rtmp_stack.cpp
        protocol/srs_rtmp_utility.cpp
        protocol/srs_rtsp_stack.cpp)


#根据不同的平台定义不同的宏
if (${ANDROID_ABI} STREQUAL x86_64)
    add_definitions(-D__x86_64__)
elseif (${ANDROID_ABI} STREQUAL x86)
    add_definitions(-D__i386__)
else ()
    add_definitions(-D__arm__)
endif ()

add_definitions(-DSRS_DISABLE_LOG)
set(srs-librtmp
        ${core}
        ${kernel}
        ${libs}
        ${protocol})

add_library(srsrtmp STATIC ${srs-librtmp})

target_include_directories(srsrtmp
        PRIVATE core kernel protocol
        PUBLIC libs)

find_library(lib-log log)
target_link_libraries(srsrtmp ${lib-log})