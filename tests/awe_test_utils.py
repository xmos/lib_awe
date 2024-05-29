# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XCORE VocalFusion Licence.
"""
Various helpers for testing awe
"""

import numpy as np
import platform
import subprocess
import sys

np.set_printoptions(formatter={'int':hex})


def load_awb(awb_file):
    """ Loads awb binary file"""
    awb_data = np.fromfile(awb_file, dtype=np.uint32)
    return awb_data

def send_packet(packet):
    if packet is None:
        return

    util_cmd = "tools/awe_hid_transfer"
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

def filter_awe_packet_log():
    """Takes the AWE low level packet log and filters out all TX operations and captures then in a file"""
    # Define the input and output file paths
    input_file = 'packet_log.txt'  # Replace with your input file path
    output_file = 'filtered_output.txt'  # Replace with your output file path

    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        for line in infile:
            # Check if the line contains 'TX'
            if 'TX' in line:
                # Split the line by 'TX' and take the part after 'TX'
                hex_part = line.split('TX', 1)[1].strip()
                # Write the hex part to the output file
                outfile.write(hex_part + '\n')

    # Print a message indicating the process is complete
    print(f"Filtered hex values have been written to {output_file}")



demo_cmds = """0020029 00020029
0002007f 0002007f
00020029 00020029
0002007f 0002007f
00020029 00020029
0002007e 0002007e
00020019 00020019
0003001a 00000001 0003001b
0003001a 00000002 00030018
00020029 00020029
0002001c 0002001c
00020029 00020029
0002000c 0002000c
00020029 00020029
0002007f 0002007f
00020029 00020029
0002007e 0002007e
00020019 00020019
0003001a 00000001 0003001b
0003001a 00000002 00030018
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008001 00000020 00000000 073d0027
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008001 00000020 00000000 073d0027
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008001 00000020 00000000 073d0027
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00060006 473b8000 40008002 00000020 00000000 073d0024
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00040007 00000001 00000001 00040007
0002000a 0002000a
0002000b 0002000b
00040007 00000002 00000002 00040007
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0009000f beef0866 00000101 00000002 00000001 00000003 00000001 00000000 bee60969
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000b000f beef0d52 00000001 00000005 00000003 00000040 00000000 00000000 00000040 00000000 bee40d5a
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0006000f beef0d53 00000100 00000000 00000004 bee90c58
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000b000f beef0d52 00000001 00000005 00000004 00000040 00000000 00000000 00000040 00000000 bee40d5d
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0006000f beef0d53 00000100 00000000 00000005 bee90c59
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000c000f beef086b 00010001 00000005 00000005 00000006 00000012 42820000 42820000 3cbf5380 3cbf5380 bee20871
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0013000f beef0868 00000102 0000000b 00000003 00000005 00000007 00000001 41200000 00000000 00000000 3b086300 3f800000 00000000 00000000 00000000 00000001 00000001 fb546a6e
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000d000f beef0813 00000101 00000006 00000007 00000007 00000000 41200000 00000001 3f800000 3f800000 3b086300 c4ca6a1a
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000c000f beef086b 00010001 00000005 00000007 00000006 00000012 42820000 42820000 3cbf5380 3cbf5380 bee20873
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0009000f beef0866 00000101 00000002 00000007 00000002 00000000 00000001 bee6096e
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0006000f beef0d53 00000100 00000000 00000008 bee90c54
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000b000f beef0808 00000101 00000004 00000008 00000008 00000005 41200000 00000001 3d8414e8 c2401dee
0002000a 0002000a
0002000b 0002000b
000b0065 0001a00d 00000000 00002000 00000005 437a0000 437a0000 437a0000 437a0000 437a0000 4370806d
000b0065 0001a00f 00000000 00008000 00000005 3f800000 3f800000 3f800000 3f800000 3f800000 3f8a206f
001f0065 0001a011 00000000 00020000 00000019 3f800000 00000000 00000000 00000000 00000000 3f800000 00000000 00000000 00000000 00000000 3f800000 00000000 00000000 00000000 00000000 3f800000 00000000 00000000 00000000 00000000 3f800000 00000000 00000000 00000000 00000000 3f9ca06d
0002000a 0002000a
0002000b 0002000b
000d000f beef0813 00000101 00000006 00000008 00000008 c0800000 41200000 00000001 3f21866c 3f21866c 3b086300 044a6a1a
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000b000f beef0d52 00000001 00000005 00000008 00000040 00000000 00000000 00000040 00000000 bee40d51
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0006000f beef0d53 00000100 00000000 00000004 bee90c58
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000004 00000009 00000001 00000002 42fa0000 fc1f0906
0002000a 0002000a
0002000b 0002000b
00040001 0000001e ffffffff fffbffe0
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000009 0000000a 00000001 00000002 42fa0000 fc1f0908
0002000a 0002000a
0002000b 0002000b
00040001 00000020 ffffffff fffbffde
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 0000000a 00000009 00000001 00000002 457a0000 fb9f0908
0002000a 0002000a
0002000b 0002000b
00040001 00000022 ffffffff fffbffdc
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000009 0000000b 00000001 00000002 457a0000 fb9f0909
0002000a 0002000a
0002000b 0002000b
00040001 00000024 ffffffff fffbffda
0002000a 0002000a
0002000b 0002000b
0008000f beef082c 00000101 00000001 0000000b 0000000c 00000001 bee70925
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0007000f beef082b 00000101 00000000 0000000c 0000000c bee80925
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0013000f beef0804 00000101 0000000c 0000000c 0000000d c0400000 00000000 3f600000 40000000 41000000 40000000 42480000 00000000 3f000000 3c29c800 39da6800 00000000 7867a907
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0801 00000101 00000003 0000000b 00000009 40a00000 40400000 00000231 be050b3f
0002000a 0002000a
0002000b 0002000b
00040001 00000029 ffffffff fffbffd7
0002000a 0002000a
0002000b 0002000b
000a000f beef0839 00000102 00000002 0000000d 00000009 0000000b 00000001 00000000 bee50938
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000004 00000009 00000000 00000002 42fa0000 fc1f0907
0002000a 0002000a
0002000b 0002000b
00040001 0000002c ffffffff fffbffd2
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000009 00000004 00000000 00000002 42fa0000 fc1f0907
0002000a 0002000a
0002000b 0002000b
00040001 0000002e ffffffff fffbffd0
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000004 00000009 00000001 00000002 457a0000 fb9f0906
0002000a 0002000a
0002000b 0002000b
00040001 00000030 ffffffff fffbffce
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000009 0000000e 00000001 00000002 457a0000 fb9f090c
0002000a 0002000a
0002000b 0002000b
00040001 00000032 ffffffff fffbffcc
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 0000000a 00000009 00000000 00000002 457a0000 fb9f0909
0002000a 0002000a
0002000b 0002000b
00040001 00000034 ffffffff fffbffca
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000009 0000000a 00000000 00000002 457a0000 fb9f0909
0002000a 0002000a
0002000b 0002000b
00040001 00000036 ffffffff fffbffc8
0002000a 0002000a
0002000b 0002000b
0008000f beef082c 00000101 00000001 0000000a 0000000c 00000001 bee70924
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0007000f beef082b 00000101 00000000 0000000c 0000000c bee80925
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0013000f beef0804 00000101 0000000c 0000000c 0000000d c0400000 00000000 3f666666 40000000 41200000 40000000 42480000 00000000 3f000000 3c29c800 39da6800 00000000 7841cf61
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0801 00000101 00000003 0000000a 00000009 40a00000 40400000 00000231 be050b3e
0002000a 0002000a
0002000b 0002000b
00040001 0000003b ffffffff fffbffc5
0002000a 0002000a
0002000b 0002000b
000a000f beef0839 00000102 00000002 0000000d 00000009 0000000a 00000001 00000000 bee50939
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000004 00000009 00000000 00000002 457a0000 fb9f0907
0002000a 0002000a
0002000b 0002000b
00040001 0000003e ffffffff fffbffc0
0002000a 0002000a
0002000b 0002000b
000a000f beef0805 00000101 00000003 00000009 00000004 00000000 00000002 457a0000 fb9f0907
0002000a 0002000a
0002000b 0002000b
00040001 00000040 ffffffff fffbffbe
0002000a 0002000a
0002000b 0002000b
0009000f beef082c 00000102 00000001 00000004 0000000e 00000009 00000000 bee60923
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0008000f beef082c 00000101 00000001 00000009 0000000c 00000001 bee70927
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0007000f beef082b 00000101 00000000 0000000c 0000000c bee80925
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0013000f beef0804 00000101 0000000c 0000000c 0000000d c0400000 00000000 3f666666 40000000 41200000 40400000 42480000 00000000 3f000000 3be2c480 39da6800 00000000 7fcac3e1
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0801 00000101 00000003 00000009 00000004 40a00000 40400000 00000231 be050b30
0002000a 0002000a
0002000b 0002000b
00040001 00000046 ffffffff fffbffb8
0002000a 0002000a
0002000b 0002000b
000a000f beef0839 00000102 00000002 0000000d 00000004 00000009 00000001 00000000 bee50937
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef082c 00000103 00000001 00000009 0000000a 0000000b 00000004 00000000 bee5092d
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0801 00000101 00000003 00000004 00000009 40a00000 40400000 00000231 be050b30
0002000a 0002000a
0002000b 0002000b
00040001 0000004a ffffffff fffbffb4
0002000a 0002000a
0002000b 0002000b
0008000f beef082c 00000101 00000001 00000004 0000000c 00000001 bee7092a
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0007000f beef082b 00000101 00000000 0000000c 0000000c bee80925
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0013000f beef0804 00000101 0000000c 0000000c 0000000d c0400000 00000000 3f666666 40000000 41200000 3f800000 42480000 00000000 3f000000 3ca8e6c0 39da6800 00000000 0740e1a1
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000a000f beef0839 00000102 00000002 0000000d 00000009 00000004 00000001 00000000 bee50937
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
0009000f beef0850 00000101 00000002 00000004 00000004 bf800000 3f642905 3e022059
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
000b000f beef0d52 00000001 00000005 00000004 00000040 00000000 00000000 00000040 00000000 bee40d5d
0002000a 0002000a
0002000b 0002000b
0002000a 0002000a
0002000b 0002000b
00040010 00000008 00000001 00040019
0002000a 0002000a
0002000b 0002000b
000c002f 00000052 00000000 0000000f 00000010 00000013 00000014 00000015 00000016 00000017 00000018 000c0069
00040010 00000027 00000001 00040036
0002000a 0002000a
0002000b 0002000b
001d002f 00000053 00000000 00000011 00000012 0000001d 0000001e 00000020 00000022 00000024 00000026 00000027 00000028 00000029 0000002b 0000002c 0000002e 00000030 00000032 00000034 00000036 00000038 00000039 0000003a 0000003b 0000003d 0000003e 00000040 001d0030
0012002f 00000053 00000019 00000042 00000043 00000044 00000045 00000046 00000048 00000049 0000004a 0000004c 0000004d 0000004e 0000004f 00000050 00000051 00120069
00040010 00000004 00000001 00040015
0002000a 0002000a
0002000b 0002000b
0008002f 00000054 00000000 00000019 0000001a 0000001b 0000001c 0008007f
0002000a 0002000a
0002000b 0002000b
0005004d 00011008 00010000 00000000 00051045
0005004d 00019008 00012000 00000000 0005b045
0005004d 0001d008 0001c000 00000000 00051045
0005004d 00013008 00051000 00000000 00012045
00020029 00020029
0002001b 0002001b
00030083 00000000 00030083"""


