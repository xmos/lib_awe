XCORE Audio Weaver Interface
============================

Lib_awe is a port of Audio Weaver for XMOS's powerful xcore.ai device. It contains code for software threads which wrap the core library and provide easy interfacing to both audio streaming components such as I2S and USB Audio as well as control interfacing to allow control and loading of pre-built Audio Weaver designs from a host or internally from the device.


.. note::
    You must manually obtain the lib_awe.a file and place in the lib_awe/lib/xs3 directory in able to build firmware with this repo.