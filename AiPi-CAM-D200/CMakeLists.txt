cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})
#增加源文件文件包
file(GLOB_RECURSE d200_camera "${CMAKE_CURRENT_SOURCE_DIR}/d200_camera/*.c")
file(GLOB_RECURSE bl618_wifi "${CMAKE_CURRENT_SOURCE_DIR}/bl618_wifi/*.c")
file(GLOB_RECURSE ap_web "${CMAKE_CURRENT_SOURCE_DIR}/ap_web/*.c")
file(GLOB_RECURSE bl616_uart "${CMAKE_CURRENT_SOURCE_DIR}/bl616_uart/*.c")
file(GLOB_RECURSE filesystem "${CMAKE_CURRENT_SOURCE_DIR}/filesystem/*.c")
file(GLOB_RECURSE button "${CMAKE_CURRENT_SOURCE_DIR}/button/*.c")

#增加头文件目录
sdk_add_include_directories(.)
sdk_add_include_directories(./chipconfig)
sdk_add_include_directories(./d200_camera)
sdk_add_include_directories(./bl618_wifi)
sdk_add_include_directories(./ap_web)
sdk_add_include_directories(./bl616_uart)
sdk_add_include_directories(./filesystem)
sdk_add_include_directories(./button)

#增加源文件目录
target_sources(app PRIVATE ${d200_camera})
target_sources(app PRIVATE ${bl618_wifi})
target_sources(app PRIVATE ${ap_web})
target_sources(app PRIVATE ${bl616_uart})
target_sources(app PRIVATE ${filesystem})
target_sources(app PRIVATE ${button})

#增加main.c源文件
sdk_set_main_file(main.c)

project(AiPi-CAM-D200)
