Using AudioWeaver on XMOS XCORE devices
=======================================

In this app note we describe how to use the AudioWeaver on an XCORE device.

AudioWeaver is a tool and libraries for implementing Digitial Signal
Processing algorithms. It comprises a GUI and a set of libraries. Standard
building blocks such as filters, equalisers, echo cancellers, can be
assembled in the GUI and then executed on a device. A control library is
available that enables on-line control of the blocks.

XCORE
is a programmable multi-core device with flexible DSP and IO interfaces.
The IO interfaces can be programmed to, for example, I2S, TDM, USB, ADAT,
or S/PDIF interfaces (or indeed any other interface), and the DSP
capability can be used to operate on data that is received from or sent to
these interfaces. In addition to interfaces and DSP, XCORE devices can also
execute control code or even ML inference engines.

In this app note we discuss how to use ``lib_awe``; an XCORE library that
encompasses AudioWeaver. In particular, it shows how to integrate
``lib_awe`` in an application program. ``lib_awe`` is licensed software,
and you should contact DSP Concepts in order to obtain a license for
AudioWeaver and it's associated libraries.

For reference, we refer to the following repositories that you may want to
use:

* <https://github.com/xmos/lib_awe.git> for the library that integrates
  AudioWeaver and XCORE.

* <https://github.com/xmos/lib_xua.git> for the USB Audio library
  design

* <https://github.com/xmos/lib_i2s.git> for the I2S library

Introduction to AudioWeaver on XCORE
------------------------------------

AudioWeaver on XCORE (and indeed on other processor) conceptually comprises
four distinct parts, shown in blue in
:ref:`using_audioweaver_on_xcore_devices_awe_overview`. In the middle is
the AudioWeaver Virtual Machine. A program that efficiently executes DSP
code. This DSP Virtual Machine receives data samples from the left from the
Audio FrontEnd, processes them, and sends the processed samples back to the
same frontend. On the right, the Virtual Machine can be controlled using
any form of controls. For example, volume controls, frequency controls,
etc. These controls are implemented as physical devices on the XCORE, and
interact with the AudioWeaver block to effect the controls. The fourth part
shown at the top is the Audio Weaver Configuration, which is a schematic of
the pipeline that is implemented.

.. _using_audioweaver_on_xcore_devices_awe_overview:

.. figure:: images/awe_overview.*

            Structure of Audio Weaver

These four blocks all executed on an XCORE embedded processor, and form
part of the Embedded System (for example a head-unit), together with
external hardware such as CODECs and human interfacing devices. A fifth
block on the embedded processor can be used for tuning and development. It
implements a standard interface between AudioWeaver and the AudioWeaver GUI
over USB (other interfaces such as I2C or UART can be made available on
request). This fifth block is not operational on normal deployment, but is
used to develop the AudioWeaver configuration and inspect the operation of
AudioWeaver. Hence, the PC for tuning is not present during deployment.

The typical flow of a sample is shown in red, data may come from a device,
go to AudioWeaver, be processed, and out to another device. The sample has
been processed in accordance to the AudioWeaver configuration, and in
accordance to any volume or other control settings from the control block

Introduction to XCORE
---------------------

XCORE is a multi-threaded, multi-core processor. XCORE-300 (the third
generation xcore) contains a vector unit that enables fast and efficient
Digital Signal Processing. The XCORE is fully programmable in C with
libraries for DSP.

The AWE library, at present, enables one of the two cores of XCORE-300 to
be used for DSP, and will use the other core for IO and other tasks. This
provides for up to 6.4 GMACC/s to be used for DSP.

XCORE I/O interfaces are also implemented in software, and a rich library
of IO libraries is available. The libraries include but are not limited to
USB-Audio, I2S, S/PDIF, TDM, ADAT, and I2C.

Programming an XCORE involves two fundamental concepts:

* An XCORE processor can run up to eight concurrent *threads*. Each
  *thread* is guaranteed a minimum bandwidth on the processor.

* Threads on an XCORE communicate by means of *channels*. A channel
  consists of two *channel-ends*, data pushed into one channel-end comes out
  of the other channel-end and vice-versa.

