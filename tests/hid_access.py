import hid
import struct

class awe_hid_comms:
    def __init__(self, VID=0x20b1, PID=0x18):
        self.awe_hid_len = 56

        # devices = hid.enumerate()
        # for device in devices:
        #     print(device)

        self.dev = hid.device()
        self.dev.open(VID, PID)
        print(f"Connected to 0x{VID:x} 0x{PID:x}")


    def cmd(self, msg):
        self.send(msg)

        return self.get_response()

    def get_response(self, timeout_ms=1000):
        try:
            response = []
            data = bytearray(self.dev.read(self.awe_hid_len, timeout_ms))
            # print(data)
            for idx in range(0, self.awe_hid_len, 4):
                word = struct.unpack('<I', data[idx: idx + 4])[0]
                if idx == 0:
                    hdr = data[idx: idx + 4]
                    num_pckts = hdr[0]
                    length = hdr[3] // 4
                    print(f"lenght: {length}")
                else:
                    response += [word]

        except IOError as e:
            print ('Error reading response: {}'.format(e))
            return None

        crc_calc = 0x00000000
        for word in response[0 : length - 1]:
            crc_calc ^= word
        assert crc_calc == response[length - 1]

        return response[0:length]

    def send(self, msg):
        crc = 0x00000000
        for word in msg:
            crc ^= word
        msg += [crc]

        max_words = self.awe_hid_len // 4 - 1 # byte count minus HID header
        total_len = len(msg)
        sequence = 0 # For splitting across multiple HID transfers
        num_bytes_written = 0

        # Send whole chunks
        while total_len > max_words:
            chunk = [self._gen_header(sequence, max_words)] + msg[max_words * sequence : max_words * sequence + 13]
            # print(chunk)
            num_bytes_written += self._send_packet(chunk)
            total_len -= max_words
            sequence += 1

        # Send remainder
        if total_len > 0:
            chunk = [self._gen_header(sequence, total_len)] + msg[max_words * sequence : ]
            # print(chunk)
            num_bytes_written += self._send_packet(chunk)

        return num_bytes_written


    def _gen_header(self, sequence, num_words):
        header = 0x100 * (sequence + 1) + 1
        header += (num_words * 4) << 24 # sizeof int

        return header

    def _send_packet(self, chunk):
        b_msg = bytearray(0) # one padding byte needed
        for word in chunk:
            print(hex(word))
            b_msg += struct.pack('<I', word)

        b_msg += bytearray([0] * (self.awe_hid_len - len(b_msg))) 
        
        try:
            num_bytes_written = self.dev.write(b_msg)
        except IOError as e:
            print ('Error writing command: {}'.format(e))
            num_bytes_written = 0

        return num_bytes_written

if __name__ == '__main__':
    awe = awe_hid_comms()
    msg =  [0x0002000d]
    response = awe.cmd(msg)

    for word in response:
        print(hex(word))
