# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Tests the FFS callbacks using the app in test_ffs
"""

import pytest
from pathlib import Path
import struct
from awe_test_utils import flash_xe, run_xe_hw, awe_hid_comms
import time

xe_ffs = "test_ffs/bin/test_ffs_rpc.xe"
xe_demo_ffs = "../examples/app_usb_audio_awe/bin/UA_FFS/app_usb_audio_awe_UA_FFS.xe"

def test_xawe_ctrl_interface():
    # stdout = flash_xe(xe_ffs, boot_partition_size=0x80000)
    stdout = run_xe_hw(xe_ffs, ["--io"])
    print(stdout)


def test_load_awb_from_ffs():
    pytest.skip() # Until we have HW test in place

    stdout = flash_xe(xe_demo_ffs, boot_partition_size=0x80000, data_partition_bin="../examples/audioweaver/awb_files/data_partition_ffs.bin")
    time.sleep(5) # Wait for HID to come up

    awe = awe_hid_comms()

    # hammer the loading mechanism 400 times
    for i in range(200):
        assert awe.load_awb_from_ffs("playBasic_3thread.awb")
        assert awe.load_awb_from_ffs("simple_volume.awb")



# For local testing only
if __name__ == "__main__":
    test_xawe_ctrl_interface()


