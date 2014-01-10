#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <usb.h>
#include <libusb.h>
#include <stdint.h>

#define _GNU_SOURCE
#include <getopt.h>


#define I_VENDOR_NUM        0x1d50
#define I_VENDOR_STRING     "www.ncrmnt.org"
#define I_PRODUCT_NUM       0x6032
#define I_PRODUCT_STRING    "SPI Loopback Test"


int main(int argc, char* argv[])
{
	unsigned int testv =0x7f;
	if (argc>1)
		testv = atoi(argv[1]);

	usb_dev_handle *h;
	h = nc_usb_open(I_VENDOR_NUM, I_PRODUCT_NUM, I_VENDOR_STRING, I_PRODUCT_STRING, NULL);
	if (!h) {
		fprintf(stderr, "No USB device found ;(\n");
		exit(0);
	}
	usb_set_configuration(h, 1);
	usb_claim_interface(h, 0);
	char buffer[128];
	int bytes = usb_control_msg(
                h,             // handle obtained with usb_open()
                USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, // bRequestType
                0,      // bRequest
                testv,              // wValue
                0,              // wIndex
                buffer,             // pointer to destination buffer
                128,  // wLength
                6000
                );
	buffer[127]=0x0;
	if (bytes)
		printf(buffer);
       	
}
