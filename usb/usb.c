#include <log.h>
#include <util.h>
#include "usb.h"

#define USB_STATE_BUFFER_SIZE 32

int usb_connector_state(void)
{
    if (access(USB_STATE_PATH, F_OK) != 0)
    {
        // LOGD("USB_STATE_PATH is %d", access(USB_STATE_PATH, F_OK));
        goto return_usb_result_disconnected;
    }

    int fd = open(USB_STATE_PATH, O_RDONLY);
    if (fd > 0)
    {
        char buffer[USB_STATE_BUFFER_SIZE] = { 0 };
        // LOGD("fd: %d", fd);
        // memset(buffer, 0, USB_STATE_BUFFER_SIZE);
        if (read(fd, buffer, USB_STATE_BUFFER_SIZE) > 0)
        {
            // LOGD("get data: %s lenth: %d", buffer, strlen(buffer));
            // LOGD("cmp: %d", memcmp(buffer, USB_STATE_VALUE_CONNECTED,  strlen(buffer)));
            // LOGD("cmp: %d", memcmp(buffer, USB_STATE_VALUE_CONFIGURED,  strlen(buffer)));
            // LOGD("cmp: %d", memcmp(buffer, USB_STATE_VALUE_DISCONNECTED,  strlen(buffer)));
            // for (size_t i = 0; i < strlen(buffer); i++)
            // {
            //     if (isspace(buffer[i]))
            //     {
            //         LOGD("%x", buffer[i]);
            //     }
            // }
            // remove space
            trim(buffer);

            if (! memcmp(buffer, USB_STATE_VALUE_CONNECTED,  strlen(buffer)))
            {
                // LOGD("state: %s", USB_STATE_VALUE_CONNECTED);
                goto return_usb_result_connected;
            }
            else if (! memcmp(buffer, USB_STATE_VALUE_CONFIGURED,  strlen(buffer)))
            {
                // LOGD("state: %s", USB_STATE_VALUE_CONFIGURED);
                goto return_usb_result_connected;
            }
            // else if (! memcmp(buffer, USB_STATE_VALUE_DISCONNECTED,  strlen(buffer)))
            // {
            //     LOGD("usb state: %s", buffer);
            // }
        }
        close(fd);
    }

return_usb_result_disconnected:
    // LOGD("usb is disconnected");
    return USB_RESULT_DISCONNECTED;

return_usb_result_connected:
    close(fd);
    return USB_RESULT_CONNECTED;
}
