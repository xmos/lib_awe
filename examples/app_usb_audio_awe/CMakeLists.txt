cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(app_usb_audio_awe)

# Grab sw_usb_audio for XU316-MC HW setup files and deps
include(FetchContent)
set(SW_USB_AUDIO_PATH ../../../sw_usb_audio)
IF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${SW_USB_AUDIO_PATH})
    message(STATUS "Skipping fetch of: sw_usb_audio")
else()
    message(STATUS "Fetching: sw_usb_audio")
    FetchContent_Declare(
        sw_usb_audio
        GIT_REPOSITORY git@github.com:xmos/sw_usb_audio.git
        GIT_TAG        v8.1.0
        GIT_SHALLOW    FALSE
        GIT_SUBMODULES_RECURSE FALSE
        SOURCE_DIR     ${CMAKE_CURRENT_LIST_DIR}/${SW_USB_AUDIO_PATH}
    )
    FetchContent_Populate(sw_usb_audio)
endif()

set(APP_HW_TARGET xk-audio-316-mc-700.xn)

# Use sw_usb_audio deps and add lib_awe
include(${CMAKE_CURRENT_LIST_DIR}/${SW_USB_AUDIO_PATH}/deps.cmake)
list(APPEND APP_DEPENDENT_MODULES "lib_awe")

set(APP_PCA_ENABLE ON)
# Audio Class 2, Async, I2S Master, 8xInput, 8xOutput

set(APP_COMMON_BUILD_FLAGS                      -fcomment-asm
                                                -O3
                                                -report
                                                -lquadflash
                                                -g
                                                -DUSB_TILE=tile[0]
                                                -DADAT_TX_USE_SHARED_BUFF=1
                                                -DQUAD_SPI_FLASH=1
                                                -DMIXER=0)

set(APP_COMPILER_FLAGS_UA                       ${EXTRA_BUILD_FLAGS}
                                                ${APP_COMMON_BUILD_FLAGS})

set(APP_COMPILER_FLAGS_I2S                      ${EXTRA_BUILD_FLAGS}
                                                ${APP_COMMON_BUILD_FLAGS}
                                                -DCODEC_MASTER=1
                                                -DAUDIO_CLASS=1
                                                -DNUM_USB_CHAN_OUT=0
                                                -DNUM_USB_CHAN_IN=0
                                                -DI2S_ONLY=1)

set(APP_COMPILER_FLAGS_UA_STANDALONE            ${EXTRA_BUILD_FLAGS}
                                                ${APP_COMMON_BUILD_FLAGS}
                                                -DSTANDALONE=1)

set(APP_COMPILER_FLAGS_playBasic_3thread_InitAWB.c  -DCore0_InitCommands=Core0_InitCommands_Basic_3thread
                                                    -DCore0_InitCommands_Len=Core0_InitCommands_Basic_3thread_Len)

set(APP_COMPILER_FLAGS_simple_volume_InitAWB.c      -DCore0_InitCommands=Core0_InitCommands_simple_volume
                                                    -DCore0_InitCommands_Len=Core0_InitCommands_simple_volume_Len)


# Grab the app sources - we will extend these with HW setup from sw_usb_audio
file(GLOB_RECURSE APP_XC_SRCS   ${APP_ROOT_DIR}/src/*.xc)
list(APPEND APP_XC_SRCS         ${SW_USB_AUDIO_PATH}/app_usb_aud_xk_316_mc/src/extensions/audiohw.xc
                                ../../tests/awb_files/playBasic_3thread_InitAWB.c
                                ../../tests/awb_files/simple_volume_InitAWB.c)

set(APP_INCLUDES    src
                    src/core
                    src/extensions
                    ${SW_USB_AUDIO_PATH}/app_usb_aud_xk_316_mc/src/extensions
                    ${SW_USB_AUDIO_PATH}/app_usb_aud_xk_316_mc/src
                    ../../tests/awb_files)


set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../)

XMOS_REGISTER_APP()
