:orphan:

###########################
lib_awe: AWE Core for xcore
###########################

:vendor: XMOS
:version: 1.0.1
:scope: General Use
:description: Audio Weaver (AWE) Core for xcore
:category: Audio
:keywords: Audio Weaver, AWE, DSP, Audio
:devices: xcore.ai

Summary
*******

lib_awe is a port of Audio Weaver Embedded (AWE) Core for XMOS's powerful xcore.ai device. It
contains code for software threads which wrap the core library and provide easy interfacing to
both audio streaming components such as I2S and USB Audio as well as control interfacing to
allow control and loading of pre-built Audio Weaver designs from a host or internally from the
device.

.. note::
    You must manually obtain the `libAWECore.a <https://www.xmos.com/file/libawecore_a>`_ file and
    place in the lib_awe/lib/xs3a directory inable to build applications with this software.

Features
********

  * xcore.ai port of AWE core version 8.D.8
  * Remote (via channel) access to AWE Tuning interface
  * Implementation of AWE Control Interface using the above
  * Example implementation of run-time selection of AWE layouts that are compiled into an application
  * Supports 48000 Hz sample rate

Known Issues
************

  * The DSP Concepts AWE Flash File System assumes that erase sectors are of a constant size. The FFS uses the data partition which is beyond the boot partition and so this is generally the case, however please check your chosen flash device for this property if enabling the FFS
  * This release is known to be incompatible with versions of AWE core previous to 8.D.8
  * AWE_DSP_THREAD_NUM counts of less than 3 are not currently supported `(#44) <https://github.com/xmos/lib_awe/issues/44>`_
  * The example layout provided (playBasic_3thread) requires the xcore.ai device to be clocked at least 700 MHz. Please use the appropriate silicon grade to support this `(#63) <https://github.com/xmos/lib_awe/issues/63>`_
  * Audio Weaver Designer code-generation produces mis-matched declaration (const) between .h and c files `(#39) <https://github.com/xmos/lib_awe/issues/39>`_
  * Currently xscope cannot be using alongside lib_awe `(#55) <https://github.com/xmos/lib_awe/issues/55>`_
  * Currently the USB HID tuning interface cannot be used if the Thesycon TUSBAudio driver is
    installed against the device that doesn't expose a UAC interface `(#69) <https://github.com/xmos/lib_awe/issues/69>`_

Host System Requirements
************************

  * For development a installation (and licence) of Audio Weaver Designer is required

Required Tools
**************

  * XMOS XTC Tools: 15.3.0

Required Libraries (dependencies)
*********************************

  * lib_xud (www.github.com/xmos/lib_xud)

Related Application Notes
*************************

The following application notes use this library:

  * `AN02016: Integrating Audio Weaver (AWE) Core into USB Audio <https://www.xmos.com/file/an02016>`_

Support
*******

This package is supported by XMOS Ltd. Issues can be raised against the software at: http://www.xmos.com/support

