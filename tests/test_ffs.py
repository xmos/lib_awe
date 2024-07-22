# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Tests the FFS callbacks using the app in test_ffs
"""

import json
import pytest
from pathlib import Path
import struct
import time

from awe_test_utils import flash_xe, run_xe_hw, awe_hid_comms
from conftest import AweDut, get_xtag_ids

from hardware_test_tools.AudioAnalyzerHarness import AudioAnalyzerHarness
from hardware_test_tools.Xsig import XsigOutput
from hardware_test_tools.check_analyzer_output import check_analyzer_output

xe_ffs_rpc = "test_ffs_rpc/bin/test_ffs_rpc.xe"
xe_demo_ffs_host = "../examples/app_usb_audio_awe/bin/UA_FFS/app_usb_audio_awe_UA_FFS.xe"
xe_ffs_rpc_device = "test_ffs_awb_device/bin/test_ffs_awb_device.xe"


def test_xawe_ffs_rpc():
    """
    This is a executable that needs to run on HW with flash and tests the individual RPC calls.
    The result is returned in the errorcode from the xrun instance.
    Note this will leave the data partition with junk in it.
    """
    
    stdout = run_xe_hw(xe_ffs_rpc, ["--io"]) # This will assert if it fails
    print(stdout)


@pytest.fixture
def flash_ua_with_ffs(scope="session"):
    """
    This test programs the UA/FFS binary and the pre-made FFS containing a couple of AWBS.
    """

    stdout = flash_xe(xe_demo_ffs_host, boot_partition_size=0x80000, data_partition_bin="../examples/audioweaver/awb_files/data_partition_ffs.bin")



def test_load_awb_from_ffs_host(flash_ua_with_ffs):
    """
    This test programs the UA/FFS binary and the pre-made FFS containing a couple of AWBS.
    It then uses a host command to load these many times to see if it works
    """
    
    time.sleep(5) # Wait for HID to come up

    awe = awe_hid_comms()

    # hammer the loading mechanism 400 times
    for i in range(200):
        assert awe.load_awb_from_ffs("playBasic_3thread.awb")
        assert awe.load_awb_from_ffs("simple_volume.awb")


    # NOW stream some audio. simple_volume should be loaded and will pass audio straight through
    fs = 48000
    duration = 30

    analyzer_dir = Path(__file__).parents[2] / "sw_audio_analyzer"
    with AudioAnalyzerHarness(adapter_harness, analyzer_dir, attach="xscope") as harness:
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

def test_load_awb_from_ffs_device(flash_ua_with_ffs):
    """
    Runs 
    """
    output = run_xe_hw(xe_ffs_rpc_device, opts=["--io"])

    assert "xawe_loadAWBfromFFS SUCCESS" in output, f"Failed loading AWB from FFS: {output}"

# For local testing only
if __name__ == "__main__":
    test_xawe_ctrl_interface()


