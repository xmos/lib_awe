
# This variable should contain a space separated list of all
# the directories containing buildable applications (usually
# prefixed with the app_ prefix)
#
# If the variable is set to "all" then all directories that start with app_
# are built.
BUILD_SUBDIRS = ./examples/app_usb_audio_awe \
				./examples/app_usb_audio_awe_small \
				./examples/app_usb_audio_awe_small_qf60 \
				./examples/app_zoom_demo

XMOS_MAKE_PATH ?= ..
include $(XMOS_MAKE_PATH)/xcommon/module_xcommon/build/Makefile.toplevel
