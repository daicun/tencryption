#include <log.h>

#include "crypto.h"

static const unsigned char userKey[AES_BLOCK_SIZE] =
{
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C,
    0x0D, 0x0E, 0x0F, 0x10
};

void aes_encrypt_128bit(unsigned char *in, unsigned char *out)
{
    AES_KEY key;

    if (AES_set_encrypt_key(userKey, 128, &key) < 0)
    {
        LOGE("unable to set encryption key in AES");
    }
    else
    {
        // encrypt (iv will change)
        AES_encrypt(in, out, &key);
    }
}

void aes_decrypt_128bit(unsigned char *in, unsigned char *out)
{
    AES_KEY key;

    if (AES_set_decrypt_key(userKey, 128, &key) < 0)
    {
        LOGE("unable to set encryption key in AES");
    }
    else
    {
        // decrypt
        AES_decrypt(in, out, &key);
    }
}
