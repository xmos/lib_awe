cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_usb_audio_awe)

set(APP_HW_TARGET xk-audio-316-mc-700.xn)
set(APP_DEPENDENT_MODULES "lib_xua(4.0.0)"
                          "lib_i2c(6.2.0)"
                          "lib_awe")

set(APP_PCA_ENABLE ON)
# Audio Class 2, Async, I2S Master, 8xInput, 8xOutput
set(APP_COMPILER_FLAGS_UA ${EXTRA_BUILD_FLAGS}  -fcomment-asm
                                                -O3
                                                -report
                                                -lquadflash
                                                -g
                                                -DUSB_TILE=tile[0]
                                                -DADAT_TX_USE_SHARED_BUFF=1
                                                -DQUAD_SPI_FLASH=1
                                                -DMIXER=0
                                                -D__awe_conf_h_exists__=1
)

set(APP_COMPILER_FLAGS_I2S ${EXTRA_BUILD_FLAGS} -fcomment-asm
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
                                                -DAUDIO_CLASS=1
                                                -DNUM_USB_CHAN_OUT=0
                                                -DNUM_USB_CHAN_IN=0
                                                -DI2S_ONLY=1
                                                -D__awe_conf_h_exists__=1
)

set(APP_INCLUDES src src/core src/extensions)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

# Workaround until xcommon cmake supports mixed archive and source libs
set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(../lib_awe/lib_build_info_sources.cmake)


XMOS_REGISTER_APP()
