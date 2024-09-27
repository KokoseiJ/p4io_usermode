#include <stdio.h>
#include <string.h>
#include <time.h>
#include <libusb.h>

//#define VID 0x0b05
//#define PID 0x19b6
#define VID 0x1CCF
#define PID 0x8010

#define INTERRUPT_SIZE 0x10
#define BULK_SIZE 64
#define LIGHTS_SIZE 0x10
#define INT_SIZE 0x10

#define P4IO_CMD_LIGHTS 0x12

#define USB_FRAME 1000
#define USB_NANOFRAME 125

/*
LIBUSB_ENDPOINT_TRANSFER_TYPE_CONTROL
Control endpoint.

LIBUSB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS
Isochronous endpoint.

LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK
Bulk endpoint.

LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT
Interrupt endpoint.
*/

int main() {
    int rtnval, i, j, transferred, interval_usec, int_buffer[4];
    libusb_device *dev = NULL;
    libusb_device_handle *dev_handle = NULL;
    struct libusb_config_descriptor *config = NULL;
    struct libusb_interface_descriptor *iface = NULL;
    struct libusb_endpoint_descriptor *bulk_out, *bulk_in, *intr_in;

    struct timespec t = {1, 0};

    uint8_t lights_buffer[LIGHTS_SIZE] = {0,}, intr_interval, speed;

    libusb_init_context(NULL, NULL, 0);

    dev_handle = libusb_open_device_with_vid_pid(NULL, VID, PID); // to be freed
    if (dev_handle == NULL) {
        printf("[!] dev_handle not found. vid: 0x%04hx hid: 0x%04hx\n", VID, PID);
        goto CLEAN;
    }

    dev = libusb_get_device(dev_handle);

    speed = libusb_get_device_speed(dev);

    printf("[*] Device speed is: ");
    switch (speed) {
        case LIBUSB_SPEED_UNKNOWN:
        printf("sorrgy accident");
        break;
        case LIBUSB_SPEED_LOW:
        printf("yo this shit slow af");
        break;
        case LIBUSB_SPEED_FULL:
        printf("Full Speed");
        break;
        case LIBUSB_SPEED_HIGH:
        printf("im going fast as fuck boi");
        break;
        case LIBUSB_SPEED_SUPER:
        printf("eminem");
        break;
        case LIBUSB_SPEED_SUPER_PLUS:
        printf("FUUUUUTURE   FUUUUUUUUUUTUUUUREEEEEE");
        break;
    }
    printf("\n");

    libusb_get_config_descriptor(dev, 0, &config); // to be freed
    iface = &config->interface->altsetting[0];
    bulk_out = &iface->endpoint[0];
    bulk_in = &iface->endpoint[1];
    intr_in = &iface->endpoint[2];

    printf("[*] Starting lights test\n\n");

    for (i=0; i<LIGHTS_SIZE; i++) {
        memset(lights_buffer, 0, LIGHTS_SIZE);
        lights_buffer[i] = 255;
        transferred = 0;

        printf("[*] Lights %d\n", i);

        rtnval = libusb_bulk_transfer(dev_handle, bulk_out->bEndpointAddress, lights_buffer, LIGHTS_SIZE, &transferred, 0);
        
        printf("rtnval: %d transferred: %d\n", rtnval, transferred);

        nanosleep(&t, NULL);
    }

    printf("\n[*] Starting input read\n");

    intr_interval = intr_in->bInterval;

    printf("[*] bInterval: %hhd ", intr_interval);

    if (speed >= LIBUSB_SPEED_HIGH)
        interval_usec = (1 << (intr_interval - 1)) * 125;
    else
        interval_usec = intr_interval * 1000;

    printf("(%d usec)\n", interval_usec);

    t.tv_sec = 0;
    t.tv_nsec = interval_usec * 1000;

    while (1) {
        memset(int_buffer, 0, INT_SIZE);
        transferred = 0;

        rtnval = libusb_interrupt_transfer(dev_handle, intr_in->bEndpointAddress, (uint8_t *) int_buffer, INT_SIZE, &transferred, 0);

        for (i=0; i<4; i++) {
            for (j=0; j<32; j++) {
                printf( ( int_buffer[3 - i] >> (31 - j) ) & 1 ? "1" : "0" );
            }
        }
        printf("\t%d %d   \r");
	nanosleep(&t, NULL);
    }

    libusb_free_config_descriptor(config);
    libusb_close(dev_handle);

CLEAN:
    libusb_exit(NULL);
}