demo_cmds_from_mac = """20029 20029
2007f 2007f
20029 20029
2007f 2007f
20029 20029
2007e 2007e
20019 20019
3001a 1 3001b
3001a 2 30018
20029 20029
2001c 2001c
20029 20029
2000c 2000c
20029 20029
2007f 2007f
20029 20029
2007e 2007e
20019 20019
3001a 1 3001b
3001a 2 30018
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008001 20 0 73d0027
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008001 20 0 73d0027
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008001 20 0 73d0027
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
60006 473b8000 40008002 20 0 73d0024
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
40007 1 1 40007
2000a 2000a
2000b 2000b
40007 2 2 40007
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
9000f beef0866 101 2 1 3 1 0 bee60969
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
b000f beef0d52 1 5 3 40 0 0 40 0 bee40d5a
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
6000f beef0d53 100 0 4 bee90c58
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
b000f beef0d52 1 5 4 40 0 0 40 0 bee40d5d
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
6000f beef0d53 100 0 5 bee90c59
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
c000f beef086b 10001 5 5 6 12 42820000 42820000 3cbf5380 3cbf5380 bee20871
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
13000f beef0868 102 b 3 5 7 1 41200000 0 0 3b086300 3f800000 0 0 0 1 1 fb546a6e
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
d000f beef0813 101 6 7 7 0 41200000 1 3f800000 3f800000 3b086300 c4ca6a1a
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
c000f beef086b 10001 5 7 6 12 42820000 42820000 3cbf5380 3cbf5380 bee20873
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
9000f beef0866 101 2 7 2 0 1 bee6096e
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
6000f beef0d53 100 0 8 bee90c54
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
b000f beef0808 101 4 8 8 5 41200000 1 3d8414e8 c2401dee
2000a 2000a
2000b 2000b
b0065 1a00d 0 2000 5 437a0000 437a0000 437a0000 437a0000 437a0000 4370806d
b0065 1a00f 0 8000 5 3f800000 3f800000 3f800000 3f800000 3f800000 3f8a206f
1f0065 1a011 0 20000 19 3f800000 0 0 0 0 3f800000 0 0 0 0 3f800000 0 0 0 0 3f800000 0 0 0 0 3f800000 0 0 0 0 3f9ca06d
2000a 2000a
2000b 2000b
d000f beef0813 101 6 8 8 c0800000 41200000 1 3f21866c 3f21866c 3b086300 44a6a1a
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
b000f beef0d52 1 5 8 40 0 0 40 0 bee40d51
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
6000f beef0d53 100 0 4 bee90c58
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0805 101 3 4 9 1 2 42fa0000 fc1f0906
2000a 2000a
2000b 2000b
40001 1e ffffffff fffbffe0
2000a 2000a
2000b 2000b
a000f beef0805 101 3 9 a 1 2 42fa0000 fc1f0908
2000a 2000a
2000b 2000b
40001 20 ffffffff fffbffde
2000a 2000a
2000b 2000b
a000f beef0805 101 3 a 9 1 2 457a0000 fb9f0908
2000a 2000a
2000b 2000b
40001 22 ffffffff fffbffdc
2000a 2000a
2000b 2000b
a000f beef0805 101 3 9 b 1 2 457a0000 fb9f0909
2000a 2000a
2000b 2000b
40001 24 ffffffff fffbffda
2000a 2000a
2000b 2000b
8000f beef082c 101 1 b c 1 bee70925
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
7000f beef082b 101 0 c c bee80925
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
13000f beef0804 101 c c d c0400000 0 3f600000 40000000 41000000 40000000 42480000 0 3f000000 3c29c800 39da6800 0 7867a907
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0801 101 3 b 9 40a00000 40400000 231 be050b3f
2000a 2000a
2000b 2000b
40001 29 ffffffff fffbffd7
2000a 2000a
2000b 2000b
a000f beef0839 102 2 d 9 b 1 0 bee50938
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0805 101 3 4 9 0 2 42fa0000 fc1f0907
2000a 2000a
2000b 2000b
40001 2c ffffffff fffbffd2
2000a 2000a
2000b 2000b
a000f beef0805 101 3 9 4 0 2 42fa0000 fc1f0907
2000a 2000a
2000b 2000b
40001 2e ffffffff fffbffd0
2000a 2000a
2000b 2000b
a000f beef0805 101 3 4 9 1 2 457a0000 fb9f0906
2000a 2000a
2000b 2000b
40001 30 ffffffff fffbffce
2000a 2000a
2000b 2000b
a000f beef0805 101 3 9 e 1 2 457a0000 fb9f090c
2000a 2000a
2000b 2000b
40001 32 ffffffff fffbffcc
2000a 2000a
2000b 2000b
a000f beef0805 101 3 a 9 0 2 457a0000 fb9f0909
2000a 2000a
2000b 2000b
40001 34 ffffffff fffbffca
2000a 2000a
2000b 2000b
a000f beef0805 101 3 9 a 0 2 457a0000 fb9f0909
2000a 2000a
2000b 2000b
40001 36 ffffffff fffbffc8
2000a 2000a
2000b 2000b
8000f beef082c 101 1 a c 1 bee70924
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
7000f beef082b 101 0 c c bee80925
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
13000f beef0804 101 c c d c0400000 0 3f666666 40000000 41200000 40000000 42480000 0 3f000000 3c29c800 39da6800 0 7841cf61
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0801 101 3 a 9 40a00000 40400000 231 be050b3e
2000a 2000a
2000b 2000b
40001 3b ffffffff fffbffc5
2000a 2000a
2000b 2000b
a000f beef0839 102 2 d 9 a 1 0 bee50939
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0805 101 3 4 9 0 2 457a0000 fb9f0907
2000a 2000a
2000b 2000b
40001 3e ffffffff fffbffc0
2000a 2000a
2000b 2000b
a000f beef0805 101 3 9 4 0 2 457a0000 fb9f0907
2000a 2000a
2000b 2000b
40001 40 ffffffff fffbffbe
2000a 2000a
2000b 2000b
9000f beef082c 102 1 4 e 9 0 bee60923
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
8000f beef082c 101 1 9 c 1 bee70927
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
7000f beef082b 101 0 c c bee80925
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
13000f beef0804 101 c c d c0400000 0 3f666666 40000000 41200000 40400000 42480000 0 3f000000 3be2c480 39da6800 0 7fcac3e1
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0801 101 3 9 4 40a00000 40400000 231 be050b30
2000a 2000a
2000b 2000b
40001 46 ffffffff fffbffb8
2000a 2000a
2000b 2000b
a000f beef0839 102 2 d 4 9 1 0 bee50937
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef082c 103 1 9 a b 4 0 bee5092d
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0801 101 3 4 9 40a00000 40400000 231 be050b30
2000a 2000a
2000b 2000b
40001 4a ffffffff fffbffb4
2000a 2000a
2000b 2000b
8000f beef082c 101 1 4 c 1 bee7092a
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
7000f beef082b 101 0 c c bee80925
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
13000f beef0804 101 c c d c0400000 0 3f666666 40000000 41200000 3f800000 42480000 0 3f000000 3ca8e6c0 39da6800 0 740e1a1
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
a000f beef0839 102 2 d 9 4 1 0 bee50937
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
9000f beef0850 101 2 4 4 bf800000 3f642905 3e022059
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
b000f beef0d52 1 5 4 40 0 0 40 0 bee40d5d
2000a 2000a
2000b 2000b
2000a 2000a
2000b 2000b
40010 8 1 40019
2000a 2000a
2000b 2000b
c002f 52 0 f 10 13 14 15 16 17 18 c0069
40010 27 1 40036
2000a 2000a
2000b 2000b
1d002f 53 0 11 12 1d 1e 20 22 24 26 27 28 29 2b 2c 2e 30 32 34 36 38 39 3a 3b 3d 3e 40 1d0030
12002f 53 19 42 43 44 45 46 48 49 4a 4c 4d 4e 4f 50 51 120069
40010 4 1 40015
2000a 2000a
2000b 2000b
8002f 54 0 19 1a 1b 1c 8007f
2000a 2000a
2000b 2000b
5004d 11008 10000 0 51045
5004d 19008 12000 0 5b045
5004d 1d008 1c000 0 51045
5004d 13008 51000 0 12045
20029 20029
2001b 2001b
30083 0 30083"""

