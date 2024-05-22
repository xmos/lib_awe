# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XCORE VocalFusion Licence.
"""
Test we can bring up AWE
"""

import pytest
from pathlib import Path
import subprocess

xe = "test_basic/bin/test_awe_basic.xe"


def run_xe(bin_dir, cmds):
    cmd = f"xsim --args {bin_dir} {cmds}"
  
    ret = subprocess.run(cmd.split(), capture_output=True, text=True)
    assert ret.returncode == 0, f"Failed runing {cmd}: {ret.stderr}"

    # print(ret.stderr)

    return ret.stdout

def check_expected(dut, expected):
    for dut_word, expected_word in zip(dut.split(), expected.split()):
        # print(dut_word, expected_word)
        assert int(dut_word, 16) == int(expected_word, 16)

# Placeholder
def test_target_info():
    dut = run_xe(xe, "00020029 00020029")
    expected = "000e0000 00000000 473b8000 4b3ebc20 00403020 24020264 08440100 00000107 534f4d58 4253555f 4cbebc20 00000000 7a6b5c4d 07c4f609"

    check_expected(dut, expected)
