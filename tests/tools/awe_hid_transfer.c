/*******************************************************
2024 - Program to talk to AWE HID endpoint
It allows access EVEN on a Mac..
********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include <hidapi.h>

#define MAX_WORDS_PER_CMD   64

const double NSAppKitVersionNumber = 0.0;

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

// Fallback/example
#ifndef HID_API_MAKE_VERSION
#define HID_API_MAKE_VERSION(mj, mn, p) (((mj) << 24) | ((mn) << 8) | (p))
#endif
#ifndef HID_API_VERSION
#define HID_API_VERSION HID_API_MAKE_VERSION(HID_API_VERSION_MAJOR, HID_API_VERSION_MINOR, HID_API_VERSION_PATCH)
#endif

//
// Sample using platform-specific headers
#if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_darwin.h>
#endif

#if defined(_WIN32) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_winapi.h>
#endif

#if defined(USING_HIDAPI_LIBUSB) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_libusb.h>
#endif
//

const char *hid_bus_name(hid_bus_type bus_type) {
	static const char *const HidBusTypeName[] = {
		"Unknown",
		"USB",
		"Bluetooth",
		"I2C",
		"SPI",
	};

	if ((int)bus_type < 0)
		bus_type = HID_API_BUS_UNKNOWN;
	if ((int)bus_type >= (int)(sizeof(HidBusTypeName) / sizeof(HidBusTypeName[0])))
		bus_type = HID_API_BUS_UNKNOWN;

	return HidBusTypeName[bus_type];
}

void print_device(struct hid_device_info *cur_dev) {
	printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
	printf("\n");
	printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
	printf("  Product:      %ls\n", cur_dev->product_string);
	printf("  Release:      %hx\n", cur_dev->release_number);
	printf("  Interface:    %d\n",  cur_dev->interface_number);
	printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
	printf("  Bus type: %u (%s)\n", (unsigned)cur_dev->bus_type, hid_bus_name(cur_dev->bus_type));
	printf("\n");
}

void print_hid_report_descriptor_from_device(hid_device *device) {
	unsigned char descriptor[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];
	int res = 0;

	printf("  Report Descriptor: ");
	res = hid_get_report_descriptor(device, descriptor, sizeof(descriptor));
	if (res < 0) {
		printf("error getting: %ls", hid_error(device));
	}
	else {
		printf("(%d bytes)", res);
	}
	for (int i = 0; i < res; i++) {
		if (i % 10 == 0) {
			printf("\n");
		}
		printf("0x%02x, ", descriptor[i]);
	}
	printf("\n");
}

void print_hid_report_descriptor_from_path(const char *path) {
	hid_device *device = hid_open_path(path);
	if (device) {
		print_hid_report_descriptor_from_device(device);
		hid_close(device);
	}
	else {
		printf("  Report Descriptor: Unable to open device by path\n");
	}
}

void print_devices(struct hid_device_info *cur_dev) {
	for (; cur_dev; cur_dev = cur_dev->next) {
		print_device(cur_dev);
	}
}

void print_devices_with_descriptor(struct hid_device_info *cur_dev) {
	for (; cur_dev; cur_dev = cur_dev->next) {
		print_device(cur_dev);
		print_hid_report_descriptor_from_path(cur_dev->path);
	}
}


int parse_cmd_line(int argc, char** argv, uint32_t msg_to_awe_buffer[]){
    int count = 0;

    if(argc <= 1){
        fprintf(stderr, "Too few args to command line: %d\n", argc - 2);
        fprintf(stderr, "Use something like  ./awe_hid_transfer 0002000d 0002000d\n");
        exit(1);
    }
    uint32_t header = strtoul(argv[++count], NULL, 16);
    fprintf(stderr, "header\t0x%8x\n", header);
    msg_to_awe_buffer[count - 1] = header;
    int num_words = header >> 16;

    if (num_words > MAX_WORDS_PER_CMD || num_words == 0) {
        fprintf(stderr, "Error in header, num_words specified (max %d): %u\n", MAX_WORDS_PER_CMD, num_words);
    }

    for(int i = 1; i < num_words; i++){
        uint32_t word = strtoul(argv[++count], NULL, 16); // Convert from hex
        msg_to_awe_buffer[count - 1] = word;
        fprintf(stderr, "payload\t0x%8x\n", word);
    }
    if(count != argc - 1) {
        fprintf(stderr, "Mismatch in number of commands and header\n");        
        exit(1);
    }

    return count;
}

int main(int argc, char* argv[])
{
    unsigned int cmd_to_send[MAX_WORDS_PER_CMD] = {0};
	int num_words_to_send = parse_cmd_line(argc, argv, cmd_to_send);

	int res;
	unsigned char buf[256];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	struct hid_device_info *devs;

	// printf("hidapi test/example tool. Compiled with hidapi version %s, runtime version %s.\n", HID_API_VERSION_STR, hid_version_str());
	if (HID_API_VERSION == HID_API_MAKE_VERSION(hid_version()->major, hid_version()->minor, hid_version()->patch)) {
		// printf("Compile-time version matches runtime version of hidapi.\n\n");
	}
	else {
		fprintf(stderr, "Compile-time version is different than runtime version of hidapi.\n]n");
	}

	if (hid_init())
		return -1;

#if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
	// To work properly needs to be called before hid_open/hid_open_path after hid_init.
	// Best/recommended option - call it right after hid_init.
	hid_darwin_set_open_exclusive(0);
#endif

	devs = hid_enumerate(0x0, 0x0);
	hid_free_enumeration(devs);

	// Set up the command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;


	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	handle = hid_open(0x20b1, 0x18, NULL);
	if (!handle) {
		fprintf(stderr, "unable to open device\n");
		hid_exit();
 		return 1;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		fprintf(stderr, "Unable to read manufacturer string\n");
	// printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		fprintf(stderr, "Unable to read product string\n");
	// printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read serial number string\n");
	// printf("Serial Number String: (%d) %ls\n", wstr[0], wstr);

	// print_hid_report_descriptor_from_device(handle);

	struct hid_device_info* info = hid_get_device_info(handle);
	if (info == NULL) {
		fprintf(stderr, "Unable to get device info\n");
	} else {
		// print_devices(info);
	}

    unsigned char cmd[MAX_WORDS_PER_CMD * (8 + 1)] = {0};

    unsigned packet_len = num_words_to_send * sizeof(unsigned int) + sizeof(unsigned int);

    unsigned int header = packet_len << 24;
    memcpy(&cmd[1], &header, sizeof(header));
    memcpy(&cmd[5], &cmd_to_send, sizeof(unsigned int) * num_words_to_send);

	hid_write(handle, cmd, packet_len + 1);
	if (res < 0) {
		fprintf(stderr, "Unable to write()/2: %ls\n", hid_error(handle));
	}

	// Read requested state. hid_read() has been set to be
	// non-blocking by the call to hid_set_nonblocking() above.
	// This loop demonstrates the non-blocking nature of hid_read().

    unsigned int response[MAX_WORDS_PER_CMD] = {0};

	res = 0;
	i = 0;
	while (res == 0) {
		res = hid_read(handle, (unsigned char*)response, sizeof(response));
		if (res == 0) {
		}
		if (res < 0) {
			fprintf(stderr, "Unable to read(): %ls\n", hid_error(handle));
			break;
		}

		i++;
		if (i >= 10) { /* 10 tries by 500 ms - 5 seconds of waiting*/
			fprintf(stderr, "read() timeout\n");
			break;
		}

#ifdef _WIN32
		Sleep(50);
#else
		usleep(50*1000);
#endif
	}

	if (res > 0) {
        int num_words_received = response[0] >> (24 + 2);
        // printf("num_words_received: %d\n", num_words_received);
        for(int i = 1; i < num_words_received + 1; i++){
            printf("%x\n", response[i]);
        }
    }

	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}
