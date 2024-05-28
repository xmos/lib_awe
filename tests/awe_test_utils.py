# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XCORE VocalFusion Licence.
"""
Various helpers for testing awe
"""

import numpy as np
import platform
import subprocess
import sys


def load_awb(awb_file):
    """ Loads awb binary file"""
    awb_data = np.fromfile(awb_file, dtype=np.uint32)
    return awb_data

def send_packet(packet):
    if packet is None:
        return

    util_cmd = "tools/awe_hid_transfer "
    packet_txt = " ".join([format(word, "X") for word in list(packet)])
    print(util_cmd, packet_txt)
    ret = subprocess.run(f"{util_cmd} {packet_txt}".split(), capture_output=True, text=True)
    assert ret.returncode == 0, f"Failed runing {util_cmd}: {ret.stderr}"

    response = np.array([int(hexval, base=16) for hexval in ret.stdout.split()])
    print("np resp:", response)

    return ret.stdout


def command_to_packet(cmd):
    """ Turns a command into a control packet.
    See: https://dspconcepts.com/sites/default/files/forum/awe-core-integration-guide.pdf
    """

    header = cmd[0]
    if header == 0: # awbs are sometime padded with zeros
        return None

    payload_len = (header >> 16) - 2
    packet_len = 1 + payload_len + 1

    # print("len: ", packet_len, payload_len)
    packet = np.zeros(packet_len, dtype=np.uint32)
    packet[0] = header
    payload = cmd[1 : payload_len + 1]
    # print("payload", payload)
    packet[1 : 1 + payload_len] = payload
    tail = np.bitwise_xor.reduce(packet[:packet_len - 1])
    # print("sum: ", hex(header), hex(np.sum(payload)), hex(tail))
    packet[packet_len - 1] = tail

    return packet


def send_awb_blob(awb_data):
    """Turns an awb blob into a sequence of short command packets suitable of control interface transmission and sends them"""

    awb_data_len = awb_data.shape[0]
    awb_idx = 0
    if platform.system() == 'Darwin':
        while(awb_idx < awb_data_len):
            packet = command_to_packet(awb_data[awb_idx:])
            if packet is None:
                break 

            send_packet(packet)

            awb_idx += np.size(packet) - 1

    else:
        pass

np.set_printoptions(formatter={'int':hex})

# awb_data = np.array([0x00020029])
# send_awb_blob(awb_data)

# sys.exit(0)


awb_data = load_awb("awb_files/playBasic_3thread.awb")
send_awb_blob(awb_data)

sys.exit(0)
# 0xbee40d5d
awb_data = np.array([0x000b000f, 0xbeef0d52, 0x00000001, 0x00000005, 0x00000004, 0x00000040, 0x00000000, 0x00000000, 0x00000040, 0x00000000])
command_to_packet(awb_data)

# 0x001d0030
awb_data = np.array([0x001d002f, 0x00000053, 0x00000000, 0x00000011, 0x00000012, 0x0000001d, 0x0000001e, 0x00000020, 0x00000022, 0x00000024, 0x00000026, 0x00000027, 0x00000028, 0x00000029, 0x0000002b, 0x0000002c, 0x0000002e, 0x00000030, 0x00000032, 0x00000034, 0x00000036, 0x00000038, 0x00000039, 0x0000003a, 0x0000003b, 0x0000003d, 0x0000003e, 0x00000040]) 
command_to_packet(awb_data)

awb_data = np.array([0x0002000a])
command_to_packet(awb_data)

awb_data = np.array([0x00030000, 0x00000001])
command_to_packet(awb_data)
# print(awb_data.shape[0], awb_data)
