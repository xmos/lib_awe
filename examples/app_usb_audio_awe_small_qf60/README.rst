XMOS xCORE-AI USB Audio AWE example
===================================

:scope: Example
:description: USB Audio application with AWE stack
:keywords: USB, UAC, AudioWeaver
:boards: XCORE-AI MC AUDIO (rev 1.0)

Overview
........

The firmware provides a high-speed USB Audio device designed to be
compliant to version 2.0 of the USB Audio Class Specification based
on the XMOS AI device. All audio is passed through an Audio Weaver DSP stack.


Key Features
............

The application is designed to run on the xCORE-AI MC Audio Board
in. It uses the XMOS USB Audio framework to implement a USB Audio
device with the following key features:

- USB Audio Class 1.0/2.0 Compliant

- Fully Asynchronous operation

- 8 channels analogue input and 8 channels analogue output (Via I2S to 4 x Stereo DACs and 2 x Quad-channel ADCs)


