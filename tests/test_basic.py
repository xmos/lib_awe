# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Test we can bring up AWE and query the lib with some basic commands.
Runs on xsim and accepts control input on the commandline and prints
the response from AWE to stdout where it is checked against the expected
output.
"""

import pytest
from pathlib import Path
import subprocess
import struct
from awe_test_utils import awe_cmd_list, awe_error_codes

xe = "test_basic/bin/test_awe_basic.xe"


def send_command(xe, cmd):
    crc = 0 ^ cmd
    cmd_str = f"{hex(cmd)} {hex(crc)}"
    cmd_str = cmd_str.replace("0x", "")
    return run_xe(xe, cmd_str)

def run_xe(bin_dir, cmds):
    cmd = f"xsim --max-cycles 1000000 --args {bin_dir} {cmds}"
  
    ret = subprocess.run(cmd.split(), capture_output=True, text=True)
    assert ret.returncode == 0, f"Failed runing {cmd}: {ret.stderr}"
    # print(ret.stderr)

    return ret.stdout

def check_expected(dut, expected):
    for dut_word, expected_word in zip(dut.split(), expected.split()):
        print(f"DUT: {dut_word} Expected: {expected_word}")
        assert int(dut_word, 16) == int(expected_word, 16)


def test_target_info():
    cmd = 0x00020000 + awe_cmd_list().lookup("PFID_GetTargetInfo")
    dut = send_command(xe, cmd)
    expected = "000e0000 00000000 473b8000 4b3ebc20 00403020 24020264 08440100 00000107 534f4d58 4253555f 4cbebc20 00000000 7a6b5c4d 07c4f609"

    to_fp = lambda a : struct.unpack('f', struct.pack('I', a))[0]

    expected_hex = [int(word, 16) for word in expected.split()]
    fs = to_fp(expected_hex[2])
    print(f"Sample rate: {fs}")

    check_expected(dut, expected)

def test_get_moduleclass_count():
    cmd = 0x00020000 + awe_cmd_list().lookup("PFID_GetCIModuleCount")
    dut = send_command(xe, cmd)
    count = 325
    expected = f"00030000 00000{count:x} 00030{count:x}"

    check_expected(dut, expected)

def test_get_core_list():
    cmd = 0x00020000 + awe_cmd_list().lookup("PFID_GetCores2")
    dut = send_command(xe, cmd)
    cores = 1
    expected = f"00040000 0000000{cores} 0000000{cores-1} 0004000{cores}"

    check_expected(dut, expected)


if __name__ == "__main__":
    cmds = awe_cmd_list()
    print(cmds.lookup("PFID_SetValuesSetCall"))
    error_codes = awe_error_codes()
    print(error_codes.lookup(-71))


