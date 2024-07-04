# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
"""
Various helpers for testing awe
"""
import hid
import struct
import numpy as np
import time
import re
import subprocess
import time
from pathlib import Path
import sys


class awe_cmd_list:
    """
    Reads the commands file and turns them into the enum for sending cmds
    """
    def __init__(self, file="../lib_awe/src/include_internal/ProxyIDs.h"):
        self.enum_dict = {}

        with open(file) as ef:
            file_content = ef.read()
            enum_pattern = re.compile(r"typedef\s+enum\s*{([^}]*)}\s*(\w+);", re.DOTALL)
            matches = enum_pattern.search(file_content)
            if matches:
                enum_body = matches.group(1)
                # Remove all comments
                enum_body = re.sub(r'//.*?\n|/\*.*?\*/', '', enum_body, flags=re.DOTALL)
                enum_entries = enum_body.split(',')
                current_value = -1
                for entry in enum_entries:
                    entry = entry.strip()
                    if entry:
                        if '=' in entry:
                            name, value = entry.split('=')
                            name = name.strip()
                            value = int(value.strip())
                            current_value = value
                        else:
                            name = entry.strip()
                            current_value += 1
                        self.enum_dict[name] = current_value

    def lookup(self, cmd):
        return self.enum_dict[cmd]

    def get_keys(self):
        return self.enum_dict

class awe_error_codes:
    """
    Reads the error file and turns them into a string from the err code
    """
    def __init__(self, file="../lib_awe/src/include_internal/Errors.h"):
        self.file = file
        self.define_dict = self._parse_defines()

    def _parse_defines(self):
        define_dict = {}
        define_pattern = re.compile(r"#define\s+(\w+)\s+\(([-\d]+)\)")

        with open(self.file, 'r') as ef:
            file_content = ef.read()
            for match in define_pattern.findall(file_content):
                name, value = match
                define_dict[int(value)] = name

        return define_dict

    def lookup(self, err_code):
        return self.define_dict.get(err_code, None)

    def get_keys(self):
        return self.define_dict


# Get the udev rules sorted in linux https://depts.washington.edu/dxscdoc/Help/Guides/HID_permissions.html#:~:text=On%20Linux&text=This%20will%20give%20read%20and,plugging%20and%20replugging%20the%20device.
class awe_hid_comms(awe_error_codes, awe_cmd_list):
    """
        See: https://dspconcepts.com/sites/default/files/forum/awe-core-integration-guide.pdf
    """

    resp_a = 0x30000
    resp_b = 0x40000

    def __init__(self, VID=0x20b1, PID=0x18, debug=False):
        np.set_printoptions(formatter={'int':hex})
        self.awe_hid_len = 56 # Bytes

        awe_error_codes.__init__(self)
        awe_cmd_list.__init__(self)

        # For debug
        if debug:
            devices = hid.enumerate()
            for device in devices:
                print(device)

        self.dev = hid.device()
        self.dev.open(VID, PID)

        if debug:
            print(f"Connected to 0x{VID:x} 0x{PID:x}")


    def cmd(self, msg):
        """ Send and receive in one hit """
        self.send(msg)
        response = self.get_response()
        err = self.check_response(response)

        return err


    def get_response(self, timeout_ms=1000):
        """ Get the response from a command over HID """
        try:
            response = []
            num_words_received = 0

            # Some longer responses span more than one HID packet
            while True:
                data = bytearray(self.dev.read(self.awe_hid_len, timeout_ms))

                for idx in range(0, self.awe_hid_len, 4):
                    word = struct.unpack('<I', data[idx: idx + 4])[0]
                    # Strip HID header
                    if idx == 0:
                        num_words, sequence = self._decode_hid_header(word)
                    else:
                        response += [word]
                        if len(response) == 1:
                            length = response[0] >> 16
                        num_words_received += 1

                # We have received all words
                if num_words_received >= length:
                    break

        except IOError as e:
            print(f'Error reading response: {e}')
            return None

        except struct.error as e:
            print(f'Empty HID response: {e}')

        # check CRC of received message
        crc_calc = 0x00000000
        for word in response[0 : length - 1]:
            crc_calc ^= word
        assert crc_calc == response[length - 1]

        return response[0:length]


    def check_response(self, response):
        if response[0] == self.resp_a:
            err = response[1]
        elif response[0] == self.resp_b:
            sequence = response[1]
            err = response[2]
        else:
            print(f"Unknown response: {response}")
            err = 0

        return err


    def send(self, msg):
        """ Send a command. Audtomatically generates CRC """
        msg.append(self._calc_crc(msg))

        max_words = self.awe_hid_len // 4 - 1 # byte count minus HID header
        total_len = len(msg)
        sequence = 0 # For splitting across multiple HID transfers if longer than std packet
        num_bytes_written = 0

        # Send whole chunks
        while total_len > max_words:
            chunk = [self._gen_hid_header(sequence, max_words)] + msg[max_words * sequence : max_words * sequence + 13]
            num_bytes_written += self._send_packet(chunk)
            total_len -= max_words
            sequence += 1

        # Send remainder
        if total_len > 0:
            chunk = [self._gen_hid_header(sequence, total_len)] + msg[max_words * sequence : ]
            num_bytes_written += self._send_packet(chunk)

        return num_bytes_written

    def _calc_crc(self, msg):
        """ Generate the AWE msg CRC """
        crc = 0
        for word in msg:
            crc ^= word

        return crc

    def _decode_hid_header(self, word):
        num_words = word >> 24 // 4
        sequence = ((word >> 8) & 0xff) - 1

        return num_words, sequence

    def _gen_hid_header(self, sequence, num_words):
        """ Calaculate the HID header """
        header = 0x100 * (sequence + 1) + 1
        header += (num_words * 4) << 24 # sizeof int

        return header

    def _send_packet(self, chunk):
        """ Low level send complete HID packet """
        b_msg = bytearray(0) # one padding byte needed
        for word in chunk:
            b_msg += struct.pack('<I', word)

        b_msg += bytearray([0] * (self.awe_hid_len - len(b_msg)))

        try:
            num_bytes_written = self.dev.write(b_msg)
        except IOError as e:
            print ('Error writing command: {}'.format(e))
            num_bytes_written = 0

        return num_bytes_written

    def send_awb(self, awb_file):
        """ Loads awb binary file"""
        awb_data = np.fromfile(awb_file, dtype=np.uint32)
        awb_data_len = awb_data.shape[0]

        # Stop audio first to avoid frequent firmware exception when destroying existing design
        # See https://xmosjira.atlassian.net/wiki/spaces/UAAI/pages/4122148883/DSPC+Integration+snag+list
        self.cmd([0x20000 + awe_cmd_list.lookup(self, 'PFID_StopAudio')])
        time.sleep(0.001) # 1ms to allow stop audio to complete

        awb_idx = 0
        while(awb_idx <= awb_data_len):
            cmd_len = (awb_data[awb_idx] >> 16) - 1
            # Catch zero padding invalid command at end of AWB
            if cmd_len == -1:
                break

            cmd = awb_data[awb_idx : awb_idx + cmd_len]
            err = self.cmd(list(cmd))
            if err:
                err_2s_compl = struct.unpack('<i', struct.pack('<I', err))[0]
                err_str = awe_error_codes.lookup(self, err_2s_compl)
                print(f"ERROR response: {err_str}")
            awb_idx += cmd_len


