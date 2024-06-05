import hid

devices = hid.enumerate()
for device in devices:
    print(device)

device = hid.device()
VID = 5824
PID = 10202
device.open(VID, PID)
print('Connected to ADU{}\n'.format(PID))

def read_from_hid(dev, timeout_ms=1000):
    try:
        # read a maximum of 8 bytes from the device, with a user specified timeout
        data = dev.read(56, timeout_ms)
    except IOError as e:
        print ('Error reading response: {}'.format(e))
        return None

    byte_str = ''.join(chr(n) for n in data[1:]) # construct a string out of the read values, starting from the 2nd byte

    result_str = byte_str.split('\x00',1)[0] # remove the trailing null '\x00' characters

    if len(result_str) == 0:
        return None

    return result_str

def write_to_hid(dev, msg_str):
    print('Writing command: {}'.format(msg_str))

    # message structure:
    #   message is an ASCII string containing the command
    #   8 bytes in length
    #   0th byte must always be 0x01 (decimal 1)
    #   bytes 1 to 7 are ASCII character values representing the command
    #   remainder of message is padded to 8 bytes with character code 0

    byte_str = chr(0x01) + msg_str + chr(0) * max(7 - len(msg_str), 0)

    try:
        num_bytes_written = dev.write(byte_str.encode())
    except IOError as e:
        print ('Error writing command: {}'.format(e))
        return None 

    return num_bytes_written

print(read_from_hid(device))
