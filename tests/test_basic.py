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
import struct
from awe_test_utils import awe_cmd_list, awe_error_codes, run_xe_sim, xe_cmd, xe_xawe


def send_command(xe, cmd, payload=[]):
    cmd = ((2 + len(payload)) << 16) + awe_cmd_list().lookup(cmd)
    crc = 0 ^ cmd
    payload_str = ""
    for item in payload:
        payload_str += f"{item:x} "
        crc ^= item

    cmd_str = f"{hex(cmd)} {payload_str} {hex(crc)}"
    cmd_str = cmd_str.replace("0x", "")

    return run_xe_sim(xe, cmd_str, max_cycles = 1000000 if len(payload) == 0 else 10000000 )


def check_expected(dut, expected):
    for dut_word, expected_word in zip(dut.split(), expected.split()):
        print(f"DUT: {dut_word} Expected: {expected_word}")
        assert int(dut_word, 16) == int(expected_word, 16)


@pytest.mark.sim
def test_target_info():
    dut = send_command(xe_cmd, "PFID_GetTargetInfo")
    major = 8
    minor = ord("D")
    patch = 8
    version_info = f"{(patch << 24) | (minor << 16) | (major << 8):x}"
    expected = f"000e0000 00000000 473b8000 4b3ebc20 00403020 24020264 {version_info} 00000107 534f4d58 4253555f 4cbebc20 00000000 7a6b5c4d 7c4ff09"
    """
    Message Length = 14 0
    error status (int)
    sample rate (float)
    profile clock speed in Hz (float)
    packedBlockSize (uint)
    packedData (uint)
    version information (uint)
    packetBufferSize (uint)
    packedName 1 (uint)
    packedName 2 (uint)
    cpuClockSpeed in Hz (float)
    coreID (uint)
    featureBits (uint)
    CRC (uint)
    """

    to_fp = lambda a : struct.unpack('f', struct.pack('I', a))[0]

    expected_hex = [int(word, 16) for word in expected.split()]
    fs = to_fp(expected_hex[2])
    print(f"Sample rate: {fs}")

    check_expected(dut, expected)


@pytest.mark.sim
def test_get_moduleclass_count():
    dut = send_command(xe_cmd, "PFID_GetCIModuleCount")
    count = 325
    expected = f"00030000 00000{count:x} 00030{count:x}"

    check_expected(dut, expected)


@pytest.mark.sim
def test_get_core_list():
    dut = send_command(xe_cmd, "PFID_GetCores2")
    cores = 1
    expected = f"00040000 0000000{cores} 0000000{cores-1} 0004000{cores}"

    check_expected(dut, expected)


@pytest.mark.sim
def test_long_packet():
    # 264 max packet length so cmd + id + len + offset + payload + CRC
    payload = [0] + [259] + [0] + [i for i in range(259)]
    dut = send_command(xe_cmd, "PFID_SetValueHandle", payload=payload)
    response = dut.split()
    error_code = int(response[1], 16)
    error_code = struct.unpack('<i', struct.pack('<I', error_code))[0] # do 2's complement

    error_lut = awe_error_codes()

    # We want a legit error rather than a broken packet
    assert error_lut.lookup(error_code) == "E_ARGUMENT_ERROR"


@pytest.mark.sim
def test_xawe_ctrl_interface():
    stdout = run_xe_sim(xe_xawe, "", max_cycles=10000000)
    print(stdout)

# Just for testing some of the utils
if __name__ == "__main__":
    cmds = awe_cmd_list()
    print(cmds.lookup("PFID_SetValuesSetCall"))
    error_codes = awe_error_codes()
    print(error_codes.lookup(-71))


