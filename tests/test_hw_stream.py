# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import json
from pathlib import Path
import pytest
import time

from awe_test_utils import awe_hid_comms
from conftest import AweDut, get_xtag_ids

from hardware_test_tools.AudioAnalyzerHarness import AudioAnalyzerHarness
from hardware_test_tools.Xsig import XsigOutput
from hardware_test_tools.check_analyzer_output import check_analyzer_output


@pytest.mark.hw
@pytest.mark.parametrize("awb_load_method", ["hid", "ffs"])
def test_stream_out(pytestconfig):
    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)

    with AweDut(adapter_dut, "UA" if awb_load_method == "hid" else "UA_FFS") as dut:
        awe = awe_hid_comms()
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
