cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_xawe_if)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_DEPENDENT_MODULES   "lib_awe" 
                            "lib_xua(4.1.0)"
)

set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -Os
                                                -report
)

set(APP_INCLUDES src)


set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

# Workaround until xcommon cmake supports mixed archive and source libs
set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(../../lib_awe/lib_build_info_sources.cmake)
list(APPEND APP_C_SRCS ../awb_files/simple_volume_InitAWB.c)



XMOS_REGISTER_APP()
message(STATUS *** ${APP_C_SRCS})
