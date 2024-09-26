#include <stdio.h>
#include <libusb.h>

//#define VID 0x0b05
//#define PID 0x19b6
#define VID 0x1CCF
#define PID 0x8010

#define INTERRUPT_SIZE 0x10
#define BULK_SIZE 64

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
    int rtnval, i, j, k;
    libusb_device *dev = NULL;
    libusb_device_handle *dev_handle = NULL;
    struct libusb_config_descriptor *config = NULL;
    struct libusb_interface *interface = NULL;
    struct libusb_interface_descriptor *iface_desc = NULL;
    struct libusb_endpoint_descriptor *endpoint = NULL;

    libusb_init_context(NULL, NULL, 0);

    dev_handle = libusb_open_device_with_vid_pid(NULL, VID, PID); // to be freed
    if (dev_handle == NULL) {
        printf("[!] dev_handle not found. vid: 0x%04hx hid: 0x%04hx\n", VID, PID);
        goto CLEAN;
    }

    dev = libusb_get_device(dev_handle);
    libusb_get_config_descriptor(dev, 0, &config); // to be freed
    printf("[*] config->bNumInterfaces: %hhu\n", config->bNumInterfaces);
    // TODO: drier sends first config desc back again, do I do that here?
    
    for (i=0; i<config->bNumInterfaces; i++) {
        interface = &config->interface[i];
        printf("[*] interface[%d]->num_altsetting: %d\n", i, interface->num_altsetting);
        for (j=0; j<interface->num_altsetting; j++) {
            iface_desc = &interface->altsetting[j];
            printf("[*] iface[%d]->bNumEndPoints: %hhu\n", j, iface_desc->bNumEndpoints);

            for (k=0; k<iface_desc->bNumEndpoints; k++) {
                endpoint = &iface_desc->endpoint[k];
                printf("[*] endpoint[%d] addr:0x%02hhx %s\n", k, endpoint->bEndpointAddress, endpoint->bEndpointAddress>>7 == 1 ? "IN" : "OUT");
                printf("[*] wMaxPacketSize: %hu\n", endpoint->wMaxPacketSize);
                printf("[*] transfer type: ");
                switch (endpoint->bmAttributes & 3) {
                    case 0:
                    printf("Control");
                    break;
                    case 1:
                    printf("Isochronous");
                    break;
                    case 2:
                    printf("Bulk");
                    break;
                    case 3:
                    printf("Interrupt");
                    break;
                }
                printf("\n\n");
            }
            printf("\n");
        }
        printf("\n");
    }

    libusb_free_config_descriptor(config);
    libusb_close(dev_handle);

CLEAN:
    libusb_exit(NULL);
}