The AWE on XCORE library
------------------------

The AWE library comprises two functions:

* One function that implements the AudioWeaver

* One function that enables data to be transported to and from AudioWeaver.

The first function creates a number of threads to perform DSP, and opens
channels to communicate with other parts of the design. The second function
uses one of the channels to communicate with the AudioWeaver block.

.. doxygenfunction:: awe_xcore_main

.. doxygenfunction:: awe_offload_data_to_dsp_engine

In order to use the functions, one needs to configure the library to use
the correct number of audio channels, threads, and heaps. To this effect,
create a file ``awe_conf.h`` in your project that defines the following values:

  =============================== ===========
  Define                          Values
  =============================== ===========
  AWE_DSP_THREAD_NUM              1..5
  AWE_INPUT_CHANNELS              0 or more
  AWE_OUTPUT_CHANNELS             0 or more
  AUDIO_INPUT_CHANNELS            0 or more
  AUDIO_OUTPUT_CHANNELS           0 or more
  AWE_BLOCK_SIZE                  32
  AWE_HEAP_SIZE                   4096
  =============================== ===========

Some values are, at present, pre-set:

  =============================== ===========
  Define                          Values
  =============================== ===========
  Sample rate                     48,000 Hz
  =============================== ===========


A worked example: integrating AudioWeaver with XMOS USB AUDIO
-------------------------------------------------------------

The XMOS USB Audio solution is extensible and allows elements such as
Digital Signal Processing to be added by the system integrator. The USB
Audio documentation contains all the details, here we just focus on adding
AudioWeaver for DSP.

Interface the data path to the USB Audio Stack
++++++++++++++++++++++++++++++++++++++++++++++

THe first step is to interface the data path to the USB Audio software
stack. The data path is the route along which audio samples coming into USB
Audio (over USB, I2S and other digital interfaces), are being processed
before being output by USB Audio (over USB, I2S and other digital
interfaces). USB Audio calls a function ``UserBufferManagement`` in order
to process input samples onto output samples:

.. literalinclude:: awe_usb.c
   :start-on: 
   :end-before: 

This function now calls the AWE convenience calls to pass the samples on to
AudioWeaver using a channel-end ``g_c_to_dspc``. This channel end needs to
be initialised, and to that extent we create a function ``dsp_main()`` that
creates the channel-end and passes it on to ``awe_xcore_main()``. Two more
channel ends are passed on that govern the tuning interface to the host.

Starting the DSP task
+++++++++++++++++++++

The dsp_main function needs to be called from our main program, and for
this we add a few lines to ``user_main.h``. First, we need to make sure
that our DSP process is started:

.. literalinclude:: extensions/user_main.h
   :start-on: Create DSP process
   :end-before: End of DSP process

Enabling tuning over a USB HID interface
++++++++++++++++++++++++++++++++++++++++

In addition, we need to create a task that deals with the AWE tuning HID
endpoint. This is the ``awe_usb_hid()`` function in the ``lib_awe`` library.

.. literalinclude:: extensions/user_main.h
   :start-on: Create HID process
   :end-before: Create DSP process

Then, we need to create the channel ends that link the HID endpoint
handler and the DSP stac together for exchaning tuning information:
    
.. literalinclude:: extensions/user_main.h
   :start-on: chan c_hid_tuning_from_host
   :end-before: USER_MAIN_CORES

Finally, we need to configure the USB Audio stack to enable HID outputs:
    
.. literalinclude:: extensions/user_main.h
   :start-on: chan c_hid_tuning_from_host
   :end-before: USER_MAIN_CORES

Separate to this, we need to provide HID descriptors for the USB stack to
extend its USB Audio descriptor to include HID IN and OUT endpoints, and
for the USB stack to describe the nature of the HID interaction. These are
in the files ``extra_hid.h`` and ``static_hid_report.h``.

Optimising memory usage
-----------------------







Control interface
-----------------

The default control interface is to call the C functions when appropriate
from a separate task. However, there are cases where this is undesirable,
for example where control is implemented from a separate system.

Optional Control through a set of registers
+++++++++++++++++++++++++++++++++++++++++++

