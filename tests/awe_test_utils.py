# Copyright 2024 XMOS LIMITED.
# This Software is subject to the terms of the XCORE VocalFusion Licence.
"""
Various helpers for testing awe
"""

import numpy as np
import platform
import subprocess


def load_awb(awb_file):
    awb_data = np.fromfile(awb_file, dtype=np.uint32)
    return awb_data

def send_awb(awb_data):
    awb_data_len = awb_data.shape[0]
    if platform.system() == 'Darwin':
        def send_chunk(chunk):
            util_cmd = "tools/awe_hid_transfer"
            chunk_txt = " ".join([format(word, "X") for word in list(chunk)])
            print(chunk_txt)
            ret = subprocess.run(f"{util_cmd} {chunk_txt}".split(), capture_output=True, text=True)
            print(ret.stderr)

        chunk_size = 16 
        for i in range(0, awb_data_len, chunk_size):
            if awb_data_len - i < 16:
                send_chunk(awb_data[i:])
            else:
                send_chunk(awb_data[i:i+16])
    else:
        pass

awb_data = load_awb("awb_files/playBasic_3thread.awb")
send_awb(awb_data)
# print(awb_data.shape[0], awb_data)
