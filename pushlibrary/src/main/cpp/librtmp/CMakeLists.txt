cmake_minimum_required(VERSION 3.4.1)
#设置加载的头文件路径
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/polarssl-1.2.19/include/")


set(src
    amf.c
    amf.h
    bytes.h
    dh.h
    dhgroups.h
    hashswf.c
    http.h
    log.c
    log.h
    parseurl.c
    rtmp.c
    rtmp.h
    rtmp_sys.h
)
set(librtmp "rtmp")

#添加USE_POLARSSL宏定义
add_definitions(-DUSE_POLARSSL)

add_library(${librtmp} STATIC ${src})

#链接polarssl库
#添加libz库
find_library(z-lib z)
target_link_libraries(${librtmp} ${z-lib} polarssl)