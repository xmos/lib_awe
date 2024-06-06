cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_usb_audio_awe)

set(APP_HW_TARGET xk-audio-316-mc-700.xn)
set(APP_DEPENDENT_MODULES "lib_xua(4.1.0)"
                          "lib_i2c(6.2.0)"
                          "lib_awe")

# Grab sw_usb_audio for XU316-MC HW setup files
include(FetchContent)

set(SW_USB_AUDIO_PATH ${CMAKE_CURRENT_LIST_DIR}/../../sw_usb_audio)
IF(EXISTS ${SW_USB_AUDIO_PATH})
    message(STATUS "Skipped: sw_usb_audio")
else()
    message(STATUS "Fetching: sw_usb_audio")
    FetchContent_Declare(
        sw_usb_audio
        GIT_REPOSITORY git@github.com:xmos/sw_usb_audio.git
        GIT_TAG        v8.1.0
        GIT_SHALLOW    FALSE
        GIT_SUBMODULES_RECURSE TRUE
        SOURCE_DIR     ${SW_USB_AUDIO_PATH}/../../sw_usb_audio
    )
    FetchContent_Populate(sw_usb_audio)
endif()


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

# Grab the app sources - we will extend these with HW setup from sw_usb_audio
file(GLOB_RECURSE APP_XC_SRCS ${APP_ROOT_DIR}/src/*.xc)
list(APPEND APP_XC_SRCS ../../sw_usb_audio/app_usb_aud_xk_316_mc/src/extensions/audiohw.xc)
message(STATUS ${APP_XC_SRCS})


set(APP_INCLUDES    src
                    src/core
                    src/extensions
                    ../../sw_usb_audio/app_usb_aud_xk_316_mc/src/extensions
                    ../../sw_usb_audio/app_usb_aud_xk_316_mc/src)

set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)

# Workaround until xcommon cmake supports mixed archive and source libs
set(APP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(../lib_awe/lib_build_info_sources.cmake)


XMOS_REGISTER_APP()