Control can be implemented on the back of a set of registers. These
registers can be written over, for example, I2C or any other mechanism
(think A2B, Ethernet, etc), and give access to AWE's control functions. The 23
registers are numbered as follows

   ================== ========== ==== ==================================================
   Register name      Number     Perm Notes
   ================== ========== ==== ==================================================
   REG_MAGIC_VID      0x00       RO   Vendor ID, 0..255
   REG_MAGIC_PID      0x01       RO   Product ID, 0..255
   REG_VERSION_MAJOR  0x02       RO   Version number major, 0..255
   REG_VERSION_MINOR  0x03       RO   Version number minor, 0..255
   REG_BYTE_STREAM    0x04       RW   Access to a stream of bytes - see below
   REG_EXECUTE_ERROR  0x05       RW   Write triggers execute, read returns error
   REG_RESERVED       0x06       RW   Reserved
   REG_COMMAND        0x07       RW   Command to be executed
   REG_PARAM_BYTES    0x08-0x0B  RW   Byte count LSB..MSB
   REG_PARAM_A        0x0C-0x0F  RW   Parameter A LSB..MSB
   REG_PARAM_B        0x10-0x13  RW   Parameter B LSB..MSB
   REG_PARAM_C        0x14-0x17  RW   Parameter C LSB..MSB
   ================== ========== ==== ==================================================

The first four registers can be read by the host check that the device is what it is
expected to be.

Register 0x04 provides serial access to a stream of data.
It should be accessed exactly N times where N is the value written in
PARAM_BYTES.

Regiser 0x05 contains any error code of the last executed command (0x00
means no error), a write to this register will execute the currently set
command.

Register 0x06 contains the command to be executed next, with its parameters
stored in PARAM_A, PARAM_B, and PARAM_C. All the PARAM registers are
notionally 32-bit values stored in four single-byte registers, with the
first register storing the least significant byte, and the last register
storing the most significant byte. PARAM registers may contain signed
integers, unsigned integers, bit masks, or IEEE-754 floating point values
depending on the command.

The commands supported are:

  =============================== ============ ====================
  Name                            Number       Byte stream transfer
  =============================== ============ ====================
  AWE_LOAD_AWB                    0x01         write to device
  AWE_CTRL_SET_VALUE              0x02         write to device
  AWE_CTRL_GET_VALUE              0x41         read from device
  AWE_CTRL_SET_STATUS             0x81         none
  AWE_SET_PROFILING_STATUS        0x82         none
  AWE_CTRL_GET_STATUS             0x84         none
  AWE_GET_AVERAGE_LAYOUT_CYCLES   0x88         none
  AWE_GET_CHANNEL_COUNT           0x90         none
  AWE_GET_INPUT_SAMPLE_RATE       0xA0         none
  AWE_LAYOUT_IS_VALID             0xC0         none
  =============================== ============ ====================

You must always first write to the command register, then the number of
bytes (for the first three commands), and finally the PARAM registers that
need to be filled in. After that, any bytes that need to be written are
written to the BYTE_STREAM registers. Then the EXECUTE_ERROR register is
written and then read, and finally for the read-from-device commands you
must read the bytes out of the BYTE_STREAM register.

The errors are returned are single byte value enumerating the 113 errors
that can be returned. The value +1 means that an unsupported command was
sent. Zero indicates success.

Below we specify how each control call is executed using the register file.

INT32 awe_ctrlSetValue(const AWEInstance *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length);

#. Write AWE_CTRL_SET_VALUE into COMMAND
#. Write the byte count into PARAM_BYTES
#. Write the handle into PARAM_A
#. Write the arrayOffset into PARAM_B
#. Write all the values one byte at a time into BYTE_STREAM
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.

INT32 awe_ctrlGetValue(const AWEInstance *pAWE, UINT32 handle, void *value, INT32 arrayOffset, UINT32 length);

#. Write AWE_CTRL_GET_VALUE into COMMAND
#. Write the byte count into PARAM_BYTES
#. Write the Handle into PARAM_A
#. Write the arrayOffset into PARAM_B
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Read all the values one byte at a time from BYTE_STREAM

