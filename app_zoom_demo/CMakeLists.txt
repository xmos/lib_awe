cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_zoom_demo)

set(APP_HW_TARGET xk-audio-316-mc.xn)
set(APP_DEPENDENT_MODULES "lib_xua(4.0.0)"
                          "lib_i2c(6.2.0)"
                          "lib_awe(0.1.0)")

set(APP_PCA_ENABLE ON)
# Audio Class 2, Async, I2S Master, 8xInput, 8xOutput
set(APP_COMPILER_FLAGS ${EXTRA_BUILD_FLAGS}     -fcomment-asm
                                                -O3
                                                -report
                                                -lquadflash
                                                -g
                                                -DUSB_TILE=tile[0]
                                                -DADAT_TX_USE_SHARED_BUFF=1
                                                -DQUAD_SPI_FLASH=1
                                                -DCODEC_MASTER=1
                                                -DMIXER=0
                                                -DI2S_CHANS_DAC=8
                                                -DI2S_CHANS_ADC=8
                                                -DAUDIO_CLASS=2
                                                -DNUM_USB_CHAN_OUT=8
                                                -DNUM_USB_CHAN_IN=8
)

set(APP_INCLUDES src src/core src/extensions)

# Workaround until xcommon cmake supports mixed archive and source libs
set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(../lib_awe/lib_build_info_sources.cmake)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

XMOS_REGISTER_APP()
