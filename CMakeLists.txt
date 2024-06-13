cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(lib_awe)

add_subdirectory(examples/app_usb_audio_awe)
add_subdirectory(examples/app_usb_audio_awe_small)
add_subdirectory(examples/app_usb_audio_awe_small_qf60)
add_subdirectory(examples/app_zoom_demo)

