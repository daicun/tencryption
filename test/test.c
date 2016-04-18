#include <usb/usb.h>
#include <user/user.h>
#include <crypto/crypto.h>

int main(int argc, char const *argv[])
{
    fprintf(stderr, "te system test:\n");
    fprintf(stderr, "......usb state: %d\n", usb_connector_state());
    fprintf(stderr, "...........puid: %d\n", getpuid());
    fprintf(stderr, "...........paid: %d\n", getpaid());
    fprintf(stderr, "...........pdid: %d\n", getpdid());
    fprintf(stderr, "..........panme: %s\n", getpname());
    while (1);
    return 0;
}