def run_xe_sim(bin_path, cmds, max_cycles=1000000):
    cmd = f"xsim --max-cycles {max_cycles} --args {bin_path} {cmds}"
  
    ret = subprocess.run(cmd.split(), capture_output=True, text=True)
    assert ret.returncode == 0, f"Failed runing {cmd}: {ret.stderr}"

    return ret.stdout

def run_xe_hw(bin_path, opts=None):
    options = "" if opts is None else " ".join(opts)
    cmd = f"xrun {options} {bin_path}"
  
    ret = subprocess.run(cmd.split(), capture_output=True, text=True)
    assert ret.returncode == 0, f"Failed runing {cmd}: {ret.stderr}"

    return ret.stdout

def flash_xe(bin_path, boot_partition_size=None, data_partition_bin=None):
    if boot_partition_size is None:
        cmd = f"xflash {bin_path}"
    else:
        cmd = f"xflash --factory {bin_path} --boot-partition-size {boot_partition_size}"
        if data_partition_bin is not None:
            cmd += f" --data {data_partition_bin}"
  
    ret = subprocess.run(cmd.split(), capture_output=True, text=True)
    assert ret.returncode == 0, f"Failed runing {cmd}: {ret.stderr}"

    return ret.stdout

def filter_awe_packet_log():
    """Takes the AWE low level packet log and decodes the commands into human readable format"""
    input_file = Path('packet_log.txt').resolve()

    cmds = awe_cmd_list()

    with open(input_file, 'r') as infile:
        for line in infile:
            # Check if the line contains 'TX'
            if 'TX' in line:
                # Split the line by 'TX' and take the part after 'TX'
                hex_part = line.split('TX', 1)[1].strip().split()
                # Write the hex part to the output file
                header = int(hex_part[0], 16)
                opcode = header & 0xff
                try:
                    key = next(key for key, value in cmds.get_keys().items() if value == opcode)
                except StopIteration:
                    print(opcode, line)

                length = header >> 16
                print(key, opcode, length, hex_part[0])

            if 'RX' in line:
                hex_part = line.split('RX', 1)[1].strip().split()
                # print(" ".join(hex_part))
                if not "failed" in line:
                    strng = ""
                    offset = 0
                    first_char = 0

                    response = [int(hex_item, 16) for hex_item in hex_part]
                    print(len(response), hex_part)
                    for word in response:
                        for byte_num in range(4):
                            byte = (word >> (8 * byte_num)) & 0xff
                            if byte < 127 and byte >= 32 :
                                strng += chr(byte)
                                if first_char == 0:
                                    first_char = offset
                            offset += 1
                    print(f"ascii ({first_char}): {strng}")
                print()


# For testing only
if __name__ == '__main__':

    # filter_awe_packet_log()
    # sys.exit(0)

    import argparse

    parser = argparse.ArgumentParser(description='awe_test_utils')
    parser.add_argument('--pid', type=int, help='PID of target device', default=0x18)

    args = parser.parse_args()

    awe = awe_hid_comms(PID=args.pid)
    awe.send_awb("awb_files/simple_volume.awb")
