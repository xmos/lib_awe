# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Runs audio through a basic design and checks for glitching
"""

import pytest
from pathlib import Path
import os, sys
sw_usb_audio_test_path = "../../sw_usb_audio/tests"
sys.path.append(os.path.join(os.path.dirname(__file__), sw_usb_audio_test_path))
from usb_audio_test_utils import (
    check_analyzer_output,
    get_xtag_dut_and_harness,
    AudioAnalyzerHarness,
    XrunDut,
    XsigInput,
    XsigOutput,
    xsig_completion_time_s
)
from conftest import list_configs, get_config_features, board_configs
from test_analogue import analogue_duration

@pytest.mark.parametrize(["board", "config"], list_configs())
def test_output(pytestconfig, board, config):
    features_tmp = get_config_features(board, config)
    features = features_tmp.copy()

    xsig_config = "mc_analogue_output_2ch"
    xsig_config_path = Path(__file__).parent / sw_usb_audio_test_path / "xsig_configs" / f"{xsig_config}.json"

    board = "xk_316_mc"
    adapter_dut, adapter_harness = get_xtag_dut_and_harness(pytestconfig, board)
    # adapter_dut, adapter_harness = get_xtag_dut_and_harness(pytestconfig, board)

    short_test = features["partial"] or board == "xk_316_mc"
    duration = analogue_duration(pytestconfig.getoption("level"), short_test)

    print("$$$$$", board, config, adapter_dut)
    print("-----", board_configs)

    fail_str = ""

    adapter_dut = "kjhkjhkjh"

    #    subprocess.run(["xrun", "--adapter-id", self.adapter_id, firmware], timeout=self.timeout)
    #    def __init__(self, adapter_id, board, config, timeout=30):

    # Override stuff
    xrun_dut = XrunDut(adapter_dut, board, config)
    xrun_dut.firmware_override = "../app_usb_audio_awe/bin/UA/app_usb_audio_awe_UA.xe"
    xrun_dut.config = "2"
    xrun_dut.adapter_id = "BJ7H71BA"

    with xrun_dut as dut:
        for fs in features["samp_freqs"]:
            # Issue 120
            if (
                platform.system() == "Windows"
                and board == "xk_316_mc"
                and config == "2AMi8o8xxxxxx_winbuiltin"
                and fs in [44100, 48000]
            ):
                continue

            dut.set_stream_format("output", fs, features["chan_o"], 24)

            with (
                AudioAnalyzerHarness(adapter_harness, xscope="io") as harness,
                XsigOutput(fs, None, xsig_config_path, dut.dev_name),
            ):
                time.sleep(duration)
                harness.terminate()
                xscope_lines = harness.get_output()

            with open(xsig_config_path) as file:
                xsig_json = json.load(file)
            failures = check_analyzer_output(xscope_lines, xsig_json["out"])
            if len(failures) > 0:
                fail_str += f"Failure at sample rate {fs}\n"
                fail_str += "\n".join(failures) + "\n\n"
                fail_str += f"xscope stdout at sample rate {fs}\n"
                fail_str += "\n".join(xscope_lines) + "\n\n"

    if len(fail_str) > 0:
        pytest.fail(fail_str)

