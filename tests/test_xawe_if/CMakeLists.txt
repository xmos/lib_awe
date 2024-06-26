cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_xawe_if)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_DEPENDENT_MODULES   "lib_awe")

set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -Os
                                                -report
                                                -mcmodel=large
)

set(APP_INCLUDES src)


file(GLOB_RECURSE APP_C_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} src/*.c)
list(APPEND APP_C_SRCS ../awb_files/simple_volume_InitAWB.c)
message(STATUS ${APP_C_SRCS})

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)



XMOS_REGISTER_APP()
