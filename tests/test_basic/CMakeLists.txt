cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_awe_basic)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_DEPENDENT_MODULES   "lib_awe")

set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -Os
                                                -report
                                                # (8 + 1) * 264 + a bit
                                                -fcmdline-buffer-bytes=4400
)

set(APP_INCLUDES src)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()
