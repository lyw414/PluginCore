#cmake 版本检测
cmake_minimum_required(VERSION 3.0)

#工程名
project(PluginCore)

#编译选项
set(CMAKE_CXX_STANDARD 98)

#代码路径
#add_subdirectory(./)
include_directories(${CMAKE_BINARY_DIR}/include 
    ${CMAKE_BINARY_DIR}/src/comm
)

#文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

#源码
set(SOURCES 
    ./src/comm/Log/Log.cpp
    ./src/comm/Time/Time.cpp
    ./src/comm/NodeList/NodeList.cpp
    ./src/comm/TaskPool/Thread.cpp
    ./src/comm/TaskPool/Task.cpp
    ./src/comm/MsgQueue/MsgQueue.cpp
    ./src/comm/MsgQueue/MsgQueueBasic.cpp
)

#头文件拷贝
set(INC_SOURCE_PATH
    ${CMAKE_BINARY_DIR}/include/Plugin.h
    ${CMAKE_BINARY_DIR}/include/PluginCoreAPI.h
    ${CMAKE_BINARY_DIR}/include/PluginCoreCommDefine.h
    ${CMAKE_BINARY_DIR}/include/Server.h
)

set(INC_OUT_PATH
    ${CMAKE_BINARY_DIR}/out/include
)
file(MAKE_DIRECTORY ${INC_OUT_PATH})
add_custom_target(copy_inc_files ALL COMMAND ${CMAKE_COMMAND} -E copy ${INC_SOURCE_PATH} ${INC_OUT_PATH} COMMENT "Copying files to ${INC_OUT_PATH}")

#配置文件拷贝
set(CFG_SOURCE_PATH
    ${CMAKE_BINARY_DIR}/cfg/plugin.jsn
    ${CMAKE_BINARY_DIR}/cfg/translate.jsn
)
set(CFG_OUT_PATH
    ${CMAKE_BINARY_DIR}/out/cfg
)
file(MAKE_DIRECTORY ${CFG_OUT_PATH})
add_custom_target(copy_cfg_files ALL COMMAND ${CMAKE_COMMAND} -E copy ${CFG_SOURCE_PATH} ${CFG_OUT_PATH} COMMENT "Copying files to ${CFG_OUT_PATH}")



#编译选项
#set(BUILD_TYPE "release")
set(BUILD_TYPE "debug")

if (BUILD_TYPE STREQUAL "debug")
    #target_compile_options(PluginCore PRIVATE -g -Wall -fsanitize=address -fno-omit-frame-pointer)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fsanitize=address -fno-omit-frame-pointer -lpthread -ldl")
    message(STATUS "BUILD DEBUG PROGRAM ...")
else ()
    #target_compile_options(PluginCore PRIVATE -g -Wall -fsanitize=address -fno-omit-frame-pointer)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -lpthread -ldl")
    message(STATUS "BUILD RELEASE PROGRAM ...")
endif ()

#动态库
#动态库编译
add_library(PluginCore SHARED ${SOURCES})
#动态库版本
set_target_properties(PluginCore PROPERTIES VERSION 1.0.0 SOVERSION 1)
#动态库输出目录
set_target_properties(PluginCore PROPERTIES LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})


add_custom_target(info_p ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Build Output Path: ${CMAKE_BINARY_DIR}/out"
) 


#安装
#install(TARGETS MyLibrary DESTINATION /usr/lib)  
#install(FILES ${CMAKE_SOURCE_DIR}/include/myheader.h DESTINATION /usr/include)

