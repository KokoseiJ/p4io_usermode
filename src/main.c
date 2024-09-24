#include <stdio.h>
#include <libusb.h>

#define VID 0x1CCF
#define PID 0x8010


int main() {
    int rtnval, i, j, k;
    libusb_device *dev = NULL;
    libusb_device_handle *dev_handle = NULL;
    struct libusb_config_descriptor *config = NULL;
    struct libusb_interface *interface = NULL;
    struct libusb_interface_descriptor *iface_desc = NULL;
    struct libusb_endpoint_descriptor *endpoint = NULL;

    libusb_init_context();

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
        printf("[*] interface->num_altsetting: %d\n", interface->num_altsetting);
        for (j=0; j<interface->num_altsetting; j++) {
            iface_desc = &interface->altsetting[j];
            printf("[*] iface->bNumEndPoints: %hhu\n", iface_desc->bNumEndpoints);

            for (i=0; i<iface->bNumEndPoints; i++) {
                endpoint = &iface->bNumEndPoints[i]
                printf("[*] endpoint[%d] addr:0x%02hhx %s\n", endpoint->bEndPointAddress, endpoint->bEndPointAddress>>7 == 1 : "IN" : "OUT");
                printf("[*] wMaxPacketSize: %hu\n", endpoint->wMaxPacketSize);
            }
        }
    }

    libusb_free_config_descriptor(config);
    libusb_close(dev_handle);

label CLEAN:
    libusb_exit(NULL);
}

