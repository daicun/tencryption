#ifndef _TE_CRYPTO_H
#define _TE_CRYPTO_H

#include <openssl/aes.h>
// #include <aes.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 4)
#define AES_128BIT_SIGNATURE "\x08\x88\x41\xF3\xCD\x83\xE8\xC0\x8F\x29\x03\xDC\x31\xE5\xF5\xA8"
#define AES_128BIT_SIGNATURE_LENGTH 16

#define MAJOR_VALUE 1
#define MINOR_VALUE 0

struct common_info
{
    unsigned char   messages[64];   // display messages
    unsigned char   signature[16];  // signature code
    unsigned short  major;          // major version
    unsigned short  minor;          // minor version
    unsigned char   pname[256];     // process name
    unsigned char   reserve[44];    // reserved space
};

#define FILE_TYPE_NORMAL 0x00a0 // normal file
#define FILE_TYPE_RIGHTS 0x00a1 // encrypted file

struct kernel_info
{
    unsigned short  file_type;      // FILE_TYPE_NORMAL or FILE_TYPE_RIGHTS
    unsigned char   reserve[126];   // reserved space
};

struct file_header {
    struct common_info common;          // common info
    unsigned char reserve[28 * 128];    // reserved space
    struct kernel_info kernel;          // kernel info
};

#pragma pack(pop)

void aes_encrypt_128bit(unsigned char *in, unsigned char *out);
void aes_decrypt_128bit(unsigned char *in, unsigned char *out);

#ifdef __cplusplus
}
#endif
#endif
