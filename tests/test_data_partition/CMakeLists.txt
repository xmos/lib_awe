cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_data_partition)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_DEPENDENT_MODULES   "lib_awe"
                            "lib_xua(4.1.0)"
)

set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -Os
                                                -report
                                                # (8 + 1) * 16 + a bit
                                                -fcmdline-buffer-bytes=192
)

set(APP_INCLUDES src)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()
