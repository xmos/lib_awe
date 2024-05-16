# The TARGET variable determines what target system the application is 
# compiled for. It either refers to an XN file in the source directories
# or a valid argument for the --target option when compiling.

TARGET = xk-audio-316-qf60.xn
APP_NAME =

# The USED_MODULES variable lists other module used by the application. These
# modules will extend the SOURCE_DIRS, INCLUDE_DIRS and LIB_DIRS variables. 
# Modules are expected to be in the directory above the BASE_DIR directory.
USED_MODULES = lib_xua lib_i2c lib_awe

# Audio Class 2, Async, I2S Master, 8xInput, 8xOutput
#XCC_FLAGS= -fcomment-asm -O3 -report -lquadflash -g -DUSB_TILE=tile[0] -DADAT_TX_USE_SHARED_BUFF=1 -DQUAD_SPI_FLASH=1 -DMIXER=0 #  -fxscope

# Audio Class 2, Async, I2S Slave, 8xInput, 8xOutput I2S ONLY
XCC_FLAGS= -fcomment-asm -O3 -report -lquadflash -g -DUSB_TILE=tile[0] -DADAT_TX_USE_SHARED_BUFF=1 -DQUAD_SPI_FLASH=1 \
-DCODEC_MASTER=1 -DMIXER=0 -DI2S_CHANS_DAC=8 -DI2S_CHANS_ADC=8  -DAUDIO_CLASS=1 \
-DMIN_FREQ=48000 -DMAX_FREQ=48000 \
-DNUM_USB_CHAN_OUT=0 -DNUM_USB_CHAN_IN=0 -DI2S_ONLY=1 -DMINSIZEREL -mcmodel=large

#=============================================================================
# The following part of the Makefile includes the common build infrastructure
# for compiling XMOS applications. You should not need to edit below here.

XMOS_MAKE_PATH ?= ../..
include $(XMOS_MAKE_PATH)/xcommon/module_xcommon/build/Makefile.common
