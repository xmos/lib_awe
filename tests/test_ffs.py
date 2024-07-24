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

from awe_test_utils import flash_xe, run_xe_hw, awe_hid_comms, xe_ffs_rpc, xe_ffs_rpc_device, xe_demo_ffs_host, dp_with_ffs, boot_partition_size 
from conftest import AweDut, get_xtag_ids

from hardware_test_tools.AudioAnalyzerHarness import AudioAnalyzerHarness
from hardware_test_tools.Xsig import XsigOutput
from hardware_test_tools.check_analyzer_output import check_analyzer_output


def post_boot_hid_delay():
    time.sleep(5) # Wait for HID to come up

@pytest.fixture
def flash_ua_with_ffs(pytestconfig, scope="session"):
    """
    This fixture programs the UA/FFS binary and the pre-made FFS containing a couple of AWBs.
    """

    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)
    stdout = flash_xe(xe_demo_ffs_host, adapter_dut, boot_partition_size=boot_partition_size, data_partition_bin=dp_with_ffs)
    print(stdout)


@pytest.mark.hw
def test_xawe_ffs_rpc(pytestconfig):
    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)

    # program flash and create empty DP 
    stdout = flash_xe(xe_demo_ffs_host, adapter_dut, boot_partition_size=boot_partition_size)
    print(stdout)
    stdout = run_xe_hw(xe_ffs_rpc, adapter_dut, ["--io"])
    print(stdout)



@pytest.mark.hw
def test_load_awb_from_ffs_host_stress(pytestconfig, flash_ua_with_ffs):
    """
    This test programs the UA/FFS binary and the pre-made FFS containing a couple of AWBs.
    It then uses a host command to load these many times to see if it works
    """
    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)
    stdout = flash_xe(xe_demo_ffs_host, adapter_dut, boot_partition_size=boot_partition_size, data_partition_bin=dp_with_ffs)
    
    post_boot_hid_delay()

    awe = awe_hid_comms()

    # hammer the loading mechanism 400 times
    for i in range(200):
        print(f"Iteration: {i}")
        assert awe.load_awb_from_ffs("playBasic_3thread.awb")
        assert awe.load_awb_from_ffs("simple_volume.awb")


@pytest.mark.hw
def test_load_awb_from_ffs_device_stress(pytestconfig, flash_ua_with_ffs):
    """
    Runs the firmware API version of xawe_loadAWBfromFFS many times on HW
    Print from xrun session will determine if it encountered an error or not
    """

    adapter_dut, adapter_harness = get_xtag_ids(pytestconfig)
    output = run_xe_hw(xe_ffs_rpc_device, adapter_dut, opts=["--io"])

    assert "xawe_loadAWBfromFFS SUCCESS" in output, f"Failed loading AWB from FFS: {output}"


# Local test only
if __name__ == "__main__":
    awe = awe_hid_comms()
    awe.send_awb("../examples/audioweaver/awb_files/simple_volume.awb")
    # awe.load_awb_from_ffs("playBasic_3thread.awb")