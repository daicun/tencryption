#ifndef _TE_USB_H
#define _TE_USB_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#define USB_STATE_PATH "/sys/class/android_usb/android0/state"

#define USB_STATE_VALUE_CONNECTED    "CONNECTED"
#define USB_STATE_VALUE_CONFIGURED   "CONFIGURED"
#define USB_STATE_VALUE_DISCONNECTED "DISCONNECTED"

#define USB_RESULT_CONNECTED    1001
#define USB_RESULT_DISCONNECTED 1002

int usb_connector_state(void);

#ifdef __cplusplus
}
#endif
#endif