# This works - they are all the same
if False:
    for l_win, l_mac in zip(demo_cmds.split("\n"), demo_cmds_from_mac.split("\n")):
        hex_values_win = l_win.split()        
        uint32_values_win = [int(hv, 16) for hv in hex_values_win]
        uint32_array_win = np.array(uint32_values_win, dtype=np.uint32)

        hex_values_mac = l_mac.split()        
        uint32_values_mac = [int(hv, 16) for hv in hex_values_mac]
        uint32_array_mac = np.array(uint32_values_mac, dtype=np.uint32)

        try:
            (uint32_array_win == uint32_array_mac).all()
        except:
            print(hex_values_win, hex_values_mac)
            sys.exit(1)

    print("ALL SAME")
    # sys.exit(0)

if False:
    for line in demo_cmds.split("\n"):
        hex_values = line.split()        
        # Convert hex strings to integers
        uint32_values = [int(hv, 16) for hv in hex_values]
        # Convert the list of integers to a numpy array with dtype=np.uint32
        uint32_array = np.array(uint32_values, dtype=np.uint32)
        print(uint32_array)
        send_packet(uint32_array)


# awb_data = np.array([0x00020029])
# send_awb_blob(awb_data)

# sys.exit(0)


awb_data = load_awb("awb_files/playBasic_3thread.awb")
send_awb_blob(awb_data)

sys.exit(0)

# # 0xbee40d5d
# awb_data = np.array([0x000b000f, 0xbeef0d52, 0x00000001, 0x00000005, 0x00000004, 0x00000040, 0x00000000, 0x00000000, 0x00000040, 0x00000000])
# command_to_packet(awb_data)

# # 0x001d0030
# awb_data = np.array([0x001d002f, 0x00000053, 0x00000000, 0x00000011, 0x00000012, 0x0000001d, 0x0000001e, 0x00000020, 0x00000022, 0x00000024, 0x00000026, 0x00000027, 0x00000028, 0x00000029, 0x0000002b, 0x0000002c, 0x0000002e, 0x00000030, 0x00000032, 0x00000034, 0x00000036, 0x00000038, 0x00000039, 0x0000003a, 0x0000003b, 0x0000003d, 0x0000003e, 0x00000040]) 
# command_to_packet(awb_data)

# awb_data = np.array([0x0002000a])
# command_to_packet(awb_data)

# awb_data = np.array([0x00030000, 0x00000001])
# command_to_packet(awb_data)
# print(awb_data.shape[0], awb_data)
