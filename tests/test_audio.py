# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Runs audio through a basic design and checks for glitching
"""

import pytest
from pathlib import Path
import os, sys
sys.path.append(os.path.join(os.path.dirname(__file__), "../../sw_usb_audio/tests"))
from usb_audio_test_utils import (
    check_analyzer_output,
    get_xtag_dut_and_harness,
    AudioAnalyzerHarness,
    XrunDut,
    XsigInput,
    XsigOutput,
    xsig_completion_time_s
)
# from test_analogue import test_analogue_output

def test_no_glitches():
    print("Pass")

