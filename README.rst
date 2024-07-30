:orphan:

lib_awe: AWE Core for xcore
###########################

:vendor: XMOS
:version: 1.0.0
:scope: General Use
:description: Audio Weaver (AWE) Core for xcore
:keywords: AudioWeaver, AWE, DSP, Audio
:devices: xcore.ai

Summary
=======

Lib_awe is a port of Audio Weaver for XMOS's powerful xcore.ai device. It contains code for
software threads which wrap the core library and provide easy interfacing to both audio streaming
components such as I2S and USB Audio as well as control interfacing to allow control and loading
of pre-built Audio Weaver designs from a host or internally from the device.

.. note::
    You must manually obtain the lib_awe.a file and place in the lib_awe/lib/xs3 directory in
    able to build applications with this software.

Features
========

  * xcore.ai port of AWE core version 8.D.8
  * Remote (via channel) access to AWE Tuning interface
  * Implementation of AWE Control Interface using the above
  * Example implementation of run-time selection of AWE layouts that are compiled into an application
  * Example integration into the XMOS USB Audio Reference Design
  * Supports 48000 Hz sample rate (will be extended in future releases)

Known Issues
============

  * The DSP Concepts AWE Flash File System assumes that erase sectors are of a constant size. The FFS uses the data partition which is beyond the boot partition and so this is generally the case, however please check your chosen flash device for this property if enabling the FFS
  * AWE_DSP_THREAD_NUM counts of less than 3 are not currently supported. This will be fixed in a future release `(#44) <https://github.com/xmos/lib_awe/issues/44>`_
  * Lib_awe requires the xcore.ai device to be run with a CPU clock of at least 700 MHz. Please use the appropriate silicon grade to support this (#63) <https://github.com/xmos/lib_awe/issues/63>`_
  * Audio Weaver code-generation produces mis-matched declaration (const) between .h and c files `(#39) <https://github.com/xmos/lib_awe/issues/39>`_
  * This release is known to be incompatible with versions of AWE core previous to 8.D.8

Host System Requirements
========================

For development a installation (and licence) of AudioWeaver is required

Required Tools
==============

  * XMOS XTC Tools: 15.3.0

Required Libraries (dependencies)
=================================

  * lib_xud (www.github.com/xmos/lib_xud)

Related Application Notes
=========================

The following application notes use this library:

  * AN020016: Integrating Audio Weaver (AWE) Core into USB Audio

Support
=======

This package is supported by XMOS Ltd. Issues can be raised against the software at: http://www.xmos.com/support