INT32 awe_ctrlSetStatus(const AWEInstance *pAWE, UINT32 handle, UINT32 status);

#. Write AWE_CTRL_SET_STATUS into COMMAND
#. Write the handle into PARAM_A
#. Write the status into PARAM_B_0 (just a single byte)
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.

INT32 awe_ctrlGetStatus(const AWEInstance *pAWE, UINT32 handle, UINT32 status);

#. Write AWE_CTRL_GET_STATUS into COMMAND
#. Write the handle into PARAM_A
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Read the status from PARAM_B_0 (just a single byte)

INT32 awe_setProfilingStatus(AWEInstance *pAWE, UINT32 status);

#. Write AWE_SET_PROFILING_STATUS into COMMAND
#. Write the profiling status into PARAM_B_0 (just a single byte)
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.

INT32 awe_getAverageLayoutCycles(AWEInstance *pAWE, UINT32 layoutIdx, UINT32 * averageCycles);

#. Write AWE_GET_AVERAGE_LAYOUT_CYCLES into COMMAND
#. Write the LayoutIndex into PARAM_A (typically 0)
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Read the average cycles from PARAM_B (four bytes)

INT32 awe_loadAWBfromArray(AWEInstance *pAWE, const UINT32 *pCommands, UINT32 arraySize, UINT32 *pPos);

#. Write AWE_LOAD_AWB into COMMAND
#. Write the byte count into PARAM_BYTES
#. Write all the values one byte at a time into BYTE_STREAM
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Optionally read PARAM_B (four bytes); this contains any information on an error position

void awe_layoutGetChannelCount(const AWEInstance *pAWE, UINT32 pinIdx, UINT32 *inCount, UINT32 *outCount);

#. Write AWE_GET_CHANNEL_COUNT into COMMAND
#. Write the pinIndx into PARAM_A (typically 0)
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Read the inCount and OutCount from the PARAM_B and PARAM_C registers
   (two times four bytes)

INT32 awe_layoutGetInputBlockSize(const AWEInstance *pAWE, UINT32 pinIdx, UINT32 *blockSize);

#. Write AWE_GET_INPUT_BLOCK_SIZE into COMMAND
#. Write the pinIndx into PARAM_A (typically 0)
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Read the block size from PARAM_B (four bytes).

INT32 awe_layoutGetInputSampleRate(const AWEInstance *pAWE, UINT32 pinIdx, FLOAT32 *sampleRate);

#. Write AWE_GET_INPUT_SAMPLE_RATE into COMMAND
#. Write the pinIndx into PARAM_A (typically 0)
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.
#. Read the block size from PARAM_B (four bytes that must be interpreted as
   a float).

INT32 awe_layoutIsValid(const AWEInstance *pAWE);

#. Write AWE_LAYOUT_IS_VALID into COMMAND
#. Write any value to the EXECUTE_ERROR register.
#. Read The EXECUTE_ERROR register back, this should contain 0 or an error value.


Optional Control over an I2C device
+++++++++++++++++++++++++++++++++++

The register file can be made available over I2C by defining
AWE_I2C_CONTROL. ``awe_main()`` requires three extra parameters in this
case: the I2C clock-port (a one bit port or a multi-bit port of which the
LSB is connected to the I2C clock); the I2C serial data-port (a one bit
port or a multi-bit port of which the LSB is connected to the I2C data);
the I2C device address, a 7-bit value.

With that defined, an I2C device is emulated on the two ports provided
allowing registers to be read and written by an I2C-host. Note that clock
stretching is used whilst the control commands are running.

Note:

* On writing, the register number is auto incremented with the excepion of the
  BYTE_STREAM and EXECUTE_ERROR registers. Hence, a block write to the
  BYTE_STREAM register can be performed to write a block in one go, and a
  17 byte block write to the command register can be performed in order to
  set the command, number of bytes, and all parameters in one operation.

* On reading, the register number is auto incremented with the excepion of the
  BYTE_STREAM register. Hence, a block read to the
  BYTE_STREAM register can be performed to read a block in one go, and a
  12 byte block read on PARAMETER_A can be performed in order to
  readb back all parameters in one operation.
