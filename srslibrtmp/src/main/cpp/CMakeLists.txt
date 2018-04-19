cmake_minimum_required(VERSION 3.4.1)

set(core
    core/srs_core_autofree.cpp
    core/srs_core_autofree.hpp
    core/srs_core_mem_watch.cpp
    core/srs_core_mem_watch.hpp
    core/srs_core_performance.cpp
    core/srs_core_performance.hpp
    core/srs_core.cpp
    core/srs_core.hpp)

set(kernel
    kernel/srs_kernel_aac.cpp
    kernel/srs_kernel_aac.hpp
    kernel/srs_kernel_buffer.cpp
    kernel/srs_kernel_buffer.hpp
    kernel/srs_kernel_codec.cpp
    kernel/srs_kernel_codec.hpp
    kernel/srs_kernel_consts.cpp
    kernel/srs_kernel_consts.hpp
    kernel/srs_kernel_error.cpp
    kernel/srs_kernel_error.hpp
    kernel/srs_kernel_file.cpp
    kernel/srs_kernel_file.hpp
    kernel/srs_kernel_flv.cpp
    kernel/srs_kernel_flv.hpp
    kernel/srs_kernel_log.cpp
    kernel/srs_kernel_log.hpp
    kernel/srs_kernel_mp3.cpp
    kernel/srs_kernel_mp3.hpp
    kernel/srs_kernel_stream.cpp
    kernel/srs_kernel_stream.hpp
    kernel/srs_kernel_ts.cpp
    kernel/srs_kernel_ts.hpp
    kernel/srs_kernel_utility.cpp
    kernel/srs_kernel_utility.hpp)

set(libs 
    libs/srs_lib_bandwidth.cpp
    libs/srs_lib_bandwidth.hpp
    libs/srs_lib_simple_socket.cpp
    libs/srs_lib_simple_socket.hpp
    libs/srs_librtmp.cpp
    libs/srs_librtmp.hpp)

set(protocol
    protocol/srs_http_stack.cpp
    protocol/srs_http_stack.hpp
    protocol/srs_protocol_buffer.cpp
    protocol/srs_protocol_buffer.hpp
    protocol/srs_protocol_json.cpp
    protocol/srs_protocol_json.hpp
    protocol/srs_protocol_kbps.cpp
    protocol/srs_protocol_kbps.hpp
    protocol/srs_raw_avc.cpp
    protocol/srs_raw_avc.hpp
    protocol/srs_rtmp_amf0.cpp
    protocol/srs_rtmp_amf0.hpp
    protocol/srs_rtmp_handshake.cpp
    protocol/srs_rtmp_handshake.hpp
    protocol/srs_rtmp_io.cpp
    protocol/srs_rtmp_io.hpp
    protocol/srs_rtmp_msg_array.cpp
    protocol/srs_rtmp_msg_array.hpp
    protocol/srs_rtmp_stack.cpp
    protocol/srs_rtmp_stack.hpp
    protocol/srs_rtmp_utility.cpp
    protocol/srs_rtmp_utility.hpp
    protocol/srs_rtsp_stack.cpp
    protocol/srs_rtsp_stack.hpp)

include_directories(core/)
include_directories(kernel/)
include_directories(libs/)
include_directories(protocol/)

add_definitions(-D__arm__)

set(srs-librtmp
   ${core}
   ${kernel}
   ${libs}
   ${protocol})
   
add_library(srsrtmp SHARED ${srs-librtmp})