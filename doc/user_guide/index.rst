AWE Library User Guide
======================


Introduction
------------

Lib_awe is a port of DSP Concept's Audio Weaver (AWE) for XMOS's powerful xcore.ai device. It contains code for software threads which wrap the core library and provide easy interfacing to both audio streaming components such as I2S and USB Audio as well as control interfacing to allow control and loading of pre-built designs from a host or internally from the device.

It utilises xcore.ai's multi-threaded architecture and vector processing unit to provide very high performance and predictable timing required by embedded systems.

.. note::
    This document refers to the XMOS specific implementation details. DSP Concepts provide several documents on the usage and integration of Audio Weaver into user system. Please refer to https://documentation.dspconcepts.com for documentation specific to Audio Weaver.


Architecture
------------

Lib_awe provides an interface to the audio streaming and control functions using XCORE channels which allow placement of the application blocks on different tiles from lib_awe.

.. figure:: ./images/lib_awe.png
   :width: 100%

   lib_awe thread diagram


Lib_Awe consists of a group of threads. There are a statically define number (maximum 5) of DSP worker threads which perform the AWE core functionality.

To support audio streaming an audio transport thread provides a channel interface to the Audio Weaver awe_audioImportSamples() and awe_audioExportSamples() functions. The purpose of this thread is to simplify connection to XMOS audio streaming components and user application logic and allows placement of the user selected application logic on different tiles.

Finally, a tuning thread is provided which abstracts away the awe_packetProcess() function calls and provides a channel API and again provides a channel based interface allowing placement of control to be on a different tile.

All of the above threads for the core lib_awe need to be placed on the same tile. Since the majority of one tile's RAM and threads are used by lib_awe it is typical to dedicate one tile to lib_awe and use the other tile for application logic. However, low-memory usage tasks such as I2S may also be placed on the lib_awe tile (when required by hardware IO constraints) and this is demonstrated in the USB Audio Example.


Lib_awe API
-----------

A single function is provided to wrap the entire lib_awe implementation and automatically spawns all of the worker and helper threads. In addition, where USB/HID is used as the control interface, an API is provided which takes care of translating messages to and from the HID endpoint and to and from the lib_awe tuning thread.

.. doxygengroup:: lib_awe
    :content-only:

USB Audio Example
-----------------

A sample application is provided called ``app_usb_audio_awe``. This is based on XMOS USB Audio reference design and associate XK-AUDIO-316-MC hardware. It is very closely related to the standard USB Audio reference design provided by XMOS. Documentation for this can be found here `sw_usb_audio design guide <https://www.xmos.com/download/sw_usb_audio:-sw_usb_audio-(user-guide)(v8_1_0).pdf>`_ 

DSP Concepts provide a helpful setup guide which can be seen here :download:`pdf <../pdf/User_Guide_for_XMOS_EVK_with_AWE.pdf>` which is designed to help you get up and running as quickly as possible.

The rest of this section focusses on the feature set of this AWE/USB Audio example.

The feature set of the demonstration is as follows:

    - USB Audio Class 1.0 (Full Speed)
    - Stereo input from the host to stereo output on the OUT 1/2 3.5 mm analog jack
    - Audio from the host is pumped through the AWE framework before being played on the output jack
    - 24b Sample resolution
    - 48 kHz sample rate
    - Control to AWE provided over USB HID with VID 0x20b1 and PID 0x0018 supporting live tuning

Building the Example
....................


