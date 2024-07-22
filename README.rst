XCORE Audio Weaver Interface
############################

:Version: 0.1.0
:Vendor: XMOS
:Scope: Demonstration

Summary
*******

Lib_awe is a port of Audio Weaver for XMOS's powerful xcore.ai device. It contains code for
software threads which wrap the core library and provide easy interfacing to both audio streaming
components such as I2S and USB Audio as well as control interfacing to allow control and loading
of pre-built Audio Weaver designs from a host or internally from the device.

.. note::
    You must manually obtain the lib_awe.a file and place in the lib_awe/lib/xs3 directory in
    able to build firmware with this repo.

Features
========

  * xcore.ai port of AWE core
  * Remote (via channel) access to AWE Tuning interface
  * Implementation of AWE Control Inteface using the above
  * Example implementation of run-time selection of AWE layouts that are compiled into an application
  * Example integration into the XMOS USB Audio Reference Design

Host System Requirements
========================

For development a installation (and licence) of AudioWeaver is required

Documentation
=============

You can find the documentation for this software in the /doc directory of the package.

Related Application Notes
=========================

The following application notes use this library:

  * AN020016: XMOS xcore.ai USB Audio AWE Examples

Required Software (dependencies)
================================

  * lib_xud (www.github.com/xmos/lib_xud)
  * xcommon_cmake (www.github.com/xmos/xcommon_xmake) v1.2

Documentation
=============

You can find the documentation for this software in the /doc directory of the package.

Support
=======

This package is supported by XMOS Ltd. Issues can be raised against the software at: http://www.xmos.com/support

Known Issues
============

  * The DSP Concepts AWE Flash File System assumes that erase sectors are of a constant size. The FFS uses the data partition which is beyond the boot partition and so this is generally the case, however please check your chosen flash device for this property if enabling the FFS.
  * AWE_DSP_THREAD_NUM counts of less than 3 are not currently supported. This will be fixed in a future release. https://github.com/xmos/lib_awe/issues/44
  * Lib_awe requires the xcore.ai device to be run with a CPU clock of at least 700 MHz. Please use the appropriate silicon grade to support this.
  * Audio Weaver code-generation produces mis-matched declaraition (const) between .h and c files (https://github.com/xmos/lib_awe/issues/39)

