cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_usb_audio_awe)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_DEPENDENT_MODULES   "lib_awe(0.1.0)" 
                            "lib_xua(4.0.0)"
)

set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -O3
                                                -report
                                                -D__awe_conf_h_exists__=1
)

set(APP_INCLUDES src)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

# Workaround until xcommon cmake supports mixed archive and source libs
set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(../../lib_awe/lib_build_info_sources.cmake)



XMOS_REGISTER_APP()
