# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Tests the FFS callbacks using the app in test_ffs
"""

import pytest
from pathlib import Path
import struct
from awe_test_utils import flash_xe, run_xe_hw

xe_ffs = "test_ffs/bin/test_ffs_rpc.xe"

def test_xawe_ctrl_interface():
    # stdout = flash_xe(xe_ffs, boot_partition_size=0x80000)
    stdout = run_xe_hw(xe_ffs, ["--io"])
    print(stdout)

# For local testing only
if __name__ == "__main__":
    test_xawe_ctrl_interface()


