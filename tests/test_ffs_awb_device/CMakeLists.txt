cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_ffs_awb_device)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_DEPENDENT_MODULES   "lib_awe")

set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -Os
                                                -report
                                                -mcmodel=large
                                                -DAWE_USE_FLASH_FILE_SYSTEM=1
                                                -lquadflash
)

set(APP_INCLUDES src)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

XMOS_REGISTER_APP()
