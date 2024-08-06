# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import json
from pathlib import Path
import pytest
import time
import sys
import re

from awe_test_utils import awe_hid_comms, xe_demo_ffs_host, boot_partition_size, dp_with_ffs
from conftest import AweDut, AweDutNoUA, get_xtag_ids

from hardware_test_tools.AudioAnalyzerHarness import AudioAnalyzerHarness
from hardware_test_tools.Xsig import XsigOutput
from hardware_test_tools.check_analyzer_output import check_analyzer_output

@pytest.mark.hw
@pytest.mark.parametrize("awb_load_method", ["HID", "FFS"])
def test_stream_out(pytestconfig, awb_load_method, flash_ua_with_ffs):
    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)

    config = "UA" + ("" if awb_load_method == "HID" else "_FFS")

    with AweDut(adapter_dut, config) as dut:
        awe = awe_hid_comms()

        if awb_load_method == "FFS":
            awe.load_awb_from_ffs("playBasic_3thread.awb")
        else:
            awb = (
                Path(__file__).parents[1]
                / "examples"
                / "audioweaver"
                / "awb_files"
                / "playBasic_3thread.awb"
            )
            assert awb.exists()
            awe.send_awb(awb)

        fs = 48000
        duration = 30

        analyzer_dir = Path(__file__).parents[2] / "sw_audio_analyzer"
        with AudioAnalyzerHarness(
            adapter_harness, analyzer_dir, attach="xscope"
        ) as harness:
            xsig_config = Path(__file__).parent / "output_sine_2ch.json"
            assert xsig_config.exists()

            with XsigOutput(fs, None, xsig_config, dut.dev_name) as xsig_proc:
                time.sleep(duration)
                harness.terminate()
                xscope_lines = harness.proc_stdout + harness.proc_stderr

            with open(xsig_config) as file:
                xsig_json = json.load(file)

            failures = check_analyzer_output(xscope_lines, xsig_json["out"])
            fail_str = (
                "\n".join(failures)
                + f"\n\nxscope output:\n{xscope_lines}\n"
                + f"xsig output:\n{xsig_proc.proc_output}"
            )
            assert len(failures) == 0, f"Failures: {fail_str}"


# Hard-coded for I2S build analyser output check
def check_analyzer_output_i2s(xscope_lines):
    regex_matches = [r"Channel 0: Frequency 1000",
                    r"Channel 1: Frequency 2000"]
    desired_output_found = True

    for regex_match in regex_matches:
        found = False
        for line in xscope_lines:
            match = re.search(regex_match, line)
            if match:
                found = True
        if not found:
            print(f"ERROR - regex_match: {regex_match} not found.", file=sys.stderr)
            desired_output_found = False

    if not desired_output_found:
        print(f"in analyser output:\n\n{xscope_lines}", file=sys.stderr)

    return desired_output_found        


@pytest.mark.hw
@pytest.mark.parametrize("awb_load_method", ["HID", "FFS"])
def test_stream_i2s_loop(pytestconfig, awb_load_method):
    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)

    if awb_load_method == "FFS":
        pytest.skip() # FFS/I2S not a supported build config yet

    config = "I2S" + ("" if awb_load_method == "HID" else "_FFS")

    with AweDutNoUA(adapter_dut, config) as dut:
        awe = awe_hid_comms()

        if awb_load_method == "FFS":
            awe.load_awb_from_ffs("playBasic_3thread.awb")
        else:
            awb = (
                Path(__file__).parents[1]
                / "examples"
                / "audioweaver"
                / "awb_files"
                / "playBasic_3thread.awb"
            )
            assert awb.exists()
            awe.send_awb(awb)

        fs = 48000
        duration = 30

        analyzer_dir = Path(__file__).parents[2] / "sw_audio_analyzer"
        with AudioAnalyzerHarness(
            adapter_harness, analyzer_dir, attach="xscope"
        ) as harness:

            # NO xsig as we have no audio device
            time.sleep(duration)
            harness.terminate()
            xscope_lines = harness.proc_stdout + harness.proc_stderr

            check_analyzer_output_i2s(xscope_lines)
